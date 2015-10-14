#include <string>

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
#include "Fft.h"

#define POW2(x) ((x)*(x))

//static const char* fname_body = "sweep_440-8800";
static const char* fname_body = "sweep_440-3520_1s";

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

static status_t GetIr(const char* name, mcon::Vector<double>& ir, int& fs)
{
    mfio::Wave wave;
    mcon::Vector<double> pcm;

    status_t status = wave.Read(name, pcm);
    if ( NO_ERROR != status )
    {
        return status;
    }

    mfio::Wave::MetaData metaData = wave.GetMetaData();

    printf("SamplingRate: %d\n", metaData.samplingRate);
    printf("Channels: %d\n", metaData.numChannels);
    printf("Length=%d\n", pcm.GetLength() / metaData.numChannels);

    // Only left channel is picked up.
    const int ch = wave.GetNumChannels();
    const int length = pcm.GetLength();
    ir.Resize(length / ch);
    for (int i = 0; i < length / ch; ++i)
    {
        ir[i] = static_cast<double>(pcm[ch*i]);
    }
    fs = wave.GetSamplingRate();
    return NO_ERROR;
}

static status_t GetAudioIn(mcon::Vector<double>& audioIn)
{
    mfio::Wave wave;
    int status = wave.Read( (std::string(fname_body) + std::string(".wav")).c_str(), audioIn);

    if ( NO_ERROR != status )
    {
        return status;
    }

    mfio::Wave::MetaData metaData = wave.GetMetaData();

    printf("fs =%d\n", metaData.samplingRate);
    printf("ch =%d\n", metaData.numChannels);
    printf("bit=%d\n", metaData.bitDepth);

    return status;
}

static status_t MakeTestIr(const char* fname, int length)
{
    const int ir_length = length;

    mcon::Vector<double> impluse;
    int fs = 0;

    int status = GetIr(fname, impluse, fs);

    if ( NO_ERROR != status )
    {
        return status;
    }
    // インパルス長が length になるように調整
    const int x = impluse.GetLength() / ir_length;

    mcon::Vector<double> ir_arranged(ir_length);

    for (int i = 0; i < impluse.GetLength(); ++i)
    {
        ir_arranged[i] = impluse[i*x];
    }
    // 正規化
    Normalize(ir_arranged);

    // int16_t に合わせるため
    ir_arranged *= 32767;

    mcon::Vector<int16_t> ir_arranged_int(ir_arranged);
    mfio::Wave ir(fs, 1, 16);

    char name[256];
    sprintf(name, "ir_test_%d.wav", length);
    status = ir.Write(name, ir_arranged_int);

    if (status != NO_ERROR)
    {
        printf("Failed in writing %s...\n", name);
    }
    return NO_ERROR;

    mcon::Vector<double> hoge(ir_length);
    for (int i = 0; i < ir_length; ++i)
    {
        hoge[i] = ir_arranged[i];
    }
    mcon::Matrix<double> comp(2, ir_length);
    mcon::Matrix<double> gp(2, ir_length/2);
    Fft::Ft(comp, hoge);
    Fft::ConvertToGainPhase(gp, comp);
    FILE*fp = fopen("ir_test_ft.csv", "w");
    double df = 1.0  * fs / ir_length;
    for (int i = 1; i < gp.GetColumnLength(); ++i)
    {
        fprintf(fp, "%f,%f,%f\n",
            i*df, gp[0][i], gp[1][i]*180/M_PI);
    }
    fclose(fp);
}

