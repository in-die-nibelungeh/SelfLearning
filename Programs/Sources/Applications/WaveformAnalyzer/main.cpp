/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2015 Ryosuke Kanata
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include <string>

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "status.h"
#include "types.h"
#include "debug.h"

#include "mcon.h"
#include "mfio.h"
#include "masp.h"
#include "mutl.h"

#include "Common.h"

/*
 * サンプル数が 512 より小さい場合は ft を使う。
 *   処理時間は問題ない規模のはず。
 *
 */


int GetU1Position(uint value)
{
    if (0 == value)
    {
        return -1;
    }
    for (int k = (sizeof(uint) * 8 - 1); k >= 0; --k)
    {
        if (value & (1 << k))
        {
            return k;
        }
    }
    ASSERT(false);
    return -2;
}

size_t GetLowerLimitSize(uint frequency)
{
    const uint tmp = frequency / 100;
    size_t size = 0;

    for (uint k = 31; k >= 0; --k)
    {
        if (tmp & (1 << k))
        {
            size = 1 << k;

            const int m = k - 1;
            if (m >= 0)
            {
                size <<= ( tmp & (1 << m) ) ? 1 : 0;
            }
            break;
        }
    }
    return size;
}

size_t GetN(size_t n, uint samplingRate)
{
    const size_t upperWidth = ( 1 << GetU1Position(n * 2 - 1) );
    const size_t lowerWidth = GetLowerLimitSize(samplingRate);
    int caseCount = 0;
    LOG("upperWidth: %ld (%08lx)\n", upperWidth, upperWidth);
    LOG("lowerWidth: %ld (%08lx)\n", lowerWidth, lowerWidth);

    for ( ;upperWidth != (lowerWidth << caseCount); ++caseCount);
    caseCount++;
    LOG("case: %d\n", caseCount);

    mcon::Vector<double> ratio(caseCount);

    for (uint w = lowerWidth, k = 0; w <= upperWidth; ++k, w *= 2)
    {
        const uint rest = n % w;
        ratio[k] = static_cast<double>(rest) / w;
        printf("n=%ld, %5d: %f (%d)\n", n, w, static_cast<double>(rest) / w, rest);
    }
    const double max = ratio.GetMaximum();
    const double min = ratio.GetMinimum();

    const double searchTarget = (1.0 - max < min) ? max : min;
    int index = -1;

    for (int k = ratio.GetLength() - 1; k >= 0 ; --k)
    {
        if (ratio[k] == searchTarget)
        {
            index = k;
            break;
        }
    }
    ASSERT(index != -1);
    printf("Size: %ld\n", lowerWidth << index);
    return (lowerWidth << index);
}

bool IsFactorial(const uint _value, const uint base)
{
    uint value = _value;
    if (0 == base || 0 == value)
    {
        return false;
    }
    for ( ; value > 1; value /= base )
    {
        if (0 != (value % base))
        {
            return false;
        }
    }
    return true;
}

static void usage(void)
{
    printf("Usage: %s INPUT \n", "wanalyzer");
    printf("\n");
    printf("INPUT must be .wav files.\n");
    printf("  -o: spefity an output filename.\n");
    printf("  -d: spefity an output directory, which should already exist.\n");
}

// Description
typedef struct mutl::ArgumentDescription Desc;

const Desc descs[] =
{
    {"h" , 0},
    {"d" , 1},
    {"ft" , 0},
    {"w" , 1},
    {"l" , 1},
    {"o" , 1}
};

