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
    for (int i = 0; i < pcm.GetLength(); ++i)
    {
        ir[i] = static_cast<double>(pcm[ch*i]);
    }
    return NO_ERROR;
}

static status_t GetAudioIn(mcon::Vector<int16_t>& audioIn)
{
    FileIo wave;
    int status = wave.Read( (std::string(fname_body) + std::string(".wav")).c_str(), audioIn);

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

static status_t MakeTestIr(int length)
{
    const int ir_length = length;

    mcon::Vector<double> impluse;
    int fs = 0;

    int status = GetIr("Trig_Room.wav", impluse, fs);

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
    FileIo ir(fs, 1, 16);

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
            const double threshold_cut = 0.8;
            int cut;
            double sum = 0.0;
            for (int cut = 0; cut < seps; ++cut)
            {
                sum += energy[cut];
                if (sum/E > threshold_cut)
                {
                    break;
                }
            }
            tmp = tmp(0, (cut+1) * width);
        }
    } while (exit == false);

    return NO_ERROR;
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
    mcon::Vector<int16_t> reference;
    mcon::Vector<int16_t> audioIn;
    {
        const char* file = (std::string(fname_body) + std::string("_conv.wav")).c_str();
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
        const char* file = (std::string(fname_body) + std::string(".wav")).c_str();
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
    FileIo filter(fs, 1, 16);
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
        MakeTestIr(tapps);

        mcon::Vector<int16_t> ir_int;
        FileIo wave;
        status_t status;

        LOG("Loading ir file ... ");
        char name[256];
        sprintf(name, "ir_test_%d.wav", tapps);
        status = wave.Read(name, ir_int);
        if ( NO_ERROR != status )
        {
            printf("An error occured: error=%d\n", status);
            return status;
        }
        LOG("Done\n");
        ir = ir_int;

        LOG("Normalizing ir ... ");
        Normalize(ir);
        LOG("Done\n");

        fs = wave.GetSamplingRate();

        LOG("Sampling rate: %d\n", fs);
    }

    // Audio-in
    {
        FileIo wave;
        mcon::Vector<int16_t> audioIn_int;

        LOG("Loading audio input ... ");
        status_t status = wave.Read(audioInFile, audioIn_int);

        if (NO_ERROR != status)
        {
            printf("An error occured during reading %s: error=%d\n", audioInFile, status);
            return -ERROR_CANNOT_OPEN_FILE;
        }
        LOG("Done\n");
        LOG("Length: %d\n", audioIn_int.GetLength());
        if (fs != wave.GetSamplingRate())
        {
            ERROR_LOG("Not matched the sampling rates: %d <=> %d\n", fs, wave.GetSamplingRate());
            return -ERROR_ILLEGAL;
        }

        LOG("Copying to global audio-in ... ");
        audioIn = audioIn_int;
        LOG("Done\n");

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

        P /= c;
        uv = 0;

        LOG("Now executing RLS with %d samples.\n", n);
        for (int i = 0; i < n; ++i)
        {
            uv.Unshift(audioInConv[i]);
            const mcon::Matrix<double>& u = uv.Transpose();
            mcon::Matrix<double> k(P.Multiply(u)); // numerator
            double denom = 1.0;
            const mcon::Matrix<double>& denominator = u.Transpose().Multiply(P).Multiply(u);
            ASSERT(denominator.GetRowLength() == 1 && denominator.GetColumnLength() == 1);
            denom = denominator[0][0] + 1;
            k /= denom;

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
                fprintf(fp, "i,e,eta,J\n");
                for (int i = 0; i < n; ++i)
                {
                    fprintf(fp, "%d,%g,%g,%g\n", i, e[i], eta[i], J[i]);
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
        mcon::Vector<int16_t> ir_int;
        FileIo wave;
        status_t status;

        LOG("Loading ir file ... ");
        status = wave.Read(irFile, ir_int);
        if ( NO_ERROR != status )
        {
            printf("An error occured: error=%d\n", status);
            return status;
        }
        LOG("Done\n");
        ir = ir_int;

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
        mcon::Vector<double> e(n);
        mcon::Vector<double> eta(n);
        mcon::Vector<double> J(n);
        mcon::Vector<double> ea(5);
        const double threshold = 1.0e-9;

        ea = 1.0;
        P /= c;
        uv = 0;
        d = 0;

        {
            d[0] = 1.0;
            /*
            d[n/2] = 1;
            if (0 == (n & 1))
            {
                d[n/2-1] = 1;
            }
            */
        }
        LOG("Now executing RLS with %d samples.\n", n);
        for (int i = 0; i < n; ++i)
        {
            uv.Unshift(ir[i]);
            const mcon::Matrix<double>& u = uv.Transpose();
            mcon::Matrix<double> k(P.Multiply(u)); // numerator
            double denom = 1.0;
            const mcon::Matrix<double>& denominator = u.Transpose().Multiply(P).Multiply(u);
            ASSERT(denominator.GetRowLength() == 1 && denominator.GetColumnLength() == 1);
            denom = denominator[0][0] + 1;
            k /= denom;

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
            ea.Unshift(e[i] * eta[i]);
            if (ea.GetAverage() < threshold && i > M)
            {
                LOG("\nThe error is enough small (%g with %d iter), so exit\n", ea.GetAverage(), i);
                break;
            }
        }
        LOG("\n");
        {
            mcon::Vector<double> coefs(h.Transpose()[0]);
            mcon::Matrix<double> complex(2, coefs.GetLength());
            mcon::Matrix<double> coefs_gp(2, coefs.GetLength());
            //Fft::Ft(complex, ir);
            //Fft::ConvertToGainPhase(ir_gp, complex);

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
                fprintf(fp, "i,Ir,Coef\n");
                for (int i = 0; i < coefs.GetLength(); ++i)
                {
                    fprintf(fp, "%d,%g,%g\n", i, ir[i], coefs[i]);
                }
                fprintf(fp, "\n");
#if 1
                // Dump some variables to evaluate the error left.
                fprintf(fp, "i,e,eta,J\n");
                for (int i = 0; i < n; ++i)
                {
                    fprintf(fp, "%d,%g,%g,%g\n", i, e[i], eta[i], J[i]);
                }
#endif
                fclose(fp);
            }
        }
    }
    return NO_ERROR;
}

int main(int argc, char* argv[])
{
    //MakeTestWave();
    //test_RLS();
    if(0)
    {
        FileIo wav;
        mcon::Vector<int16_t> impluse_int;
        wav.Read("Trig_Room.wav", impluse_int);
        mcon::Vector<double> impluse(impluse_int);
        EnergyRatio(impluse);
        return 0;
    }

    std::string fbody("sweep_440-3520_1s");
    if (argc > 1)
    {
        fbody = std::string(argv[1]);
    }
    printf("Proccessing for %s.wav\n", fbody.c_str());
    std::string audioIn = fbody + std::string(".wav");
    int tapps[] = {16, 64, 256, 512, 1024, 2048, 0};

    for (int i = 0; tapps[i] != 0; ++i)
    {
        printf("tapps=%d\n", tapps[i]);
        //RlsFromAudio(audioIn.c_str(), "Trig_Room.wav", tapps[i]);
        RlsFromIr("Trig_Room.wav", tapps[i]);
    }
    printf("Done\n\n");
    return 0;
}
