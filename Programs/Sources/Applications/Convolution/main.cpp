#include <string>

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "status.h"
#include "types.h"
#include "debug.h"

#include "mcon.h"
#include "mfio.h"

status_t ConvoluteTwoWaveforms(const char* inputFile, const char* systemFile);

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
}

int main(int argc, char* argv[])
{
    std::string reference;
    std::string input;
    if ( argc < 3 )
    {
        usage();
        return 0;
    }
    input = std::string(argv[1]);
    reference = std::string(argv[2]);

    // Display messages real-time
    setvbuf(stdout, NULL, _IONBF, 0);

    LOG("Input: %s\n", input.c_str());
    LOG("Impluse: %s\n", reference.c_str());
    LOG("\n");
    ConvoluteTwoWaveforms(input.c_str(), reference.c_str());

    return 0;
}

status_t ConvoluteTwoWaveforms(const char* inputFile, const char* systemFile)
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
            fbody  = std::string(inputFile);
            fbody.erase( fbody.length()-4, 4);
            fbody += std::string("_");
            fbody += std::string(systemFile);
            fbody.erase( fbody.length()-4, 4);
        }
        LOG("\n");
        {
            const int ch = output.GetRowLength();
            std::string fname = fbody + std::string(".wav");
            LOG("Saving as %s ... ", fname.c_str());
            mfio::Wave wave(fs, ch, 16);
            wave.Write(fname, output);
            LOG("Done\n");
        }
    }
    LOG("Finished.\n");
    return NO_ERROR;
}
