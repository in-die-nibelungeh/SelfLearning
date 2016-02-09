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

// Description
typedef struct mutl::ArgumentDescription Desc;

namespace {
    const Desc descs[] =
    {
        {"h" , 0},
        {"o" , 1},
        {"d" , 1},
        {"c" , 1},
        {"l" , 1},
        {"m" , 1},
        {"opt" ,0},
        {"sep" ,0},
        {"log" ,0}
    };
    ProgramParameter param = ProgramParameter();

    void usage(const ProgramParameter& param)
    {
        LOG("Usage: %s [OPTIONS] INPUT REFERENCE\n", "rls");
        LOG("\n");
        LOG("INPUT and REFERENCE must be .wav files.\n");
        LOG("  -h: show help.\n");
        LOG("  -o: spefity the output filename.\n");
        LOG("  -d: spefity an output directory, which should already exist.\n");
        LOG("  -m: spefity the tapps, which should be a positive number.\n");
        LOG("  -c: spefity the absolute value clamped at leveling state (default=%f).\n", param.upperValue);
        LOG("  -l: spefity to use input signal with the specified length.\n");
        LOG("  -opt: spefity to optimize so as to minimize error.\n");
        LOG("  -sep: spefity to optimize each channel separately.\n");
        LOG("  -log: spefity to output a log file on the process of optimization.\n");
    }
}

int main(int argc, const char* argv[])
{
    mutl::ArgumentParser parser;

    if( false == parser.Initialize(argc, argv, descs, sizeof(descs)/sizeof(Desc))
        || parser.IsEnabled("h")
        || parser.GetArgumentCount() < 2 )
    {
        usage(param);
        return 0;
    }
    param.upperValue = 32767.0;
    param.optimize = false;
    param.optimizeSeparately = false;
    param.outputLog = false;
    param.inputLength = -1;

    param.inputFilepath = parser.GetArgument(0);
    param.referenceFilepath = parser.GetArgument(1);

    size_t& tapps = param.tapps;

    tapps = 256;
    if (parser.IsEnabled("m"))
    {
        tapps = atoi( parser.GetOption("m").c_str() );
    }
    if ( tapps < 1 )
    {
        ERROR_LOG("tapps (an argument of \"-m\" swtich) must be larger than 0: %d\n", static_cast<int>(tapps) );
        return 0;
    }
    if (parser.IsEnabled("c"))
    {
        param.upperValue = atof( parser.GetOption("c").c_str() );
        if (param.upperValue <= 0.0)
        {
            ERROR_LOG("The specified value with -c (%f) must be positive.\n", param.upperValue);
            return 0;
        }
    }
    if (parser.IsEnabled("l"))
    {
        param.inputLength = atoi( parser.GetOption("l").c_str() );
        if (param.inputLength < 1)
        {
            ERROR_LOG("The specified value with -l (%d) must be positive.\n", static_cast<int>(param.inputLength) );
            return 0;
        }
    }
    if (parser.IsEnabled("opt"))
    {
        param.optimize = true;
    }
    if (parser.IsEnabled("sep"))
    {
        param.optimizeSeparately = true;
    }
    if (parser.IsEnabled("log"))
    {
        param.outputLog = true;
    }

    std::string outputPrefix;
    if (parser.IsEnabled("o"))
    {
        outputPrefix = parser.GetOption("o");
    }
    else
    {
        mutl::NodePath _inputPath(param.inputFilepath);
        mutl::NodePath _referencePath(param.referenceFilepath);
        char tappsString[11];
        sprintf(tappsString, "%d", static_cast<int>(tapps));
        outputPrefix = std::string("./Af_")
            + std::string(tappsString)
            + std::string("tapps_")
            + _inputPath.GetBasename()
            + std::string("_")
            + _referencePath.GetBasename();
    }

    std::string outdir("./");
    if ( parser.IsEnabled("d") )
    {
        outdir = parser.GetOption("d");
        outdir += std::string("/");
    }
    param.outputBase = outdir + outputPrefix;

    // Display messages real-time
    setvbuf(stdout, NULL, _IONBF, 0);

    LOG("Input: %s\n", param.inputFilepath.c_str());
    LOG("Reference: %s\n", param.referenceFilepath.c_str());
    LOG("Tapps: %d\n", static_cast<int>(tapps) );
    LOG("Clamped Value: %g\n", param.upperValue);
    LOG("\n");

    status_t status;
    status = Setup(&param);
    if ( NO_ERROR != status )
    {
        ERROR_LOG("Failed in Startup(): %d\n", status);
        return 0;
    }
    LOG("[Input]\n");
    LOG("    File        : %s\n", param.inputFilepath.c_str());
    LOG("    SamplingRate: %d\n", static_cast<int>(param.samplingRate) );
    LOG("    Length      : %d\n", static_cast<int>(param.inputSignal.GetColumnLength()) );
    LOG("\n");
    LOG("[Reference]\n");
    LOG("    File  : %s\n", param.referenceFilepath.c_str());
    LOG("    Length: %d\n", static_cast<int>(param.referenceSignal.GetLength()));
    LOG("\n");

    status = PreProcess(&param);
    if ( NO_ERROR != status )
    {
        ERROR_LOG("Failed in PreProcess(): %d\n", status);
        return 0;
    }

    status = Process(&param);
    if ( NO_ERROR != status )
    {
        ERROR_LOG("Failed in Estimater(): %d\n", status);
        return 0;
    }

    status = PostProcess(&param);
    if ( NO_ERROR != status )
    {
        ERROR_LOG("Failed in PostProcess(): %d\n", status);
        return 0;
    }

    status = Cleanup(&param);
    if ( NO_ERROR != status )
    {
        ERROR_LOG("Failed in Cleanup(): %d\n", status);
        return 0;
    }
    return 0;
}