int main(int argc, const char* argv[])
{
#if 0
    const uint freqs[] =
    {
        4000,
        8000,
        16000,
        32000,
        44100,
        48000,
        96000,
        192000
    };
    for (uint k = 0; k < sizeof(freqs)/sizeof(uint); ++k)
    {
        const size_t size = GetLowerLimitSize(freqs[k]);
        printf("%3dk: %ld\n", freqs[k]/1000, size);
    }

    const uint base = 3;
    for (uint k = 0; k < 33; ++k)
    {
        LOG("%d: %s\n", k, IsFactorial(k, base) ? "yes" : "no ");
    }
#endif
    mutl::ArgumentParser parser;
    if( false == parser.Initialize(argc, argv, descs, sizeof(descs)/sizeof(Desc))
        || parser.IsEnabled("h")
        || parser.GetArgumentCount() < 1 )
    {
        usage();
        return 0;
    }
    ProgramParameter param;
    param.sampleCount = 0;
    param.windowLength = 0;
    param.isUsedOnlyFt = false;
    param.inputFilepath = parser.GetArgument(0);

    const mutl::NodePath inputPath(param.inputFilepath);
    std::string outfile = inputPath.GetBasename();
    if (parser.IsEnabled("o"))
    {
        outfile = parser.GetOption("o");
    }
    std::string outdir("./");
    if ( parser.IsEnabled("d") )
    {
        outdir = parser.GetOption("d");
        outdir += std::string("/");
    }
    param.outputBase = outdir + outfile;

    if ( parser.IsEnabled("ft") )
    {
        param.isUsedOnlyFt = true;
    }
    if ( parser.IsEnabled("w") )
    {
        const uint width  = atoi( parser.GetOption("w").c_str() );
        if ( !IsFactorial(width, 2) )
        {
            ERROR_LOG("The value specified with -w must be 2^n: %d\n", width);
            return 0;
        }
        param.windowLength = width;
    }
    if ( parser.IsEnabled("l") )
    {
        param.sampleCount = atoi( parser.GetOption("l").c_str() );
    }

    LOG("Input : %s\n", param.inputFilepath.c_str());
    LOG("Output: %s\n", param.outputBase.c_str());
    LOG("Window: %d\n", param.windowLength);
    LOG("Sample: %d\n", param.sampleCount);

    Setup(&param);
    LOG("Length: %d\n", param.signal.GetColumnLength());

    PreProcess(&param);
    LOG("[PreProcessed]\n");
    LOG("Length: %d\n", param.signal.GetColumnLength());
    LOG("Window: %d\n", param.windowLength);
    LOG("Sample: %d\n", param.sampleCount);
    LOG("Count : %d (%d)\n", param.signal.GetColumnLength() / param.windowLength, param.signal.GetColumnLength() % param.windowLength);

    Process(&param);

    return 0;
}

status_t Setup(ProgramParameter* param)
{
    const std::string& inputFile = param->inputFilepath;
    // Input
    LOG("Loading input ... ");
    mfio::Wave wave;
    status_t status = wave.Read(inputFile, param->signal);
    if (NO_ERROR != status)
    {
        printf("An error occured during reading %s: error=%d\n", inputFile.c_str(), status);
        return -ERROR_CANNOT_OPEN_FILE;
    }
    LOG("Done\n");
    param->samplingRate = wave.GetSamplingRate();
    return NO_ERROR;
}

/*
sampleCount が指定されている
  ==> windowLength を決める。
  ==> 解析に使わるサンプル数は、指定された値から変わる可能性がある。

windowLength が指定されている
  ==> sampleCount を決める。
  ==> windowLength は変わらない。

sampleCount も windowLength も指定されている
  ==> sampleCount を決める。
*/

status_t PreProcess(ProgramParameter* param)
{
    // 指定されたサンプル数がデータのサンプル数より多い場合はクランプ。
/*
    const uint sampleCount = param->signal.GetColumnLength();
    if (param->sampleCount > sampleCount)
    {
        param->sampleCount = sampleCount;
    }
*/

    const uint signalLength = param->signal.GetColumnLength();
    //----------------------------------------------------------------
    // 以下のブロックでは、ウィンドウサイズとサンプル数を決定する。
    //----------------------------------------------------------------
    // ウィンドウサイズが未指定の場合
    if (0 == param->windowLength)
    {
        const uint sampleCount = param->sampleCount > 0 ? param->sampleCount : signalLength;
        // サンプル長が2 の階乗なら FFT で良い。
        if ( IsFactorial(sampleCount, 2) )
        {
            param->isUsedOnlyFt = false;
            param->windowLength = sampleCount;
        }
        else
        {
            param->windowLength = (true == param->isUsedOnlyFt) ? sampleCount : GetN(sampleCount, param->samplingRate);
        }
    }
    // ウィンドウサイズが指定されている場合
    else
    {
        // ウィンドウ幅が無駄に長い場合は短くする。
        if (param->windowLength > (signalLength << 1))
        {
            param->windowLength = 1 << (GetU1Position(signalLength) + 1);
        }
    }

    const uint width = param->windowLength;
    const uint sampleCount = param->sampleCount > 0 ? param->sampleCount : signalLength;
    const double ratio = static_cast<double>(sampleCount % width) / width;
    const int windowCount = (sampleCount / width) + (ratio >= 0.5f ? 1 : 0);
    const uint sampleCountUsed = windowCount * width;

    // サイズを変更する。
    if (sampleCountUsed != signalLength)
    {
        const size_t ch = param->signal.GetRowLength();
        const uint N = sampleCountUsed;
        const mcon::Matrix<double> tmp(param->signal);

        param->signal.Resize(ch, N);
        param->signal = 0;

        for (uint k = 0; k < ch; ++k)
        {
            param->signal[k].Copy(tmp[k]);
        }
    }

    return NO_ERROR;
}

