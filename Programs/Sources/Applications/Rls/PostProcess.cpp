/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2015 Ryosuke Kanata
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

#include <string>

#include "masp.h"
#include "mfio.h"

#include "Common.h"

namespace {
    status_t Verify(mcon::Vector<double>& origin, const mcon::VectordBase& input, const mcon::VectordBase& estimated)
    {
        status_t status = NO_ERROR;
        const int N = input.GetLength();
        UNUSED(N);
        ASSERT( N > 0 );
        ASSERT( estimated.GetLength() > 0 );

        status = masp::fir::Convolution(origin, input, estimated);
        ASSERT( NO_ERROR == status );
        ASSERT( ! origin.IsNull() );
        return status;
    }
}

status_t PostProcess(ProgramParameter* param)
{
    status_t status = NO_ERROR;

    // レベル合わせ
    // 1. 周波数領域で合わせる案
    //    どの周波数帯域で合わせるかが問題になった。
    //    最大の帯域でレベル合わせすると、聴感が変わってくる。
    //    フラットな特性への補正であればやり易いが、そうではなくなっている。
    // 2. エネルギーで合わせる
    //    何に相当する？周波数帯域で合わせるのとは何が違う？
    //    やりたいことは 3 と同じか？
    // 3. 時間領域で合わせる (採)
    //    係数の絶対値を加算した結果が 1.0f 以下になるように合わせる。
    //    (あり得ない想定だが) 全タップにわたり係数と同じ符号の 32767 が
    //    入力された場合に、16bit の正最大値を超えないようにするための調整。
    LOG("Post processing ...\n");
    {
        const size_t ch = param->inversedSignal.GetRowLength();
        const size_t N = param->inversedSignal.GetColumnLength();

        mcon::Vectord sumups(ch);
        mcon::Vectord energies(ch);

        for (uint r = 0; r < ch; ++r)
        {
            // チャンネル毎のレベル調整
            const mcon::VectordBase& signal = param->inversedSignal[r];
            energies[r] = sqrt(signal.Dot(signal));
            LOG("    Ch-%d Energy: %g\n", r, energies[r]);
            // 積算 (畳み込み) する時のための最大値調整 (全体)
            double sumup = 0;
            for (uint k = 0; k < N; ++k)
            {
                sumup += fabs(signal[k]);
            }
            sumups[r] = sumup;
        }
        LOG("    Ajusted Level:\n");
        // レベル調整 (上げる方向)
        const double maxEnergy = energies.GetMaximum();
        for (uint r = 0; r < ch; ++r)
        {
            mcon::VectordBase& signal = param->inversedSignal[r];
            const double extendFactor =
                sqrt( maxEnergy / energies[r]  );
            signal *= extendFactor;
            LOG("    Ch-%d Energy: %g\n", r, sqrt(signal.Dot(signal)) );
        }
        param->inversedSignal *= param->upperValue / sumups.GetMaximum();
    }

    LOG("Verifying ... \n");
    {
        const int ch = param->inputSignal.GetRowLength();
        const int N = param->inputSignal.GetColumnLength();
        mcon::Matrix<double> saved(ch, N);
        for (int c = 0; c < ch; ++c)
        {
            const mcon::VectordBase& input(param->inputSignal[c]);
            const mcon::VectordBase& inversed(param->inversedSignal[c]);
            mcon::Vectord origin;
            status = Verify(
                origin,
                input,
                inversed);
            if ( NO_ERROR != status )
            {
                ERROR_LOG("Failed in Verify(): error=%d\n", status);
                return status;
            }
            saved[c] = origin;
        }
        const std::string ewav(".wav");
        const std::string filepath = param->outputBase + std::string("_iconv") + ewav;
        const int Pcm32Bit = 32;
        mfio::Wave wave(param->samplingRate, ch, Pcm32Bit, mfio::Wave::IEEE_FLOAT);
        status = wave.Write(filepath, saved);
        LOG("    Output: %s\n", filepath.c_str());
        if (NO_ERROR != status)
        {
            ERROR_LOG("Failed in writing %s: error=%d\n", filepath.c_str(), status);
        }
    }
    return status;
}
