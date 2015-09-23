#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#include "status.h"
#include "types.h"
#include "debug.h"
#include "Matrix.h"
#include "FileIo.h"

#define POW2(x) ((x)*(x))

static status_t GetIr(const char* name, mcon::Vector<double>& ir, int& fs)
{
    FileIo wave;
    mcon::Vector<int16_t> pcm;

    status_t status = wave.Read(name, pcm);
    if ( NO_ERROR != status )
    {
        return status;
    }

    FileIo::MetaData metaData = wave.GetMetaData();

    printf("SamplingRate: %d\n", metaData.samplingRate);
    printf("Channels: %d\n", metaData.numChannels);
    printf("Length=%d\n", pcm.GetLength() / metaData.numChannels);

    fs = wave.GetSamplingRate();
    ir.Resize(pcm.GetLength() / metaData.numChannels);

    // Only left channel is picked up.
    int ch = metaData.numChannels;
    double energy = 0.0;
    for (int i = 0; i < pcm.GetLength(); ++i)
    {
        ir[i] = static_cast<double>(pcm[ch*i]);
        energy += POW2(ir[i]);
    }
    // ir.GetLength() で割ると一要素あたりのエネルギになる。
    // 全区間にわたるエネルギで正規化しておきたい (畳み込みの結果が範囲内に収まるように)。
    //
    energy = sqrt(energy);// / ir.GetLength();
    printf("Energy: %f (length=%d)\n", energy, ir.GetLength());
    // Normalizing
    for (int i = 0; i < pcm.GetLength(); ++i)
    {
        ir[i] /= energy;
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

static status_t MakeTestIr(void)
{
    static const int ir_length = 64;

    mcon::Vector<double> impluse;
    int fs = 0;

    int status = GetIr("Trig Room.wav", impluse, fs);

    if ( NO_ERROR != status )
    {
        return status;
    }
    // インパルス長が 64 になるように調整
    const int x = impluse.GetLength() / ir_length;

    mcon::Vector<double> ir_arranged(ir_length);

    double energy = 0.0;
    for (int i = 0; i < impluse.GetLength(); ++i)
    {
        ir_arranged[i] = impluse[i*x];
        energy += POW2(ir_arranged[i]);
    }
    energy = sqrt(energy);

    printf("Energy: %g\n", energy);
    // 区間のエネルギで割る
    ir_arranged /= energy;

    // int16_t に合わせるため
    ir_arranged *= 32767;

    mcon::Vector<int16_t> ir_arranged_int(ir_arranged);
    FileIo ir(fs, 1, 16);

    const char* name = "ir_test.wav";
    status = ir.Write(name, ir_arranged_int);

    if (status != NO_ERROR)
    {
        printf("Failed in writing %s...\n", name);
    }
}

static status_t MakeTestWave(void)
{
    mcon::Vector<double> impluse;
    mcon::Vector<int16_t> audioIn;
    int fs = 0;

    int status = GetIr("ir_test.wav", impluse, fs);
    // return NO_ERROR;
    // TBD
    // 範囲を超えないように調整
    // →どうするべき？
    impluse *= 0.3;
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
    for (int dgain = 10, exit = 0; dgain >= 0 ; --dgain)
    {
        exit = 1;
        for (int i = 0; i < audioIn.GetLength(); ++i)
        {
            double sum = 0;
            for (int k = 0; k < len; ++k)
            {
                sum += (audioIn[i + k - len] * impluse[len - k]);
            }
            //sum /= (1L << (15+tapps_log2-dgain));
            if (sum < -32768 || 32767 < sum)
            {
                printf("Failed at gain=%d (sum=%f)\n", dgain, sum);
                exit = 0;
                break;
            }
            if ( (i % 4000) == 0 )
            {
                printf("%4.1f [%%] out[%d]=%f\r", i*100.0/audioIn.GetLength(),i, sum);
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
    {
        const char* file = "sweep_440-3520_conv.wav";
        FileIo waveAudioIn;
        status_t status = waveAudioIn.Read(file, audioIn);
        if (NO_ERROR != status)
        {
            printf("Failed in reading %s: error=%d\n", file, status);
            return ;
        }
        FileIo::MetaData metaData = waveAudioIn.GetMetaData();
        printf("fs=%d\n", metaData.samplingRate);
        fs = metaData.samplingRate;
    }

    {
        const char* file = "sweep_440-3520.wav";
        FileIo waveReference;
        status_t status = waveReference.Read(file, reference);
        if (NO_ERROR != status)
        {
            printf("Failed in reading %s: error=%d\n", file, status);
            return ;
        }
        FileIo::MetaData metaData = waveReference.GetMetaData();
        printf("fs=%d\n", metaData.samplingRate);

        if ( fs != metaData.samplingRate )
        {
            printf("fs doesn't match: %d != %d\n", fs, metaData.samplingRate);
        }
    }
    static const int n = audioIn.GetLength();
    static const int M = 64; // a design parameter.
    double c = 0.5; // an appropriately small number
    mcon::Matrix<double> P = mcon::Matrix<double>::E(M);
    mcon::Matrix<double> h(M, 1);
    //mcon::Matrix<double> u(audioIn.Transpose());
    mcon::Vector<double> d(reference);
    mcon::Vector<double> uv(M);

    P /= c;
    h[0] = 0;
    uv = 0;

    printf("Start (n=%d)\n", n);
    for (int i = 0; i < n; ++i)
    {
        uv.Fifo(audioIn[i]);
        const mcon::Matrix<double>& u = uv.Transpose();
        // Calling template <typename U> operator Vector<U>() const
        //mcon::Vector<double> d = reference(i, M);
        mcon::Matrix<double> k(P.Multiply(u)); // numerator
        double denom = 1.0;
        const mcon::Matrix<double>& denominator = u.Transpose().Multiply(P).Multiply(u);
        ASSERT(denominator.GetRowLength() == 1 && denominator.GetColumnLength() == 1);
        denom = denominator[0][0] + 1;
        k /= denom;

        const mcon::Matrix<double>& m = u.Transpose().Multiply(h);
        ASSERT(m.GetRowLength() == 1 && m.GetColumnLength() == 1);
        double eta = d[i] - m[0][0];

        h += k * eta;

        P -= k.Multiply(u.Transpose()).Multiply(P);
        if ( (i % 10) == 0 )
        {
            printf("%4.1f [%%]: %d/%d\r", i*100.0/n, i, n);
        }
    }
    printf("\n");
    double max = 0.0;
    for (int i = 0; i < h.GetRowLength(); ++i)
    {
        if (max < fabs(h[i][0]))
        {
            max = fabs(h[i][0]);
        }
    }
    mcon::Vector<int16_t> coefs(h.Transpose()[0] * (32767.0/max));
    FileIo filter(fs, 1, 16);
    filter.Write("rls_taps_coefficients.wav", coefs);
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
    //MakeTestIr();
    //MakeTestWave();
    test_RLS();

    return 0;
}
