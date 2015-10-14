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

status_t RlsFromTwoWaveforms(const char* inputFile, const char* refFile, int tapps)
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

    LOG("\n");
    LOG("[Input]\n");
    LOG("    SamplingRate: %d\n", fs);
    LOG("    Length      : %d\n", input.GetLength());

    // Audio-ref
    {
        mfio::Wave wave;
        mcon::Matrix<double> referenceMatrix;
        LOG("Loading audio reference ... ");
        status_t status = wave.Read(refFile, referenceMatrix);

        if (NO_ERROR != status)
        {
            printf("An error occured during reading %s: error=%d\n", refFile, status);
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
    LOG("\n");
    LOG("[Reference]\n");
    LOG("    Length      : %d\n", reference.GetLength());

    if ( input.GetLength() > reference.GetLength() )
    {
        LOG("\nWarning: input is longer than reference.\n");
        LOG("This may cause a wrong estimation.\n");
    }
    {
        LOG("\n");
        LOG("Initializing ... ");
        const int n = input.GetLength();
        const int M = tapps;
        double c = 0.5; // an appropriately small number
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
            fbody += std::string(refFile);
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
                for (int i = 1; i < length; ++i)
                {
                    gain[0][i] = i*df;
                    gain[1][i] = -20 * log10(gp[0][i]/max);
                }
                std::string fname = fbody + std::string("_gain") + ecsv;
                mfio::Csv csv(fname);
                csv.Write("freq,Gain[dB]\n");
                csv.Write(gain);
                csv.Close();
                LOG("Output: %s\n", fname.c_str());
            }
            // Phase
            {
                mcon::Matrix<double> phase(2, length);
                const double df = 1.0  * fs / M;
                for (int i = 1; i < length; ++i)
                {
                    phase[0][i] = i*df;
                    phase[1][i] = gp[1][i]*180/M_PI;
                }
                std::string fname = fbody + std::string("_phase") + ecsv;
                mfio::Csv csv(fname);
                csv.Write("freq,Phase[deg]\n");
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
            LOG("Verifying ... \n");
            const int n = input.GetLength();
            mcon::Vector<double> origin(n);
            Convolution(origin, input, resp);
            LOG("Done\n");
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

static void usage(void)
{
    printf("Usage: %s INPUT REFERENCE [TAPPS]\n", "rls");
    printf("\n");
    printf("INPUT and REFERENCE must be .wav file.\n");
    printf("TAPPS must be an interger value which is larger than 0.\n");
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

    printf("Input: %s\n", input.c_str());
    printf("Reference: %s\n", reference.c_str());
    printf("Tapps: %d\n", tapps);

    RlsFromTwoWaveforms(input.c_str(), reference.c_str(), tapps);

    printf("Done\n\n");
    return 0;
}
