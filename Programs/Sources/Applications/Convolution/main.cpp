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
#include "mutl.h"

status_t ConvoluteTwoWaveforms(const char* inputFile, const char* systemFile, const std::string& outdir, const std::string& out = "");

status_t Convolution(mcon::Matrix<double>& audioOut, const mcon::Matrix<double>& audioIn, const mcon::Matrix<double>& impluse)
{
    const int M = impluse.GetColumnLength();
    if (audioIn.GetColumnLength() < M )
    {
        return -ERROR_ILLEGAL;
    }

    audioOut.Resize(audioIn.GetRowLength(), audioIn.GetColumnLength());
    for (int ch = 0; ch < audioIn.GetRowLength(); ++ch)
    {
        int ich = ch;
        if ( 1 == impluse.GetRowLength() )
        {
            ich = 0;
        }
        for (int i = 0; i < audioIn.GetColumnLength(); ++i)
        {
            audioOut[ch][i] = 0.0;
            for (int k = 0; k < ( (M - 1 > i) ? i+1 : M); ++k)
            {
                audioOut[ch][i] += audioIn[ch][i - k] * impluse[ich][k];
            }
        }
    }
    return NO_ERROR;
}

static void usage(void)
{
    printf("Usage: %s [OPTIONS] INPUT IMPLUSE\n", "convolution");
    printf("\n");
    printf("INPUT and IMPLUSE must be .wav files.\n");
    printf("  -h: show help.\n");
    printf("  -o: spefity an output filename.\n");
    printf("  -d: spefity an output directory, which should already exist.\n");
}

// Description
typedef struct mutl::ArgumentDescription Desc;

const Desc descs[] =
{
    {"h" , 0},
    {"d" , 1},
    {"o" , 1}
};

int main(int argc, const char* argv[])
{
    mutl::ArgumentParser parser;
    if ( false == parser.Initialize(argc, argv, descs, sizeof(descs)/sizeof(Desc)) )
    {
        usage();
        return 0;
    }

    if ( parser.IsEnabled("h") || parser.GetArgumentCount() < 2 )
    {
        usage();
        return 0;
    }
    const std::string input = parser.GetArgument(0);
    const std::string reference = parser.GetArgument(1);
    std::string outfile;

    std::string outdir("./");
    if ( parser.IsEnabled("d") )
    {
        outdir = parser.GetOption("d");
        outdir += std::string("/");
    }

    if ( parser.IsEnabled("o") )
    {
        outfile = parser.GetOption("o");
    }
    // Display messages real-time
    setvbuf(stdout, NULL, _IONBF, 0);

    LOG("Input: %s\n", input.c_str());
    LOG("Impluse: %s\n", reference.c_str());
    LOG("\n");
    ConvoluteTwoWaveforms(input.c_str(), reference.c_str(), outdir, outfile);


    return 0;
}

status_t ConvoluteTwoWaveforms(const char* inputFile, const char* systemFile, const std::string& outdir, const std::string& outfile)
{
    mcon::Matrix<double> input;
    mcon::Matrix<double> system;
    std::string fbody;
    int fs;

    // Input
    {
        mfio::Wave wave;
        LOG("Loading input ... ");
        status_t status = wave.Read(inputFile, input);

        if (NO_ERROR != status)
        {
            printf("An error occured during reading %s: error=%d\n", inputFile, status);
            return -ERROR_CANNOT_OPEN_FILE;
        }
        LOG("Done\n");

        fs = wave.GetSamplingRate();
    }

    LOG("[Input]\n");
    LOG("    SamplingRate: %d\n", fs);
    LOG("    Length      : %d\n", input.GetColumnLength());
    LOG("    Channels    : %d\n", input.GetRowLength());
    LOG("\n");

    // System
    {
        mfio::Wave wave;
        LOG("Loading audio system ... ");
        status_t status = wave.Read(systemFile, system);

        if (NO_ERROR != status)
        {
            printf("An error occured during reading %s: error=%d\n", systemFile, status);
            return -ERROR_CANNOT_OPEN_FILE;
        }
        LOG("Done\n");
        if (fs != wave.GetSamplingRate())
        {
            ERROR_LOG("Not matched the sampling rates: %d (input) <=> %d (system)\n", fs, wave.GetSamplingRate());
            return -ERROR_ILLEGAL;
        }
    }
    if (system.GetRowLength() != 1
        && system.GetRowLength() != input.GetRowLength())
    {
        ERROR_LOG("Illegal channel counts: %d (system)\n", system.GetRowLength());
        ERROR_LOG("The number of system's should be 1 or the same as input's (%d) \n", input.GetRowLength());
        return -ERROR_ILLEGAL;
    }

    LOG("[System]\n");
    LOG("    Length      : %d\n", system.GetColumnLength());
    LOG("    Channels    : %d\n", system.GetRowLength());
    LOG("\n");

    if ( input.GetColumnLength() < system.GetColumnLength() )
    {
        LOG("\Error: input (%d) is shorter than system (%d).\n", input.GetColumnLength(), system.GetColumnLength());
        LOG("Exiting ... \n");
        return 0;
    }

    {
        LOG("\n");
        status_t status;
        LOG("Convluting ... ");
        mcon::Matrix<double> output;
        status = Convolution(output, input, system);
        LOG("Done\n");
        if (NO_ERROR != status)
        {
            ERROR_LOG("An error occured in Convolution: error=%d\n", status);
            return status;
        }
        LOG("Levelaring ... \n");
        double max = output[0].GetMaximumAbsolute();
        for ( int ch = 1; ch < output.GetRowLength(); ++ch )
        {
            const double _max = output[ch].GetMaximumAbsolute();
            if ( max < _max )
            {
                max = _max;
            }
        }
        output *= (32767.0/max);
        LOG("Done\n");
        {
            mutl::NodePath _input(inputFile);
            mutl::NodePath _system(systemFile);
            fbody  = _input.GetBasename();
            fbody += std::string("_");
            fbody += _system.GetBasename();
        }
        LOG("\n");
        {
            const int ch = output.GetRowLength();
            std::string fname = fbody + std::string(".wav");
            if ( !outfile.empty() )
            {
                fname = outfile;
            }
            const std::string outpath = outdir + fname;

            LOG("Saving as %s ... ", outpath.c_str());
            mfio::Wave wave(fs, ch, 16);
            wave.Write(outpath, output);
            LOG("Done\n");
        }
    }
    LOG("Finished.\n");
    return NO_ERROR;
}
