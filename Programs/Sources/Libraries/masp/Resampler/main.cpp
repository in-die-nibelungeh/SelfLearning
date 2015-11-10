#include <stdio.h>
#include <string>

#include "debug.h"
#include "mcon.h"
#include "mfio.h"
#include "Resampler.h"

static void test_resampler(void)
{
    std::string fbody("sweep_440-3520_1s");
    mcon::Vector<double> input;
    mfio::Wave wave;
    wave.Read(fbody + std::string(".wav"), input);

    const int baseFs = wave.GetSamplingRate();
    const int targetFs = 16000;
    const double fp = 0.35;
    const double fs = 0.45;

    masp::Resampler resampler(targetFs, baseFs, fp, fs);
    resampler.MakeFilterByWindowType(masp::Resampler::HANNING);

    {
        mcon::Vector<double> coefs;
        resampler.GetCoefficients(coefs);
        LOG("Length=%d\n", coefs.GetLength());

    }
    {
        mcon::Vector<double> output;
        resampler.Convert(output, input);
        output *= 32767.0/output.GetMaximumAbsolute();

        mfio::Wave w(targetFs, wave.GetNumChannels(), wave.GetBitDepth());
        w.Write(fbody + std::string("_resampled1.wav"), output);
    }
    const double ripple = 0.01;
    const double decay = 80;

    resampler.MakeFilterBySpec(ripple, decay);

    {
        mcon::Vector<double> coefs;
        resampler.GetCoefficients(coefs);
        LOG("Length=%d\n", coefs.GetLength());

    }
    {
        mcon::Vector<double> output;
        resampler.Convert(output, input);
        output *= 32767.0/output.GetMaximumAbsolute();

        mfio::Wave w(targetFs, wave.GetNumChannels(), wave.GetBitDepth());
        w.Write(fbody + std::string("_resampled2.wav"), output);
    }
    // �T���v���̐���ύX����
    // ==> �����I�Ɏ��g���ϊ��H
    // ==> �Ⴄ�C������...
    {
        const int N = 100;
        const double ratio = 1/1.5;
        const double fp = 0.35;
        const double fs = 0.45;
        const double ripple = 0.01;
        const double decay = 80;
        mcon::Vector<double> src(N);
        for (int k = 0; k < N; ++k)
        {
            src[k] = k;
        }
        resampler.Initialize(N, static_cast<int>(ratio * N), fp, fs);
        resampler.MakeFilterBySpec(ripple, decay);

        mcon::Vector<double> dst;
        resampler.Convert(dst, src);
        mfio::Csv::Write("sample_count_convert.csv", dst);
    }
}

int main(void)
{
    test_resampler();
    return 0;
}