static status_t AnalyzeWaveform(mcon::Vector<double>& data)
{
    double max = fabs(data[0]);
    double min = fabs(data[0]);
    double ave;
    double var;
    double sd;
    double E = 0.0;

    {
        double sum = fabs(data[0]);

        for (int i = 1; i < data.GetLength(); ++i)
        {
            if (max < fabs(data[i]))
            {
                max = fabs(data[i]);
            }
            if (min > fabs(data[i]))
            {
                min = fabs(data[i]);
            }
            sum += fabs(data[i]);
            E += POW2(data[i]);
        }
        ave = sum / data.GetLength();
    }

    // 意味なし
    if (0 == max || min == max)
    {
        return NO_ERROR;
    }

    ASSERT(0 != min);

    printf("Maximum: %g\n", max);
    printf("Minimum: %g\n", min);
    printf("Average: %g\n", ave);

    {
        double sum = 0.0;

        for (int i = 1; i < data.GetLength(); ++i)
        {
            sum += POW2(data[i] - ave);
        }
        // 分散、標準偏差
        var = sum / data.GetLength();
        sd  = sqrt(var);
    }

    // とにかく 64 段階分割
    const int seps = 64;
    mcon::Vector<int> bars(seps);
    mcon::Vector<double> energy(seps);

    bars = 0;
    energy = 0;

    double upper = pow(2, static_cast<int>(log10(max) / log10(2)) + 1);
    double lower = upper / 2.0;
    int count = 0;
    for (int k = seps - 1; k >= 0; --k)
    {
        if (0 != k)
        {
            for (int i = 0; i < data.GetLength(); ++i)
            {
                double v = fabs(data[i]);
                if (lower <= v && v < upper)
                {
                    ++bars[k];
                    energy[k] += POW2(v);
                    ++count;
                }
            }
        }
        else
        {
            for (int i = 0; i < data.GetLength(); ++i)
            {
                double v = fabs(data[i]);
                if (v < upper)
                {
                    ++bars[k];
                    energy[k] += POW2(v);
                    ++count;
                }
            }
        }
        upper /= 2;
        lower = upper / 2;
    }
    printf("length=%d\n", data.GetLength());
    printf("count =%d\n", count);

    return NO_ERROR;
}

static status_t EnergyRatio(mcon::Vector<double>& data)
{
    const int seps = 16;
    const double threshold = 0.5;
    mcon::Vector<double> tmp(data);
    bool exit = false;
    do
    {
        const int width = static_cast<int>(tmp.GetLength() / seps + 0.5) + 1;
        mcon::Vector<double> energy(seps);
        double E = 0.0;

        energy = 0;

        for (int i = 0; i < tmp.GetLength(); ++i)
        {
            int ai = i / width;

            E += POW2(tmp[i]);
            energy[ai] += POW2(tmp[i]);
        }

        if (energy[0]/E < threshold)
        {
            double sum = 0;

            printf("Width: %d\n", width);

            for (int i = 0; i < seps; ++i)
            {
                sum += energy[i];

                printf("%d\t%g\t%g\n", i,
                    energy[i]/E*100, sum/E*100);
            }
            exit = true;
        }
        else
        {
            const double threshold_cut = 0.95;
            int cut;
            double sum = 0.0;
            for (cut = 0; cut < seps; ++cut)
            {
                sum += energy[cut];
                //printf("cut=%d\tsum=%f\n", cut, sum/E);
                if (sum/E > threshold_cut)
                {
                    break;
                }
            }
            tmp = tmp(0, (cut+1) * width);
            //printf("Length=%d\n", tmp.GetLength());
        }
    } while (exit == false);

    return NO_ERROR;
}


static status_t MakeTestWave(void)
{
    mcon::Vector<double> impluse;
    mcon::Vector<double> audioIn;
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

    mfio::Wave wave(fs, 1, 16);

    status = wave.Write( (std::string(fname_body) + std::string("_conv.wav")).c_str(), audioOut);

    if ( NO_ERROR != status )
    {
        printf("Failed in writing...\n");
    }
    return status;
}


