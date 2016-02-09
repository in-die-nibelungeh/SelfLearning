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
#include <stdio.h>

#include "status.h"
#include "types.h"
#include "debug.h"

#include "mcon.h"
#include "mfio.h"
#include "mutl.h"

#if !defined(PROGRAM_NAME)
#error PROGRAM_NAME is not defined.
#endif

static void usage(void)
{
    printf("Usage: %s [OPTIONS] [COMMAND] WAVEFILES ...\n", PROGRAM_NAME);
    printf("\n");
    printf("COMMAND should be -s or -c.\n");
    printf("  -h: show help.\n");
    printf("  -s: split multi-channel .wav to some .wav files.\n");
    printf("  -c: combine some .wav files into one .wav.\n");
    printf("  -o: spefity an output filename.\n");
    printf("  -d: spefity an output directory, which should already exist.\n");
}

// Description
typedef struct mutl::ArgumentDescription Desc;

const Desc descs[] =
{
    {"h" , 0},
    {"o" , 1},
    {"d" , 1},
    {"s" , 0},
    {"c" , 0}
};

int main(int argc, const char* argv[])
{
    // -s(plit)
    // -c(ombine)
    mutl::ArgumentParser parser;
    if ( false == parser.Initialize(argc, argv, descs, sizeof(descs)/sizeof(Desc)) )
    {
        usage();
        return 0;
    }

    if ( parser.IsEnabled("h")
         || (parser.IsEnabled("c") && parser.IsEnabled("s")))
    {
        usage();
        return 0;
    }

    std::string outdir("./");
    if ( parser.IsEnabled("d") )
    {
        outdir = parser.GetOption("d");
        outdir += std::string("/");
    }

    if ( parser.IsEnabled("s") )
    {
    }
    else if ( parser.IsEnabled("c") )
    {
        mcon::Matrix<double> wavRoot;
        std::string outfile("");
        int fs = 0;
        mfio::Wave::WaveFormat format = mfio::Wave::LPCM;
        int bits = 0;
        const int inputCount = parser.GetArgumentCount();
        for (int k = 0; k < inputCount; ++k )
        {
            const mutl::NodePath node(parser.GetArgument(k));
            const std::string path = node.GetFullpath();
            const std::string basename = node.GetBasename();
            DEBUG_LOG("basename=%s\n", basename.c_str());
            mfio::Wave wavFile;
            mcon::Matrix<double> wav;
            status_t status = wavFile.Read(path, wav);
            if ( NO_ERROR != status )
            {
                ERROR_LOG("An error occured during reading %s: error=%d\n",
                    path.c_str(), status);
                ERROR_LOG("Skipping\n");
                continue;
            }
            if ( !wavRoot.IsNull() )
            {
                if ( fs != wavFile.GetSamplingRate() )
                {
                    ERROR_LOG("The fs doesn't match one of the root: %d (%s) <==> %d (root)\n",
                        wavFile.GetSamplingRate(), path.c_str(), fs);
                    ERROR_LOG("Skipping\n");
                    continue;
                }

                if ( wav.GetColumnLength() != wavRoot.GetColumnLength() )
                {
                    ERROR_LOG("The length doesn't match one of the root: %d (%s) <==> %d (root)\n",
                        static_cast<int>(wav.GetColumnLength()), path.c_str(), static_cast<int>(wavRoot.GetColumnLength()) );
                    ERROR_LOG("Skipping\n");
                    continue;
                }

                outfile.append( std::string("-") + basename);

                const int length = wavRoot.GetColumnLength();
                const int row = wavRoot.GetRowLength() + wav.GetRowLength();
                mcon::Matrix<double> old(wavRoot);
                wavRoot.Resize(row, length);
                DEBUG_LOG("row=%d, col=%d\n", row, length);
                const int boundary = old.GetRowLength();
                for ( int k = 0; k < row; ++k )
                {
                    if ( k < boundary )
                    {
                        DEBUG_LOG("root[%d] = old[%d]\n", k, k);
                        wavRoot[k] = old[k];
                    }
                    else
                    {
                        DEBUG_LOG("root[%d] = wav[%d]\n", k, k - boundary);
                        wavRoot[k] = wav[k - boundary];
                    }
                }
            }
            else
            {
                wavRoot = wav;
                fs = wavFile.GetSamplingRate();
                format = wavFile.GetWaveFormat();
                bits = wavFile.GetBitDepth();
                outfile.append(basename);
            }
        }
        // Write
        {
            const std::string ewav(".wav");
            outfile += ewav;
            if (parser.IsEnabled("o"))
            {
                outfile = parser.GetOption("o");
            }
            const std::string outpath = outdir + outfile;

            LOG("fs=%d\n", fs);
            LOG("bits=%d\n", bits);
            LOG("format=%d\n", format);
            LOG("ch=%d\n", static_cast<int>(wavRoot.GetRowLength()));
            LOG("fn=%s\n", outpath.c_str());
            mfio::Wave wav(fs, wavRoot.GetRowLength(), bits, format);
            status_t status = wav.Write( outpath, wavRoot );
            if (status != NO_ERROR)
            {
                ERROR_LOG("An error occured during writing %s: error=%d\n", outpath.c_str(), status);
            }
        }
    }
    return 0;
}

