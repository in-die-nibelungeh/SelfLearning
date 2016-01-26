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

#include "mfio.h"

#include "Common.h"

// These are in Process.cpp
void NormalEquationPre(
    mcon::Matrixd& Inversed,
    mcon::Matrixd& Ut,
    const mcon::Vectord& u,
    const mcon::Matrixd& W);

void NormalEquationPost(
    mcon::Vector<double>& h,
    const mcon::Matrixd& Inversed,
    const mcon::Matrixd& Ut,
    const mcon::Vectord& d,
    const mcon::Matrixd& W,
    double* pError);

namespace {

template <typename Type>
Type Clamp(Type min, Type in, Type max)
{
    if (in < min)
    {
        return min;
    }
    if (in > max)
    {
        return max;
    }
    return in;
}

template <typename Type>
Type ClampLower(Type min, Type in)
{
    if (in < min)
    {
        return min;
    }
    return in;
}

template <typename Type>
Type ClampUpper(Type in, Type max)
{
    if (in > max)
    {
        return max;
    }
    return in;
}

double CalculateSquaredSum(const mcon::VectordBase& signal)
{
    const mcon::Vectord v(signal);
    return v.Dot(v);
}

int FindBaseFromFront(const mcon::VectordBase& signal, const double energy, const double threshold)
{
    const uint N = signal.GetLength();
    int start = -1;
    double tmp = 0;
    // 前から探す
    for (uint k = 0; k < N; ++k)
    {
        tmp += signal[k] * signal[k];
        if (tmp / energy > threshold)
        {
            start = k;
            break;
        }
    }
    return start;
}

int FindBaseFromBack(const mcon::VectordBase& signal, const double energy, const double threshold)
{
    const uint N = signal.GetLength();
    int end = -1;
    double tmp = 0;
    // 後ろから探す
    for (uint k = N - 1; k >= 0; --k)
    {
        tmp += signal[k] * signal[k];
        if (tmp / energy > threshold)
        {
            end = k;
            break;
        }
    }
    return end;
}

void GetEnergySpotIndex(int* pStart, int* pEnd, const mcon::VectordBase& signal, double threshold = 0.95 /* TBD */)
{
    //const int minimumWidth = 8;            // TBD
    //const int maximumWidth = Clamp<int>(minimumWidth, tapps / 20, 64);  // TBD

    const double energy = CalculateSquaredSum(signal);

    *pStart = FindBaseFromBack (signal, energy, threshold);
    *pEnd   = FindBaseFromFront(signal, energy, threshold);
}

typedef struct _tagRange
{
    int start;
    int end;
    uint width;
} Range;


Range GetEnergySpotIndexOfInput(const mcon::Matrixd& input, const double threshold)
{
    const int N = input.GetColumnLength();
    int startIndex = N - 1;
    int endIndex = 0;
    for (int r = 0; r < input.GetRowLength(); ++r)
    {
        int startSpot = 0;
        int endSpot = 0;
        GetEnergySpotIndex(&startSpot, &endSpot, input[r], threshold);

        startIndex = std::min(startIndex, startSpot);
        endIndex = std::max(endIndex, endSpot);
    }
    const Range range =
    {
        startIndex,
        endIndex,
        static_cast<uint>(endIndex - startIndex + 1)
    };
    return range;
}

Range GetEnergySpotIndexOfReference(const mcon::Vectord& reference, const double threshold)
{
    int startIndex = 0;
    int endIndex = 0;
    GetEnergySpotIndex(&startIndex, &endIndex, reference, threshold);
    const Range range =
    {
        startIndex,
        endIndex,
        static_cast<uint>(endIndex - startIndex + 1)
    };
    return range;
}

int FindLocalMinimum(const mcon::Vectord& v)
{
    // 値が最少となる極小値を探す
    double minimum = v.GetMaximum();
    int index = -1;
    for (int k = 0; k < v.GetLength(); ++k)
    {
        if (v[k]==0)
        {
            continue;
        }
        DEBUG_LOG("%d,%g\n", k, log10(v[k]));
        if (0 == k || k == v.GetLength() - 1)
        {
            continue;
        }
        // 谷であり、かつ評価値が最少となる Js を選ぶ。
        if (v[k - 1] > v[k] && v[k] < v[k + 1])
        {
            if (minimum > v[k])
            {
                index = k;
                minimum = v[k];
            }
        }
    }
    return index;
}

int GetIterationCount(const uint M, const int split)
{
    int count = 0;
    for (int _range = M, step; 1 != step; _range = 3 * step, ++count)
    {
        step = ClampLower<int>(0, (_range + split - 1) / split);
    }
    return count;
}

// 動機
//   - 入力信号と参照信号の位置関係で (ずらし具合で) 誤差が異なる。
//
// 要件
//   - できる限り簡単な計算で最少誤差の場所を特定したい。

int Optimizer(
    int* pReferenceOffset,
    const mcon::Matrixd& input,
    const mcon::Vectord& reference,
    const int tapps,
    const bool outputLog,
    const std::string& outputBase
)
{
    const mcon::Matrixd W; // 重み行列 (使わない)
    const int split = 16;  // 探索範囲の分割数

    // 入力信号のエネルギが高い領域 (開始点と終了点) を取得する。
    // 入力信号長を短くしても結果が (大きく) 変わらないなら使いたい。
    const Range erInput = GetEnergySpotIndexOfInput(input, 0.999);
    DEBUG_LOG("Input    : %d-%d (width=%d)\n", erInput.start, erInput.end, erInput.width);
    UNUSED(erInput);

    // TBD:
    //   諸々のチェックを実施する必要がある (主に長さに関するチェック)。
    // 指定タップ数と同じ or 1/2 (計算量削減)
    //   1. 減らすと精度が落ち、数10サンプルずれた誤認識をした。
    //   2. もし減らすなら 1/2、数サンプル程度のずれだった。
    const uint M = erInput.width; // or tapps or tapps / 2;
    // タップ数の2倍か信号長か、短い方
    //const int N = std::min(M * 2, input.GetColumnLength());
    // 1. N == M にするとうまくいかない...
    // 2. N == 1.1 M くらいならまだうまくいくが、N == 1.05 M ではダメ。
    //    何故か理解できていないのが残念
    // とりあえず、タップ数の1.5 倍にしておく
    const int N = std::min(static_cast<int>(M * 1.5), input.GetColumnLength());

    const Range erReference = GetEnergySpotIndexOfReference(reference, 0.99);
    LOG("    Reference: %d-%d (width=%d)\n", erReference.start, erReference.end, erReference.width);
    int offset = ClampLower<int>(0, erReference.end - M + 1); // 捜索開始インデックス
    int range = M; // 捜索範囲
    DEBUG_LOG("M=%d, N=%d\n", M, N);

    const int iter = GetIterationCount(M, split);
    mcon::Matrixd log(iter, split + 1); // + 1 はインデックスを入れるため。

    for (int i = 0 ; ; ++i)
    {
        const int step = ClampLower<int>(0, (range + split - 1) / split);
        //const int end = ( (erReference.start - offset) + step - 1 ) / step;
        DEBUG_LOG("offset=%d, range=%d, step=%d\n", offset, range, step);
        LOG("    Stage-%d: Range=%d-%d, Step=%d\n", i + 1, offset, offset + range - 1, step);
        mcon::Vectord Js(split);

        Js = 0;

        for (int r = 0; r < input.GetRowLength(); ++r)
        {
            const mcon::Vectord _signal(input[r]);
            const mcon::Vectord signal = _signal(0, N);
            mcon::Matrixd Ut(M, N);
            DEBUG_LOG("Ut: (%d, %d)\n", Ut.GetRowLength(), Ut.GetColumnLength());
            mcon::Matrixd inversed;
            DEBUG_LOG("r=%d: NormalEquationPre()\n", r);
            NormalEquationPre(inversed, Ut, signal, W);
            DEBUG_LOG("Inv: (%d, %d)\n", inversed.GetRowLength(), inversed.GetColumnLength());
            DEBUG_LOG("r=%d: Do, Optimizing ...\n", r);
            for (int k = 0; k < split; ++k)
            {
                double J;
                mcon::Vector<double> h; // unused
                const mcon::Vectord d = reference(k * step + offset, N);
                ASSERT(d.GetLength() == N);
#if 0
            int startSpot = 0;
            int endSpot = 0;
            GetEnergySpotIndex(&startSpot, &endSpot, d, 0.99);
            DEBUG_LOG("Spot[%d]: Start=%d, End=%d, dlen=%d\n", k, startSpot, endSpot, d.GetLength());
#endif

                NormalEquationPost(h, inversed, Ut, d, W, &J);
                Js[ k ] += J;
            }
        }
        for (int k = 0; k < log.GetColumnLength() - 1; ++k)
        {
            log[i][k] = Js[k];
        }
        // 誤差配列で極小値をとるインデックスを取得する。
        int lmIndex = FindLocalMinimum(Js);

        // 見つからない場合は処理を打ち切る止める (TBD)。
        if (lmIndex < 0)
        {
            LOG("        ==> Not found any local minimum ... exiting.\n");
            break;
        }
        // Reference 信号上のインデックスに直す。
        lmIndex *= step;
        lmIndex += offset;
        log[i][log.GetColumnLength() - 1] = lmIndex;
        DEBUG_LOG("    Found: index=%d\n", lmIndex);
        LOG("        ==> Local minimum: %d.\n", lmIndex);
        // Step が 1 なら終了する。
        if (1 == step)
        {
            *pReferenceOffset = lmIndex;
            break;
        }
        // そうでなければ変数を更新して次のステージへ
        // 極小値が見つかった区間とその前後、合計3 の区間を対象にする。
        // そうすると、確実に次も極小値が見つかる。
        // なので...
        offset = ClampLower(0, lmIndex - step + 1); // オフセットは1 区間前の先頭に置く
        range = 3 * step; // 範囲は3 区間
    }
    // ここで変更する必要はない
    // *pReferenceOffset = 0;
    if (true == outputLog)
    {
        const std::string logFilepath = outputBase + std::string("_optlog.csv");
        LOG("    Output a log of optimizing: %s\n", logFilepath.c_str());
        if (NO_ERROR != mfio::Csv::Write(logFilepath, log))
        {
            ERROR_LOG("    Failed int writing %s\n", logFilepath.c_str());
        }
    }
    LOG("Done.\n\n");
    return 0;
}

}

status_t PreProcess(ProgramParameter* param)
{
    if (!param->optimize)
    {
        return NO_ERROR;
    }
    LOG("Optimizing ...\n");
    return Optimizer(
        &param->referenceOffset,
        param->inputSignal,
        param->referenceSignal,
        param->tapps,
        param->outputLog,
        param->outputBase);
}