static void test_RLS(void)
{
    int fs;
    mcon::Vector<double> reference;
    mcon::Vector<double> audioIn;
    {
        const char* file = (std::string(fname_body) + std::string("_conv.wav")).c_str();
        mfio::Wave waveAudioIn;
        status_t status = waveAudioIn.Read(file, audioIn);
        if (NO_ERROR != status)
        {
            printf("Failed in reading %s: error=%d\n", file, status);
            return ;
        }
        mfio::Wave::MetaData metaData = waveAudioIn.GetMetaData();
        printf("fs=%d\n", metaData.samplingRate);
        fs = metaData.samplingRate;
    }

    {
        const char* file = (std::string(fname_body) + std::string(".wav")).c_str();
        mfio::Wave waveReference;
        status_t status = waveReference.Read(file, reference);
        if (NO_ERROR != status)
        {
            printf("Failed in reading %s: error=%d\n", file, status);
            return ;
        }
        mfio::Wave::MetaData metaData = waveReference.GetMetaData();
        printf("fs=%d\n", metaData.samplingRate);

        if ( fs != metaData.samplingRate )
        {
            printf("fs doesn't match: %d != %d\n", fs, metaData.samplingRate);
        }
    }
    const int n = audioIn.GetLength();
    const int M = 64; // a design parameter.
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
    for (int i = 0; i < h.GetColumnLength(); ++i)
    {
        if (max < fabs(h[i][0]))
        {
            max = fabs(h[i][0]);
        }
    }
    mcon::Vector<int16_t> coefs(h.Transpose()[0] * (32767.0/max));
    mfio::Wave filter(fs, 1, 16);
    filter.Write("rls_taps_coefficients.wav", coefs);
    h.DumpMatrix(h, "%f");
    mcon::Vector<double> hoge(M);
    for (int i = 0; i < M; ++i)
    {
        hoge[i] = h[i][0];
    }
    mcon::Matrix<double> comp(2, M);
    mcon::Matrix<double> gp(2, M/2);
    Fft::Ft(comp, hoge);
    Fft::ConvertToGainPhase(gp, comp);
    FILE*fp = fopen("coef_ft.csv", "w");
    double df = 1.0  * fs / M;
    for (int i = 1; i < gp.GetColumnLength(); ++i)
    {
        fprintf(fp, "%f,%f,%f\n",
            i*df, gp[0][i], gp[1][i]*180/M_PI);
    }
    fclose(fp);
}

