#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#include "status.h"
#include "types.h"
#include "debug.h"
#include "Matrix.h"
#include "FileIo.h"
#include "Buffer.h"

static status_t GetIr(mcon::Vector<int16_t>& ir, int& fs)
{
    FileIo wave;
    mcon::Vector<int16_t> pcm;
    status_t status = wave.Read("Trig Room.wav", pcm);

    if ( NO_ERROR != status )
    {
        return status;
    }

    FileIo::MetaData metaData = wave.GetMetaData();

    printf("SamplingRate: %d\n", metaData.samplingRate);
    printf("Channels: %d\n", metaData.numChannels);
    printf("Length=%d\n", pcm.GetLength() / metaData.numChannels);

    fs = metaData.samplingRate;
    ir.Resize(pcm.GetLength() / metaData.numChannels);

    int ch = metaData.numChannels;
    for (int i = 0; i < pcm.GetLength(); ++i)
    {
        ir[i] = pcm[2*i];
    }
    return NO_ERROR;
}

static status_t GetAudioIn(mcon::Vector<int16_t>& audioIn)
{
    FileIo wave;
    int status = wave.Read("sweep_440-3520.wav", audioIn);

    if ( NO_ERROR != status )
    {
        return status;
    }

    FileIo::MetaData metaData = wave.GetMetaData();

    printf("fs =%d\n", metaData.samplingRate);
    printf("ch =%d\n", metaData.numChannels);
    printf("bit=%d\n", metaData.bitDepth);

    return status;
}

static status_t MakeTestWave(void)
{
    mcon::Vector<int16_t> impluse;
    mcon::Vector<int16_t> audioIn;
    int fs = 0;

    int status = GetIr(impluse, fs);
    if ( NO_ERROR != status )
    {
        return status;
    }
    status = GetAudioIn(audioIn);
    if ( NO_ERROR != status )
    {
        return status;
    }

    int tapps_log2 = static_cast<int>( log2(impluse.GetLength()) + 0.5 );

    mcon::Vector<int16_t> audioOut(audioIn.GetLength());

    int len = impluse.GetLength();
    printf("Convolution\n");
    for (int dgain = 10, exit = 0; ; --dgain)
    {
        exit = 1;
        for (int i = 0; i < audioIn.GetLength(); ++i)
        {
            double sum = 0;
            for (int k = 0; k < len; ++k)
            {
                sum += (audioIn[i + k - len] * impluse[len - k]);
            }
            sum /= (1L << (15+tapps_log2-dgain));
            if (sum < -32768 || 32767 < sum)
            {
                printf("Failed at gain=%d\n", dgain);
                exit = 0;
                break;
            }
            if ( (i % 4000) == 0 )
            {
                printf("%4.1f [%%] out[%d]=%f\n", i*100.0/audioIn.GetLength(),i, sum);
            }
            audioOut[i] = static_cast<int16_t>(sum);
        }
        if (exit == 1)
        {
            break;
        }
    }

    FileIo wave(fs, 1, 16);

    status = wave.Write("sweep_440-3520_conv.wav", audioOut);

    if ( NO_ERROR != status )
    {
        printf("Failed in writing...\n");
    }
    return status;
}


static void test_RLS(void)
{
    int fs;
    mcon::Vector<int16_t> reference;
    mcon::Vector<int16_t> audioIn;
    mcon::Vector<int16_t> iIr;
    status_t status = GetIr(iIr, fs);
    if (NO_ERROR != status)
    {
        printf("An error occured: err=%d\n", status);
        return ;
    }

    static const int n = iIr.GetLength();

    mcon::Vector<double> dIr(iIr);
    mcon::Matrix<double> mIr(1, n);
    mIr[0] = dIr;

    static const int M = 256; // a design parameter.
    double c = 0.5; // an appropriately small number
    mcon::Matrix<double> Phi(M, M);
    mcon::Matrix<double> P(M, M);
    mcon::Matrix<double> h(M, 1);
    mcon::Matrix<double> u(mIr.Transpose());
    mcon::Vector<double> d(n);

    // Impluse is a desired one.
    memset(d, 0, d.GetLength() * sizeof(double));
    d[0] = 1;
    d[1] = 1;
    printf("Dimension of u: %d, %d\n", u.GetNumOfArray(), u.GetNumOfData());

    return ;
    Phi *= c;
    P /= c;
    h[0] = 0;

    for (int i = 1; i < n; ++i)
    {
        mcon::Matrix<double> k(P.Multiply(u)); // numerator
        double denom = 1.0;
        mcon::Matrix<double> denominator = u.Transpose().Multiply(P).Multiply(u);
        ASSERT(denominator.GetNumOfArray() == 1 && denominator.GetNumOfData() == 1);
        denom = denominator[0][0] + 1;
        k /= denom;

        mcon::Matrix<double> m = u.Transpose().Multiply(h);
        ASSERT(m.GetNumOfArray() == 1 && m.GetNumOfData() == 1);
        double eta = d[i] - m[0][0];

        h = h + k * eta;

        P = P - k.Multiply(u.Transpose()).Multiply(P);
    }
    h.DumpMatrix(h, "%f");
}

static void test_IR(void)
{
    FileIo wave;
    mcon::Vector<int16_t> pcm;

    wave.Read("Trig Room.wav", pcm);
    FileIo::MetaData metaData = wave.GetMetaData();

    int length = 0;
    if (1 == metaData.numChannels)
    {
    }
    else if (2 == metaData.numChannels)
    {
    }
    printf("Length=%d\n", pcm.GetLength() / metaData.numChannels);
}

int main(void)
{
    MakeTestWave();
    //test_RLS();

    return 0;
}
