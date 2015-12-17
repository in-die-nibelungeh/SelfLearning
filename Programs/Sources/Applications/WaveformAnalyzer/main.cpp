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

status_t Analyze(const char* inputFile, const std::string& = "", const std::string& outdir = "");

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
    {"o" , 1}
};

int main(int argc, const char* argv[])
{
    mutl::ArgumentParser parser;
    if( false == parser.Initialize(argc, argv, descs, sizeof(descs)/sizeof(Desc))
        || parser.IsEnabled("h")
        || parser.GetArgumentCount() < 1 )
    {
        usage();
        return 0;
    }

    const std::string input = parser.GetArgument(0);
    std::string outfile;

    LOG("Input : %s\n", input.c_str());
    if (parser.IsEnabled("o"))
    {
        outfile = parser.GetOption("o");
        LOG("Output: %s\n", outfile.c_str());
    }
    std::string outdir("./");
    if ( parser.IsEnabled("d") )
    {
        outdir = parser.GetOption("d");
        outdir += std::string("/");
    }

    Analyze(input.c_str(), outfile, outdir);

    return 0;
}

status_t Analyze(const char* inputFile, const std::string& outfile, const std::string& outdir)
{
    mcon::Vector<double> input;
    mfio::Wave wave;
    mutl::NodePath inputPath(inputFile);
    std::string outbase = outdir;

    {
        std::string fbody = inputPath.GetBasename();
        if ( !outfile.empty() )
        {
            fbody = outfile;
        }
        outbase += fbody;
    }

    // Input
    {
        mcon::Matrix<double> inputMatrix;
        LOG("Loading input ... ");
        status_t status = wave.Read(inputFile, inputMatrix);

        if (NO_ERROR != status)
        {
            printf("An error occured during reading %s: error=%d\n", inputFile, status);
            return -ERROR_CANNOT_OPEN_FILE;
        }
        if ( wave.GetNumChannels() > 1 )
        {
            LOG("%d-channel waveform was input.\n", wave.GetNumChannels());
            LOG("Only left channel is used for estimation.\n");
        }
        LOG("Done\n");

        input = inputMatrix[0];
    }

    LOG("[Input]\n");
    LOG("    SamplingRate: %d\n", wave.GetSamplingRate());
    LOG("    Length      : %d\n", input.GetLength());

    // Energy
    {
        const size_t N = input.GetLength();
        mcon::Matrix<double> energyRatio(2, N);
        const double energy = sqrt(input.GetDotProduct(input));
        LOG("    Energy      : %g\n", energy);
        double sum = 0;
        for ( uint k = 0; k < N; ++k )
        {
            sum += input[k] * input[k];
            energyRatio[0][k] = sqrt(sum);
            energyRatio[1][k] = sqrt(sum) / energy;
        }
        const std::string ecsv("_energy.csv");

        mfio::Csv csv(outbase + ecsv);
        csv.Write(",Enegy,Energy ratio\n");
        csv.Write(energyRatio);
        csv.Close();
    }

    {
        LOG("\n");
        mcon::Matrix<double> polar;

        {
            mcon::Matrix<double> complex;
            masp::ft::Ft(complex, input);
            masp::ft::ConvertToPolarCoords(polar, complex);
        }
        {
            const int N = input.GetLength();
            mcon::Matrix<double> matrix(4, N);

            for ( int i = 0; i < N; ++i )
            {
                matrix[0][i] = 1.0 * i / N * wave.GetSamplingRate();
            }
            matrix[1] = polar[0];
            matrix[2] = polar[1];
            matrix[3] = input;

            const std::string ecsv("_spectrum.csv");

            mfio::Csv csv(outbase + ecsv);
            csv.Write("Id,Frequency,Amplitude,Argument,Impulse\n");
            csv.Write(matrix);
            csv.Close();
        }
        LOG("Done\n");
    }
    LOG("Finished.\n");
    return NO_ERROR;
}
