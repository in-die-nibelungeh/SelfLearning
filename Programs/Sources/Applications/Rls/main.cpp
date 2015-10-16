#include <string>

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "status.h"
#include "types.h"
#include "debug.h"
#include "Matrix.h"
#include "FileIo.h"

#include "Fft.h"

#define POW2(x) ((x)*(x))

status_t RlsFromTwoWaveforms(const char* inputFile, const char* referenceFile, int tapps);
status_t ConvoluteTwoWaveforms(const char* inputFile, const char* systemFile);

status_t Normalize(mcon::Vector<double>& vec)
{
    mcon::Matrix<double> complex(2, vec.GetLength());
    mcon::Matrix<double> gp(2, vec.GetLength());

    LOG("Ft ... ");
    Fft::Ft(complex, vec);
    LOG("Done\n");
    LOG("Convert to gain and phsae ... ");
    Fft::ConvertToGainPhase(gp, complex);
    LOG("Done\n");

    // Normalizing
    double max = gp[0].GetMaximum();
    LOG("Divied max (%f) ...", max);
    vec /= max;
    LOG("Done\n");
    return NO_ERROR;
}

status_t Convolution(mcon::Vector<double>& audioOut, const mcon::Vector<double>& audioIn, const mcon::Vector<double>& impluse)
{
    if (audioIn.GetLength() < impluse.GetLength())
    {
        return -ERROR_ILLEGAL;
    }

    audioOut.Resize(audioIn.GetLength());

    for (int i = 0; i < audioIn.GetLength(); ++i)
    {
        audioOut[i] = 0.0;
        for (int k = 0; k < impluse.GetLength(); ++k)
        {
            audioOut[i] += audioIn[i - k] * impluse[k];
        }
    }
    return NO_ERROR;
}

