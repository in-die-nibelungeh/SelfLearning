/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2015-2016 Ryosuke Kanata
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

#include <math.h>

#include "mcon.h"
#include "mfio.h"
#include "masp.h"

#include "Common.h"

namespace {

status_t CaculateEnergy(const mcon::Matrix<double>& input, const std::string& outputBase)
{
    // Energy
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
    mfio::Csv csv(outputBase + ecsv);
    csv.Write("Id");
    for (uint c = 0; c < ch; ++c)
    {
        csv.Write(",Enegy,Energy ratio");
    }
    csv.Write("\n");
    csv.Write(energyRatio);
    csv.Close();

    return NO_ERROR;
}

status_t CaculateSpectrum(const ProgramParameter* param)
{
    const mcon::Matrix<double>& input = param->signal;
    const size_t ch = input.GetRowLength();
    const size_t N = param->windowLength;
    mcon::Matrix<double> matrix(2 * ch + 1, N);
    mcon::Vector<double> window(N);

    switch(param->windowType)
    {
        case WindowType_Rectangular:
            masp::window::Rectangular(window);
            break;
        case WindowType_Hamming:
            masp::window::Hamming(window);
            break;
        case WindowType_Blackman:
            masp::window::Blackman(window);
            break;
        case WindowType_BlackmanHarris:
            masp::window::BlackmanHarris(window);
            break;
        case WindowType_Hanning:
        default:
            masp::window::Hanning(window);
            break;
    }

    for (uint i = 0; i < N; ++i)
    {
        matrix[0][i] = 1.0 * i / N * param->samplingRate;
    }
    const double windowEnergy = sqrt(window.Dot(window) / window.GetLength()) ;
    DEBUG_LOG("WindowEnergy=%g\n", windowEnergy);
    for (uint c = 0; c < ch; ++c)
    {
        const int count = input.GetColumnLength() / param->windowLength;
        const int rest = (input.GetColumnLength() % param->windowLength);
        ASSERT( rest == 0 );
        UNUSED(rest);

        mcon::Matrix<double> sum(2, N);
        sum = 0;
        // 幅の半分ずつ解析する。
        for (int k = 0; k < 2 * count + 1; ++k)
        {
            mcon::Vector<double> part(N);
            mcon::Matrix<double> polar;
            mcon::Matrix<double> complex;
            // 先頭と末尾は特別に処理する
            if (k == 0 || k == 2 * count)
            {
                const double* src = input[c];
                const int copySize = sizeof(double) * N / 2;
                double* dst = part;
                part = 0;
                if (k == 0)
                {
                    std::memcpy(dst + N / 2, src, copySize);
                }
                else
                {
                    std::memcpy(dst, src + (2 * count - 1) * N / 2, copySize);
                }
            }
            else
            {
                // k == 0 は入らないので、負のインデックスにはならない。
                const int head = N * (k - 1) / 2;
                part = static_cast<mcon::Vectord>(input[c])(head, N);
            }
            part *= window;

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
        // 振幅に対してのみ、窓関数のエネルギに応じた補正をする。
        sum[0] /= windowEnergy;
        if (param->gainFormat == GainFormat_10Log
            || param->gainFormat == GainFormat_20Log)
        {
            const double gain = param->gainFormat == GainFormat_10Log ? 10.0 : 20.0;
            for (uint i = 0; i < N; ++i)
            {
                sum[0][i] = gain * log10(sum[0][i]);
            }
        }
        if (param->argFormat == ArgFormat_Degree)
        {
            for (uint i = 0; i < N; ++i)
            {
                sum[1][i] = sum[1][i] / M_PI * 180.0;
            }
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

    return NO_ERROR;
}

}

status_t Process(const ProgramParameter* param)
{
    RETURN_IF_FAILED( CaculateEnergy(param->signal, param->outputBase) );

    RETURN_IF_FAILED( CaculateSpectrum(param) );

    return NO_ERROR;
}
