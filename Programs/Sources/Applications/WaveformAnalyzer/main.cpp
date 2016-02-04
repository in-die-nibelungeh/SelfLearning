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

#include "mutl.h"

#include "Common.h"

static void usage(void)
{
    LOG("Usage: %s INPUT \n", PROGRAM_NAME);
    LOG("\n");
    LOG("INPUT must be .wav files.\n");
    LOG("  -h: display this help.\n");
    LOG("  -o: spefity an output filename.\n");
    LOG("  -d: spefity an output directory, which should already exist.\n");
    LOG("  -w: spefity a window length of fft, which must be factrial of 2.\n");
    LOG("  -wt: spefity a window type, which accepts only \"rec\", \"han\", \"ham\", \"blk\", or \"hrs\".\n");
    LOG("  -l: spefity a sample length used in analyzing.\n");
    LOG("  -ft: spefity to use only ft.\n");
    LOG("  -amp: spefity to output in amplitude.\n");
    LOG("  -10log: spefity to output in 10 * log.\n");
    LOG("  -20log: spefity to output in 20 * log.\n");
    LOG("  -deg: spefity to output in degree.\n");
    LOG("  -rad: spefity to output in radian.\n");
}

// Description
typedef struct mutl::ArgumentDescription Desc;

const Desc descs[] =
{
    {"h" , 0},
    {"ft" , 0},
    {"rad" , 0},
    {"deg" , 0},
    {"amp" , 0},
    {"10log" , 0},
    {"20log" , 0},
    {"w" , 1},
    {"wt" , 1},
    {"l" , 1},
    {"d" , 1},
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

    // Display messages real-time
    setvbuf(stdout, NULL, _IONBF, 0);

    ProgramParameter param;
    param.sampleCount = 0;
    param.windowLength = 0;
    param.windowType = WindowType_Rectangular;
    param.isUsedOnlyFt = false;
    param.gainFormat = GainFormat_Amplitude;
    param.argFormat = ArgFormat_Radian;
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
    if ( parser.IsEnabled("wt") )
    {
        const std::string windowType = parser.GetOption("wt");
        if (windowType == std::string("rec"))
        {
            param.windowType = WindowType_Rectangular;
        }
        else if (windowType == std::string("han"))
        {
            param.windowType = WindowType_Hanning;
        }
        else if (windowType == std::string("ham"))
        {
            param.windowType = WindowType_Hamming;
        }
        else if (windowType == std::string("blk"))
        {
            param.windowType = WindowType_Blackman;
        }
        else if (windowType == std::string("hrs"))
        {
            param.windowType = WindowType_BlackmanHarris;
        }
        else
        {
            ERROR_LOG("The value specified with -wt must be \"rec\", \"han\", \"ham\", \"blk\", or \"hrs\": %s\n", windowType.c_str());
            return 0;
        }
    }
    if ( parser.IsEnabled("l") )
    {
        param.sampleCount = atoi( parser.GetOption("l").c_str() );
    }
    if ( parser.IsEnabled("10log") )
    {
        param.gainFormat = GainFormat_10Log;
    }
    else if ( parser.IsEnabled("20log") )
    {
        param.gainFormat = GainFormat_20Log;
    }
    else if ( parser.IsEnabled("amp") )
    {
        param.gainFormat = GainFormat_Amplitude;
    }
    if ( parser.IsEnabled("rad") )
    {
        param.argFormat = ArgFormat_Radian;
    }
    else if ( parser.IsEnabled("deg") )
    {
        param.argFormat = ArgFormat_Degree;
    }

    LOG("[Input]\n");
    LOG("    InputFile   : %s\n", param.inputFilepath.c_str());
    LOG("    OutputFile  : %s\n", param.outputBase.c_str());
    LOG("    Window      : %d\n", param.windowLength);
    LOG("    WindowType  : %d\n", param.windowType);
    LOG("    Sample      : %d\n", param.sampleCount);
    LOG("    Process     : %s\n", param.isUsedOnlyFt ? "ft" : "fft");
    LOG("    Gain        : %d\n", param.gainFormat);
    LOG("    Argument    : %d\n", param.argFormat);

    PRINT_RETURN_IF_FAILED( Setup(&param) );
    LOG("    SignalLength: %d\n", param.signal.GetColumnLength());
    LOG("    SamplingRate: %d\n", param.samplingRate);
    LOG("\n");

    PRINT_RETURN_IF_FAILED( PreProcess(&param) );
    LOG("[PreProcessed]\n");
    LOG("    SignalLength: %d\n", param.signal.GetColumnLength());
    LOG("    Window      : %d\n", param.windowLength);
    LOG("    Count       : %d (%d)\n", param.signal.GetColumnLength() / param.windowLength, param.signal.GetColumnLength() % param.windowLength);
    LOG("    Process     : %s\n", param.isUsedOnlyFt ? "ft" : "fft");

    PRINT_RETURN_IF_FAILED( Process(&param) );

    return 0;
}