status_t RlsFromTwoWaveforms(const char* inputFile, const char* referenceFile, int tapps)
{
    mcon::Vector<double> input;
    mcon::Vector<double> reference;
    mcon::Vector<double> resp;
    std::string fbody;
    int fs;

    // Input
    {
        mfio::Wave wave;
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
        fs = wave.GetSamplingRate();
    }

    LOG("[Input]\n");
    LOG("    SamplingRate: %d\n", fs);
    LOG("    Length      : %d\n", input.GetLength());
    LOG("\n");

    // Reference
    {
        mfio::Wave wave;
        mcon::Matrix<double> referenceMatrix;
        LOG("Loading audio reference ... ");
        status_t status = wave.Read(referenceFile, referenceMatrix);

        if (NO_ERROR != status)
        {
            printf("An error occured during reading %s: error=%d\n", referenceFile, status);
            return -ERROR_CANNOT_OPEN_FILE;
        }
        LOG("Done\n");
        if ( wave.GetNumChannels() > 1 )
        {
            LOG("%d-channel waveform was input.\n", wave.GetNumChannels());
            LOG("Only left channel is used for estimation.\n");
        }
        if (fs != wave.GetSamplingRate())
        {
            ERROR_LOG("Not matched the sampling rates: %d (input) <=> %d (reference)\n", fs, wave.GetSamplingRate());
            return -ERROR_ILLEGAL;
        }
        reference = referenceMatrix[0];
    }
    LOG("[Reference]\n");
    LOG("    Length      : %d\n", reference.GetLength());
    LOG("\n");

    if ( input.GetLength() > reference.GetLength() )
    {
        LOG("\nWarning: input is longer than reference.\n");
        LOG("This may cause a wrong estimation.\n");
    }
    {
        LOG("\n");
        LOG("Initializing variables ... ");
        const int n = input.GetLength();
        const int M = tapps;
        double c = 0.0000001; // an appropriately small number
        mcon::Matrix<double> P = mcon::Matrix<double>::E(M);
        mcon::Vector<double> _h(M);
        _h = 0;
        mcon::Matrix<double> h(_h.Transpose());
        mcon::Vector<double>& d = input;
        mcon::Vector<double> uv(M);
        mcon::Vector<double> e(n);
        mcon::Vector<double> eta(n);
        mcon::Vector<double> J(n);
        mcon::Vector<double> K(n);
        mcon::Vector<double> U(n);

        //h = 0;
        P /= c;
        uv = 0;

        LOG("Done\n");
        LOG("Now starting RLS with %d samples.\n", n);
        for (int i = 0; i < n; ++i)
        {
            uv.Unshift(reference[i]);
            U[i] = uv.GetNorm();
            const mcon::Matrix<double>& u = uv.Transpose();
            mcon::Matrix<double> k(P.Multiply(u)); // numerator
            double denom = 1.0;
            const mcon::Matrix<double>& denominator = u.Transpose().Multiply(P).Multiply(u);
            ASSERT(denominator.GetRowLength() == 1 && denominator.GetColumnLength() == 1);
            denom = denominator[0][0] + 1;
            k /= denom;
            K[i] = k.Transpose()[0].GetNorm();
            const mcon::Matrix<double>& m = u.Transpose().Multiply(h);
            ASSERT(m.GetRowLength() == 1 && m.GetColumnLength() == 1);
            eta[i] = d[i] - m[0][0];
            //printf("i=%d,eta=%g,d=%g,m=%g\n", i, eta, d[i], m[0][0]);
            h += k * eta[i];

            e[i] = d[i] - (u.Transpose().Multiply(h))[0][0];
            J[i] = J[i-1] + e[i] * eta[i];
            P -= k.Multiply(u.Transpose()).Multiply(P);
            if ( (i % 10) == 0 )
            {
                LOG("%4.1f [%%]: %d/%d\r", i*100.0/n, i, n);
            }
        }
        resp = h.Transpose()[0];
        {
            char _fbody[128];
            sprintf(_fbody, "Af_%dtapps_", tapps);
            fbody = std::string(_fbody);
            fbody += std::string(inputFile);
            fbody.erase( fbody.length()-4, 4);
            fbody += std::string("_");
            fbody += std::string(referenceFile);
            fbody.erase( fbody.length()-4, 4);
        }
        LOG("\n");
        {
            const int length = resp.GetLength();
            mcon::Matrix<double> complex(2, length);
            mcon::Matrix<double> gp(2, length);

            Fft::Ft(complex, resp);
            Fft::ConvertToPolarCoords(gp, complex);
            const std::string ecsv(".csv");
            // Gain
            {
                mcon::Matrix<double> gain(2, length);
                const double df = 1.0  * fs / M;
                const double max = gp[0].GetMaximum();
                for (int i = 0; i < length; ++i)
                {
                    gain[0][i] = i*df;
                    gain[1][i] = -20 * log10(gp[0][i]/max);
                }
                std::string fname = fbody + std::string("_gain") + ecsv;
                mfio::Csv csv(fname);
                csv.Write(",freq,Gain[dB]\n");
                csv.Write(gain);
                csv.Close();
                LOG("Output: %s\n", fname.c_str());
            }
            // Phase
            {
                mcon::Matrix<double> phase(2, length);
                const double df = 1.0  * fs / M;
                for (int i = 0; i < length; ++i)
                {
                    phase[0][i] = i*df;
                    phase[1][i] = gp[1][i]*180/M_PI;
                }
                std::string fname = fbody + std::string("_phase") + ecsv;
                mfio::Csv csv(fname);
                csv.Write(",freq,Phase[deg]\n");
                csv.Write(phase);
                csv.Close();
                LOG("Output: %s\n", fname.c_str());
            }
            // TimeSeries Data of the estimated response.
            {
                std::string fname = fbody + std::string("_ts") + ecsv;
                mfio::Csv::Write(fname, resp);
                LOG("Output: %s\n", fname.c_str());
            }

            // Rls logs
            {
                mcon::Matrix<double> logs(5, n);
                logs[0] = e;
                logs[1] = eta;
                logs[2] = J;
                logs[3] = K;
                logs[4] = U;
                std::string fname = fbody + std::string("_logs") + ecsv;
                mfio::Csv csv(fname);
                csv.Write("i,e,eta,J,|k|,|u|\n");
                csv.Write(logs);
                csv.Close();
                LOG("Output: %s\n", fname.c_str());
            }
        }
        LOG("\n");
        {
            LOG("Verifying ... ");
            const int n = input.GetLength();
            mcon::Vector<double> origin(n);
            Convolution(origin, input, resp);
            LOG("Done\n");
            origin *= 32767.0/origin.GetMaximumAbsolute();
            const std::string ewav(".wav");
            std::string fname = fbody + ewav;
            LOG("Saving as %s\n", fname.c_str());
            mfio::Wave wave;
            wave.SetNumChannels(1);
            wave.SetBitDepth(16);
            wave.SetSamplingRate(fs);
            wave.SetWaveFormat(mfio::Wave::LPCM);
            status_t status = wave.Write(fname, origin);
            if ( NO_ERROR != status )
            {
                ERROR_LOG("Failed in writing %s: error=%d\n", fname.c_str(), status);
            }
            LOG("Done.\n");
        }
    }
    LOG("Finished.\n");
    return NO_ERROR;
}

