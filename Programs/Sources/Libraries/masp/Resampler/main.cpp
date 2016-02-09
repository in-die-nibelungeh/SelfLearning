/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2015-2016 Ryosuke Kanata
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include <stdio.h>
#include <string>

#include "debug.h"
#include "mcon.h"
#include "mfio.h"
#include "masp/Resampler.h"

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
#if defined(__WIN64)
        LOG("Length=%I64d\n", coefs.GetLength());
#else
        LOG("Length=%lld\n", coefs.GetLength());
#endif
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
#if defined(__WIN64)
        LOG("Length=%I64d\n", coefs.GetLength());
#else
        LOG("Length=%lld\n", coefs.GetLength());
#endif
    }
    {
        mcon::Vector<double> output;
        resampler.Convert(output, input);
        output *= 32767.0/output.GetMaximumAbsolute();

        mfio::Wave w(targetFs, wave.GetNumChannels(), wave.GetBitDepth());
        w.Write(fbody + std::string("_resampled2.wav"), output);
    }
    // サンプルの数を変更する
    // ==> 実質的に周波数変換？
    // ==> 違う気がする...
    {
        const int N = 100;
        const double ratio = 1.5;
        const double fp = 0.35;
        const double fs = 0.45;
        const double ripple = 0.01;
        const double decay = 80;
        mcon::Vector<double> src(N);
        for (int k = 0; k < N; ++k)
        {
            src[k] = k;
        }
        resampler.Initialize(static_cast<int>(ratio * N), N, fp, fs);
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
