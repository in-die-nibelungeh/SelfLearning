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
    printf("Usage: %s COMMAND WAVEFILES ...\n", "mudem");
    printf("\n");
    printf("COMMAND should be -s, -c, or -h.\n");
    printf("  -h: show help.\n");
    printf("  -s: split multi-channel .wav to some .wav files.\n");
    printf("  -c: combine some .wav files into one .wav.\n");
}

int main(int argc, const char* argv[])
{
    // -s(plit)
    // -c(ombine)

    if ( argc < 2 )
    {
        usage();
        return 0;
    }
    const std::string command(argv[1]);

    if ( command == std::string("-h") )
    {
        usage();
    }
    else if ( command == std::string("-s") )
    {
    }
    else if ( command == std::string("-c") )
    {
        mcon::Matrix<double> wavRoot;
        std::string filename("");
        int fs;
        mfio::Wave::WaveFormat format;
        int bits;
        const int offset = 2; // argv[0] is program and argv[1] is command.
        for (int k = 0; k < argc - offset; ++k )
        {
            std::string name(argv[k+2]);
            mfio::Wave wavFile;
            mcon::Matrix<double> wav;
            status_t status = wavFile.Read(name, wav);
            if ( NO_ERROR != status )
            {
                ERROR_LOG("An error occured during reading %s: error=%d\n",
                    name.c_str(), status);
                ERROR_LOG("Skippe %s\n", name.c_str());
                continue;
            }
            if ( !wavRoot.IsNull() )
            {
                if ( fs != wavFile.GetSamplingRate() )
                {
                    ERROR_LOG("The fs doesn't match one of the root: %d (wav) <==> %d (root)\n",
                        wavFile.GetSamplingRate(), fs);
                    ERROR_LOG("Skippe %s\n", name.c_str());
                    continue;
                }

                if ( wav.GetColumnLength() != wavRoot.GetColumnLength() )
                {
                    ERROR_LOG("The length doesn't match one of the root: %d (wav) <==> %d (root)\n",
                        wav.GetColumnLength(), wavRoot.GetColumnLength());
                    ERROR_LOG("Skippe %s\n", name.c_str());
                    continue;
                }
                filename.append( std::string("-") + name);
                filename.erase( filename.length()-4, 4 ); // ägí£éqÇçÌèú

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
                filename.append(name);
                filename.erase( filename.length()-4, 4 ); // ägí£éqÇçÌèú
            }
        }
        // Write
        {
            const std::string ewav(".wav");
            LOG("fs=%d\n", fs);
            LOG("bits=%d\n", bits);
            LOG("format=%d\n", format);
            LOG("ch=%d\n", wavRoot.GetRowLength());
            LOG("fn=%s\n", filename.c_str());
            mfio::Wave wav(fs, wavRoot.GetRowLength(), bits, format);
            status_t status = wav.Write( filename + ewav, wavRoot );
            if (status != NO_ERROR)
            {
                ERROR_LOG("An error occured during writing %s: error=%d\n", filename.c_str(), status);
            }
        }
    }
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
            std::string fname = fbody + std::string(".wav");
            LOG("Saving as %s ... ", fname.c_str());
            mfio::Wave wave(fs, 1, 16);
            wave.Write(fname, output);
            LOG("Done\n");
        }
    }
    LOG("Finished.\n");
    return NO_ERROR;
}
