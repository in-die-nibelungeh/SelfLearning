#include <string>

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "status.h"
#include "types.h"
#include "debug.h"
#include "Matrix.h"
#include "FileIo.h"
#include "masp.h"

status_t Analyze(const char* inputFile);

static void usage(void)
{
    printf("Usage: %s INPUT \n", "ft");
    printf("\n");
    printf("INPUT must be .wav files.\n");
}

int main(int argc, char* argv[])
{
    std::string input;
    if ( argc < 2 )
    {
        usage();
        return 0;
    }
    input = std::string(argv[1]);

    LOG("Input: %s\n", input.c_str());
    Analyze(input.c_str());

    return 0;
}

status_t Analyze(const char* inputFile)
{
    mcon::Vector<double> input;
    mfio::Wave wave;
    std::string fbody;

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
    LOG("\n");

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
                matrix[0][i] = 1.0 * i / N;
            }
            matrix[1] = polar[0];
            matrix[2] = polar[1];
            matrix[3] = input;

            fbody  = std::string(inputFile);
            fbody.erase( fbody.length()-4, 4);
            const std::string ecsv(".csv");

            mfio::Csv csv(fbody + ecsv);
            csv.Write(",,Gain[dB],Phase[rad],Pulse\n");
            csv.Write(matrix);
            csv.Close();
        }
        LOG("Done\n");
    }
    LOG("Finished.\n");
    return NO_ERROR;
}