status_t Process(const ProgramParameter* param)
{
    const mcon::Matrix<double>& input = param->signal;

    LOG("[Input]\n");
    LOG("    SamplingRate: %d\n", param->samplingRate);
    LOG("    Channels    : %d\n", input.GetRowLength());
    LOG("    Length      : %d\n", input.GetColumnLength());

    // Energy
    {
        const size_t ch = input.GetRowLength();
        const size_t N = input.GetColumnLength();
        mcon::Matrix<double> energyRatio(2 * ch, N);

        for (uint c = 0; c < ch; ++c)
        {
            const mcon::Vector<double>& _input = input[c];
            const double energy = sqrt(_input.GetDotProduct(_input));
            LOG("    Energy[%d-ch]: %g\n", c, energy);
            double sum = 0;
            for ( uint k = 0; k < N; ++k )
            {
                sum += _input[k] * _input[k];
                energyRatio[2 * c + 0][k] = sqrt(sum);
                energyRatio[2 * c + 1][k] = sqrt(sum) / energy;
            }
        }
        const std::string ecsv("_energy.csv");
        mfio::Csv csv(param->outputBase + ecsv);
        csv.Write("Id");
        for (uint c = 0; c < ch; ++c)
        {
            csv.Write(",Enegy,Energy ratio");
        }
        csv.Write("\n");
        csv.Write(energyRatio);
        csv.Close();
    }

    {
        LOG("\n");
        const size_t ch = input.GetRowLength();
        const size_t width = param->windowLength;
        mcon::Matrix<double> matrix(2 * ch + 1, width);

        for (uint i = 0; i < width; ++i)
        {
            matrix[0][i] = 1.0 * i / width * param->samplingRate;
        }

        for (uint c = 0; c < ch; ++c)
        {
            const int count = input.GetColumnLength() / param->windowLength;
            const int rest = (input.GetColumnLength() % param->windowLength);
            ASSERT( rest == 0 );
            UNUSED(rest);

            mcon::Matrix<double> sum(2, width);
            sum = 0;
            for (int k = 0; k < count; ++k)
            {
                mcon::Matrix<double> polar;
                mcon::Matrix<double> complex;

                mcon::Vector<double> part = input[c](width * k, width);
                if (true == param->isUsedOnlyFt)
                {
                    masp::ft::Ft(complex, part);
                }
                else
                {
                    masp::ft::Fft(complex, part);
                }
                masp::ft::ConvertToPolarCoords(polar, complex);
                sum[0] += polar[0];
                sum[1] += polar[1];
            }
            matrix[c * 2 + 1] = sum[0];
            matrix[c * 2 + 2] = sum[1];
        }

        const std::string ecsv("_spectrum.csv");

        mfio::Csv csv(param->outputBase + ecsv);
        csv.Write("Id,Frequency");
        for (uint c = 0; c < ch; ++c)
        {
            csv.Write(",Amplitude,Argument");
        }
        csv.Write("\n");
        csv.Write(matrix);
        csv.Close();
        LOG("Done\n");
    }
    LOG("Finished.\n");
    return NO_ERROR;
}