static void usage(void)
{
    printf("Usage: %s [OPTIONS] INPUT REFERENCE [TAPPS]\n", "rls");
    printf("\n");
    printf("INPUT and REFERENCE must be .wav files.\n");
    printf("TAPPS must be an interger value larger than 0.\n");
}

int main(int argc, char* argv[])
{
    std::string reference;
    std::string input;
    int tapps = 256;

    if ( argc < 3 )
    {
        usage();
        return 0;
    }
    input = std::string(argv[1]);
    reference = std::string(argv[2]);

    if ( argc > 3 )
    {
        tapps = atoi(argv[3]);
    }

    if ( tapps < 1 )
    {
        usage();
        return 0;
    }

    LOG("Input: %s\n", input.c_str());
    LOG("Reference: %s\n", reference.c_str());
    LOG("Tapps: %d\n", tapps);
    LOG("\n");
    //ConvoluteTwoWaveforms(input.c_str(), reference.c_str());
    RlsFromTwoWaveforms(input.c_str(), reference.c_str(), tapps);

    return 0;
}

status_t ConvoluteTwoWaveforms(const char* inputFile, const char* systemFile)
{
    mcon::Vector<double> input;
    mcon::Vector<double> system;
    std::string fbody;
    int fs;

    // Input
    {
        mfio::Wave wave;
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
        fs = wave.GetSamplingRate();
    }

    LOG("[Input]\n");
    LOG("    SamplingRate: %d\n", fs);
    LOG("    Length      : %d\n", input.GetLength());
    LOG("\n");

    // System
    {
        mfio::Wave wave;
        mcon::Matrix<double> systemMatrix;
        LOG("Loading audio system ... ");
        status_t status = wave.Read(systemFile, systemMatrix);

        if (NO_ERROR != status)
        {
            printf("An error occured during reading %s: error=%d\n", systemFile, status);
            return -ERROR_CANNOT_OPEN_FILE;
        }
        LOG("Done\n");
        if ( wave.GetNumChannels() > 1 )
        {
            LOG("%d-channel waveform was input.\n", wave.GetNumChannels());
            LOG("Only left channel is used for estimation.\n");
        }
        if ( wave.GetWaveFormat() != mfio::Wave::LPCM )
        {
            mcon::Matrix<double> lpcm(systemMatrix);
            double max = lpcm[0].GetMaximumAbsolute();
            for ( int i = 1; i < lpcm.GetRowLength(); ++i )
            {
                double _max = lpcm[i].GetMaximumAbsolute();
                if ( _max > max )
                {
                    max = _max;
                }
            }
            lpcm *= 32767.0/max;
            std::string fname = std::string(systemFile);
            fname.erase( fname.length()-4, 4);
            fname += std::string("_LPCM.wav");
            wave.SetWaveFormat(mfio::Wave::LPCM);
            wave.SetBitDepth(16);
            wave.Write(fname, lpcm);
        }
        if (fs != wave.GetSamplingRate())
        {
            ERROR_LOG("Not matched the sampling rates: %d (input) <=> %d (system)\n", fs, wave.GetSamplingRate());
            return -ERROR_ILLEGAL;
        }
        system = systemMatrix[0];
    }
    LOG("[System]\n");
    LOG("    Length      : %d\n", system.GetLength());
    LOG("\n");

    if ( input.GetLength() < system.GetLength() )
    {
        LOG("\Error: input (%d) is shorter than system (%d).\n", input.GetLength(), system.GetLength());
        LOG("Exiting ... \n");
        return 0;
    }
    {
        LOG("\n");
        LOG("Convluting ... ");
        mcon::Vector<double> output;
        Convolution(output, input, system);
        LOG("Done\n");
        const double max = output.GetMaximumAbsolute();
        output *= (32767.0/max);
        LOG("max=%f\n", max);
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