static void test_IR(void)
{
    mfio::Wave wave;
    mcon::Vector<double> pcm;

    wave.Read("Trig_Room.wav", pcm);
    mfio::Wave::MetaData metaData = wave.GetMetaData();

    int length = 0;
    if (1 == metaData.numChannels)
    {
    }
    else if (2 == metaData.numChannels)
    {
    }
    printf("Length=%d\n", pcm.GetLength() / metaData.numChannels);
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

status_t RlsFromAudio(const char* audioInFile, const char* irFile, int tapps)
{
    mcon::Vector<double> ir;
    mcon::Vector<double> audioIn;
    mcon::Vector<double> audioInConv;
    int fs;

    {
        MakeTestIr(irFile, tapps);

        mfio::Wave wave;
        status_t status;

        LOG("Loading ir file ... ");
        char name[256];
        sprintf(name, "ir_test_%d.wav", tapps);
        status = wave.Read(name, ir);
        if ( NO_ERROR != status )
        {
            printf("An error occured: error=%d\n", status);
            return status;
        }
        LOG("Normalizing ir ... ");
        Normalize(ir);
        LOG("Done\n");

        fs = wave.GetSamplingRate();

        LOG("Sampling rate: %d\n", fs);
    }

    // Audio-in
    {
        mfio::Wave wave;

        LOG("Loading audio input ... ");
        status_t status = wave.Read(audioInFile, audioIn);

        if (NO_ERROR != status)
        {
            printf("An error occured during reading %s: error=%d\n", audioInFile, status);
            return -ERROR_CANNOT_OPEN_FILE;
        }
        LOG("Done\n");
        LOG("Length: %d\n", audioIn.GetLength());
        if (fs != wave.GetSamplingRate())
        {
            ERROR_LOG("Not matched the sampling rates: %d <=> %d\n", fs, wave.GetSamplingRate());
            return -ERROR_ILLEGAL;
        }

#if 0
        LOG("Normalizing audio-in ... ");
        if (1)
        {
            Normalize(audioIn);
        }
        else
        {
            double max = 27179412.822902;
            audioIn /= max;
        }
        LOG("Done\n");
#endif
    }

    // Convolution
    {
        audioInConv.Resize(audioIn.GetLength());

        LOG("Convoluting ir and audio-in ... ");
        Convolution(audioInConv, audioIn, ir);
        LOG("Done\n");

        // No need to normalize.
        // Normalize(audioInConv);
    }

    {
        const int n = audioIn.GetLength();
        const int M = ir.GetLength(); /// a design parameter.
        double c = 0.5; // an appropriately small number
        mcon::Matrix<double> P = mcon::Matrix<double>::E(M);
        mcon::Vector<double> _h(M);
        _h = 0;
        mcon::Matrix<double> h(_h.Transpose());
        mcon::Vector<double>& d = audioIn;
        mcon::Vector<double> uv(M);
        mcon::Vector<double> e(n);
        mcon::Vector<double> eta(n);
        mcon::Vector<double> J(n);
        mcon::Vector<double> K(n);
        mcon::Vector<double> U(n);

        P /= c;
        uv = 0;

        LOG("Now executing RLS with %d samples.\n", n);
        for (int i = 0; i < n; ++i)
        {
            uv.Unshift(audioInConv[i]);
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
        LOG("\n");
        {
            mcon::Vector<double> coefs(h.Transpose()[0]);
            mcon::Matrix<double> complex(2, ir.GetLength());
            mcon::Matrix<double> ir_gp(2, ir.GetLength());
            mcon::Matrix<double> coefs_gp(2, coefs.GetLength());
            Fft::Ft(complex, ir);
            Fft::ConvertToGainPhase(ir_gp, complex);

            Fft::Ft(complex, coefs);
            Fft::ConvertToGainPhase(coefs_gp, complex);
            char _fname[128];
            sprintf(_fname, "ir_vs_coefs_%d_", tapps);
            std::string fname(_fname);
            fname += std::string(audioInFile);
            fname.erase( fname.length()-4, 4);
            fname += std::string(".csv");
            FILE* fp = fopen(fname.c_str(), "w");
            if (NULL != fp)
            {
                double df = 1.0  * fs / M;
                fprintf(fp, "freq,Gain(IR),Gain(Coefs)\n");
                for (int i = 1; i < ir_gp.GetColumnLength(); ++i)
                {
                    fprintf(fp, "%g,%g,%g\n", i*df, ir_gp[0][i], coefs_gp[0][i]);
                }
                fprintf(fp, "\n");
                fprintf(fp, "freq,Phase(IR),Phase(Coefs)\n");
                for (int i = 1; i < ir_gp.GetColumnLength(); ++i)
                {
                    fprintf(fp, "%g,%g,%g\n", i*df, ir_gp[1][i]*180/M_PI, coefs_gp[1][i]*180/M_PI);
                }
                fprintf(fp, "\n");
                fprintf(fp, "i,IR,Coef\n");
                for (int i = 0; i < ir.GetLength(); ++i)
                {
                    fprintf(fp, "%d,%g,%g\n", i, ir[i], coefs[i]);
                }
                fprintf(fp, "\n");
#if 1
                // Dump some variables to evaluate the error left.
                fprintf(fp, "i,e,eta,J,|k|,|u|\n");
                for (int i = 0; i < n; ++i)
                {
                    fprintf(fp, "%d,%g,%g,%g,%g,%g\n", i, e[i], eta[i], J[i], K[i], U[i]);
                }
#endif
                fclose(fp);
            }
        }
    }
    return NO_ERROR;
}

status_t RlsFrom2Audio(const char* audioInFile, const char* audioRefFile, int tapps)
{
    mcon::Vector<double> audioIn;
    mcon::Vector<double> audioRef;
    int fs;

    // Audio-in
    {
        mfio::Wave wave;
        LOG("Loading audio input ... ");
        status_t status = wave.Read(audioInFile, audioIn);

        if (NO_ERROR != status)
        {
            printf("An error occured during reading %s: error=%d\n", audioInFile, status);
            return -ERROR_CANNOT_OPEN_FILE;
        }
        LOG("Done\n");

        fs = wave.GetSamplingRate();

    }

    LOG("SamplingRate: %d\n", fs);
    LOG("Length (audioIn) : %d\n", audioIn.GetLength());

    // Audio-ref
    {
        mfio::Wave wave;

        LOG("Loading audio reference ... ");
        status_t status = wave.Read(audioRefFile, audioRef);

        if (NO_ERROR != status)
        {
            printf("An error occured during reading %s: error=%d\n", audioRefFile, status);
            return -ERROR_CANNOT_OPEN_FILE;
        }
        LOG("Done\n");
        LOG("Length: %d\n", audioRef.GetLength());
        if (fs != wave.GetSamplingRate())
        {
            ERROR_LOG("Not matched the sampling rates: %d <=> %d\n", fs, wave.GetSamplingRate());
            return -ERROR_ILLEGAL;
        }

        LOG("Done\n");
    }

    LOG("Length (audioRef): %d\n", audioRef.GetLength());

    {
        const int n = audioIn.GetLength();
        const int M = tapps;
        double c = 0.5; // an appropriately small number
        mcon::Matrix<double> P = mcon::Matrix<double>::E(M);
        mcon::Vector<double> _h(M);
        _h = 0;
        mcon::Matrix<double> h(_h.Transpose());
        mcon::Vector<double>& d = audioIn;
        mcon::Vector<double> uv(M);
        mcon::Vector<double> e(n);
        mcon::Vector<double> eta(n);
        mcon::Vector<double> J(n);
        mcon::Vector<double> K(n);
        mcon::Vector<double> U(n);

        P /= c;
        uv = 0;

        LOG("Now executing RLS with %d samples.\n", n);
        for (int i = 0; i < n; ++i)
        {
            uv.Unshift(audioRef[i]);
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
        LOG("\n");
        {
            mcon::Vector<double> coefs(h.Transpose()[0]);
            mcon::Matrix<double> complex(2, coefs.GetLength());
            mcon::Matrix<double> coefs_gp(2, coefs.GetLength());

            Fft::Ft(complex, coefs);
            Fft::ConvertToGainPhase(coefs_gp, complex);
            char _fname[128];
            sprintf(_fname, "ir_vs_coefs_%d_", tapps);
            std::string fname(_fname);
            fname += std::string(audioInFile);
            fname.erase( fname.length()-4, 4);
            fname += std::string(".csv");
            FILE* fp = fopen(fname.c_str(), "w");
            if (NULL != fp)
            {
                double df = 1.0  * fs / M;
                fprintf(fp, "freq,Gain(Coefs)\n");
                for (int i = 1; i < coefs_gp.GetColumnLength(); ++i)
                {
                    fprintf(fp, "%g,%g\n", i*df, coefs_gp[0][i]);
                }
                fprintf(fp, "\n");
                fprintf(fp, "freq,Phase(Coefs)\n");
                for (int i = 1; i < coefs_gp.GetColumnLength(); ++i)
                {
                    fprintf(fp, "%g,%g\n", i*df, coefs_gp[1][i]*180/M_PI);
                }
                fprintf(fp, "\n");
                fprintf(fp, "i,Coef\n");
                for (int i = 0; i < coefs.GetLength(); ++i)
                {
                    fprintf(fp, "%d,%g\n", i, coefs[i]);
                }
                fprintf(fp, "\n");
#if 1
                // Dump some variables to evaluate the error left.
                fprintf(fp, "i,e,eta,J,|k|,|u|\n");
                for (int i = 0; i < n; ++i)
                {
                    fprintf(fp, "%d,%g,%g,%g,%g,%g\n", i, e[i], eta[i], J[i], K[i], U[i]);
                }
#endif
                fclose(fp);
            }
        }
    }
    return NO_ERROR;
}

status_t RlsFromIr(const char* irFile, int tapps)
{
    mcon::Vector<double> ir;
    int fs;

    {
        mcon::Vector<double> ir_all;
        mfio::Wave wave;
        status_t status;

        MakeTestIr(irFile, tapps);
        char irf[128];
        sprintf(irf, "ir_test_%d.wav", tapps);
        LOG("Loading ir file ... ");
        status = wave.Read(irf, ir_all);
        if ( NO_ERROR != status )
        {
            printf("An error occured: error=%d\n", status);
            return status;
        }
        LOG("Done\n");
        const int ch = wave.GetNumChannels();
        const int length = ir_all.GetLength();
        ir.Resize(length / ch);
        for (int i = 0; i < length / ch; ++i)
        {
            ir[i] = ir_all[ch*i];
        }
        //Normalize(ir);

        fs = wave.GetSamplingRate();
        LOG("Sampling rate: %d\n", fs);
    }

    {
        const int n = ir.GetLength();
        const int M = tapps;
        double c = 0.5; // an appropriately small number
        mcon::Matrix<double> P = mcon::Matrix<double>::E(M);
        mcon::Vector<double> _h(M);
        _h = 0;
        mcon::Matrix<double> h(_h.Transpose());
        mcon::Vector<double> d(n);
        mcon::Vector<double> uv(M);
        mcon::Vector<double> e(2*n);
        mcon::Vector<double> eta(2*n);
        mcon::Vector<double> J(2*n);
        mcon::Vector<double> K(2*n);
        mcon::Vector<double> U(2*n);
        mcon::Vector<double> ea(5);
        const double threshold = 1.0e-9;

        ea = 1.0;
        P /= c;
        uv = 0;

        // インパルス応答
        {
            d = 0;
            d[0] = 32767.0;
        }
        // 直接伝達による立ち上がり (と思われる箇所) を調べる。
        // タップ数が少ない場合、伝達の遅延を破棄しつつ、
        // ゲインの推定精度を上げられると思われる。
        int s = 0;
        double max = fabs(ir[0]);
        // 1 秒間を調べる
        // これより遅い立ち上がりはないはず。
        for (int i = 1; i < fs; ++i)
        {
            const double v = fabs(ir[i]);
            if (v > max)
            {
                max = v;
                s = i;
            }
        }
        printf("Max = %g at %d\n", max, s);
        s = 0;

        LOG("Now executing RLS with %d samples.\n", n);
        for (int i = s; i < 2*n; ++i)
        {
            uv.Unshift(ir[i]);
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
                LOG("%4.1f [%%]: %d/%d\r", i*100.0/2/n, i, 2*n);
            }
#if 0
            ea.Unshift(e[i] * eta[i]);
            if (ea.GetAverage() < threshold && i > M)
            {
                LOG("\nThe error is enough small (%g with %d iter), so exit\n", ea.GetAverage(), i);
                break;
            }
#endif
        }
        LOG("\n");
        {
            mcon::Vector<double> coefs(h.Transpose()[0]);
            mcon::Matrix<double> complex(2, coefs.GetLength());
            mcon::Matrix<double> ir_gp(2, coefs.GetLength());
            mcon::Matrix<double> coefs_gp(2, coefs.GetLength());

            Fft::Ft(complex, ir);
            Fft::ConvertToGainPhase(ir_gp, complex);

            Fft::Ft(complex, coefs);
            Fft::ConvertToGainPhase(coefs_gp, complex);
            char _fname[128];
            sprintf(_fname, "ir_vs_coefs_%d_", tapps);
            std::string fname(_fname);
            fname += std::string(irFile);
            fname.erase( fname.length()-4, 4);
            fname += std::string(".csv");
            FILE* fp = fopen(fname.c_str(), "w");
            if (NULL != fp)
            {
                double df = 1.0  * fs / M;
                fprintf(fp, "freq,Gain(IR),Gain(Coefs)\n");
                for (int i = 1; i < coefs_gp.GetColumnLength(); ++i)
                {
                    fprintf(fp, "%g,%g,%g\n", i*df, ir_gp[0][i], coefs_gp[0][i]);
                }
                fprintf(fp, "\n");
                fprintf(fp, "freq,Phase(IR),Phase(Coefs)\n");
                for (int i = 1; i < coefs_gp.GetColumnLength(); ++i)
                {
                    fprintf(fp, "%g,%g,%g\n", i*df, ir_gp[1][i]*180/M_PI, coefs_gp[1][i]*180/M_PI);
                }
                fprintf(fp, "\n");
                fprintf(fp, "i,Ir,Coef\n");
                for (int i = 0; i < coefs.GetLength(); ++i)
                {
                    fprintf(fp, "%d,%g,%g\n", i, ir[i], coefs[i]);
                }
                fprintf(fp, "\n");
#if 1
                // Dump some variables to evaluate the error left.
                fprintf(fp, "i,e,eta,J,|k|,|u|\n");
                for (int i = 0; i < e.GetLength(); ++i)
                {
                    fprintf(fp, "%d,%g,%g,%g,%g,%g\n", i, e[i], eta[i], J[i], K[i], U[i]);
                }
#endif
                fclose(fp);
            }
        }
    }
    printf("\n\n");
    return NO_ERROR;
}

// sweep_440-3520_10s.wav を Trig_Room.wav と畳み込んだが、
// 振幅の調整が出鱈目なので、ws で確認したゲインの違いをもとに
// レベルを合わせるための処理
// 一回きりのもの。
// →出鱈目でも良いのではないか？
static void MakeReference(void)
{
    mcon::Vector<double> data;
    mfio::Wave wave;
    std::string fbody("sweep_440-3520_10s");
    std::string fname = fbody + std::string(".wav");
    wave.Read(fname.c_str(), data);
    double ratio = 0.101936005111598 / 0.417508606968193;
    data *= ratio;
    std::string name = fbody + std::string("_ref.wav");
    wave.Write(name.c_str(), data);
}

status_t GetEnergyRatio(mcon::Vector<double>& ratio, const mcon::Vector<double>& data);

status_t GetEnergyRatio(mcon::Vector<double>& ratio, const mcon::Vector<double>& data)
{
    ratio = data;   // 代入
    ratio *= data;  // 二乗
    double E = ratio.GetSum(); // 合計
#if 0
    double E = 0.0;

    if ( false == ratio.Resize(data.GetLength() )
    {
        return -ERROR_CANNOT_ALLOCATE_MEMORY;
    }

    for (int i = 0; i < data.GetLength(); ++i)
    {
        ratio[i] = data[i] * data[i]);
        E += ratio[i];
    }
#endif
    ratio /= E; // 合計で割る

    return NO_ERROR;
}

static int sign(double v)
{
    return (v < 0) ? -1 : 1;
}

static void EstimateIr(void)
{
    const std::string ewav(".wav");
    const std::string ecsv(".csv");

    std::string wbody("sweep_440-3520_1s");
    std::string ibody("101-st");
    mcon::Matrix<double> ir;
    mcon::Vector<double> audio;
    mcon::Vector<double> audioConv;

    mfio::Wave wave;
    wave.Read(ibody + ewav, ir);
    {
        printf("ch=%d\n", wave.GetNumChannels());
        printf("fs=%d\n", wave.GetSamplingRate());
        printf("length=%d\n", ir.GetColumnLength());
        mfio::Csv::Write(ibody + ecsv, ir);
    }
    {
        mfio::Wave wave;
        wave.Read(wbody + ewav, audio);
        LOG("Length (audio): %d\n", audio.GetLength());
        LOG("Convoluting\n");
        Convolution(audioConv, audio, ir[0]);

        const double max = audioConv.GetMaximumAbsolute();
        LOG("Maximum=%f\n", max);
        audioConv *= 32767.0/max;

        wave.SetNumChannels(1);
        wave.Write(wbody + std::string("_") + ibody + ewav, audioConv);
    }
    {
        mcon::Vector<double> eRatio;
        GetEnergyRatio(eRatio, ir[0]);

        const int n = eRatio.GetLength();
        mcon::Matrix<double> matrix(2, n);

        matrix[0] = eRatio;
        matrix[1] = eRatio;
        for (int i = 1; i < n; ++i)
        {
            matrix[1][i] += matrix[1][i-1];
        }
        mfio::Csv::Write("101-st_eratio.csv", matrix);
    }

    {
        /*
         * ++ -> +-
         * +- -> ++
         * -+ -> --
         * -- -> -+
         */
        mcon::Vector<double>& vector = ir[0];
        const int n = vector.GetLength();
        mcon::Matrix<double> complex;
        mcon::Matrix<double> icomplex(2, n);
        mcon::Matrix<double> gp;
        mcon::Matrix<double> igp(2, n);
        mcon::Vector<double> inv(n);

        LOG("Ft\n");
        Fft::Ft(complex, vector);
        LOG("ToPolar\n");
        Fft::ConvertToPolarCoords(gp, complex);
        LOG("ToInversedComplex\n");
        for (int i = 0; i < n; ++i)
        {
            const double gain = gp[0][i];
            const double phase = gp[1][i];

            icomplex[0][i] = cos(2*M_PI-phase) / gain;
            icomplex[1][i] = sin(2*M_PI-phase) / gain;
        }
        LOG("ToPolar, again\n");
        Fft::ConvertToPolarCoords(igp, icomplex);
        LOG("Saving\n");
        {
            mcon::Matrix<double> matrix(4, n);
            matrix[0] = gp[0];
            matrix[1] = igp[0];
            matrix[2] = gp[1];
            matrix[3] = igp[1];
            mfio::Csv::Write("101-st_gp_igp.csv", matrix);
        }
        mcon::Vector<double> irInversed;
        LOG("Ift\n");
        Fft::Ift(irInversed, icomplex);
        printf("n=%d, m=%d\n", icomplex.GetRowLength(), icomplex.GetColumnLength());
        printf("irInversed=%d\n", irInversed.GetLength());
        mcon::Vector<double> audioInversed;
        LOG("Convoluting, again\n");
        Convolution(audioInversed, audioConv, irInversed);
        const double max = audioInversed.GetMaximumAbsolute();
        LOG("Maximum=%f\n", max);
        audioInversed *= 32767.0/max;
        LOG("Saving to compare\n");
        {
            mfio::Wave wave;
            wave.SetMetaData(44100, 1, 16, mfio::Wave::LPCM);
            status_t status = wave.Write(wbody + std::string("_inversed") + ewav, audioInversed);
            if (NO_ERROR != status)
            {
                LOG("Failed in writing with error=%d\n", status);
            }
            mcon::Matrix<double> matrix(2, audioInversed.GetLength());
            matrix[0] = audio;
            matrix[1] = audioInversed;
            mfio::Csv::Write(wbody + std::string("_compare") + ecsv, matrix);
        }
        LOG("Saving as wav\n");
        printf("format=%d\n", wave.GetWaveFormat());
        printf("fs=%d\n", wave.GetSamplingRate());
        printf("bits=%d\n", wave.GetBitDepth());
        printf("ch=%d\n", wave.GetNumChannels());
        wave.SetWaveFormat(mfio::Wave::IEEE_FLOAT);
        wave.SetNumChannels(1);
        status_t status = wave.Write("101-st_left_inversed.wav", irInversed);
        mfio::Csv::Write("101-st_left_inversed.csv", irInversed);
        printf("status=%d\n", status);
    }
}

int main(int argc, char* argv[])
{
    //MakeTestWave();
    //test_RLS();
    //MakeReference();
    if (1)
    {
        EstimateIr();
        return 0;
    }
    if (0)
    {
        int fs;
        std::string fbody("Trig_Room");
        std::string fwave = fbody + std::string(".wav");
        mcon::Vector<double> ir;
        GetIr(fwave.c_str(), ir, fs);
        const int n = ir.GetLength();
        LOG("IR length: %d\n", n);
        mcon::Matrix<double> complex(2, n);
        mcon::Matrix<double> gp(2, n);
        Fft::Ft(complex, ir);
        Fft::ConvertToPolarCoords(gp, complex);
        std::string fcsv = fbody + std::string("_spectrum.csv");
        FILE* fp = fopen(fcsv.c_str(), "w");
        if (NULL != fp)
        {
            const double df = 1.0  * fs / n;
            fprintf(fp, "freq,gain,phase\n");
            for (int i = 1; i < n; ++i)
            {
                fprintf(fp, "%g,%g,%g\n", i*df, gp[0][i], gp[1][i]*180/M_PI);
            }
            fclose(fp);
        }
        else
        {
            LOG("An error occured when opening file with write-attribute\n");
        }
        return 0;
    }
    if (0)
    {
        std::string ref("sweep_440-3520_10s_ref.wav");
        std::string in("sweep_440-3520_conv_Trig_Room.wav");
        RlsFrom2Audio(in.c_str(), ref.c_str(), 64);
        return 0;
    }

    if(1)
    {
        mfio::Wave wave;
        mcon::Matrix<double> impluse;
        //wave.Read("Trig_Room.wav", impluse);
        wave.Read("101-st.wav", impluse);
        EnergyRatio(impluse[0]);

        return 0;
    }

    std::string fbody("sweep_440-3520_1s");
    if (argc > 1)
    {
        fbody = std::string(argv[1]);
    }
    printf("Proccessing for %s.wav\n", fbody.c_str());
    std::string audioIn = fbody + std::string(".wav");
    int tapps[] = {16, 64, 256, 0};//, 512, 1024, 2048, 0};

    for (int i = 0; tapps[i] != 0; ++i)
    {
        printf("tapps=%d\n", tapps[i]);
        //RlsFromAudio(audioIn.c_str(), "Trig_Room.wav", tapps[i]);
        RlsFromIr("Trig_Room.wav", tapps[i]);
    }
    printf("Done\n\n");
    return 0;
}
