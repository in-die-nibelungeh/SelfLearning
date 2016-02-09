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

#include <string>

#include "mcon.h"

#include "Common.h"

#define DUPLICATE(v) (v), (v)

namespace {

const int LowestSampleCount = 512;

int GetU1Position(size_t value)
{
    if (0 == value)
    {
        return -1;
    }
    for (int k = (sizeof(size_t) * 8 - 1); k >= 0; --k)
    {
        if (value & (1 << k))
        {
            return k;
        }
    }
    ASSERT(false);
    return -2;
}

size_t GetLowerLimitSize(size_t frequency)
{
    const size_t tmp = frequency / 100;
    size_t size = 0;

    for (size_t k = 31; k >= 0; --k)
    {
        if (tmp & (1 << k))
        {
            size = 1 << k;
            // 半分を超えていたら、さらに 1 ビットシフトさせる。
            if (k == 0 || k == 31)
            {
                break;
            }
            size <<= ( tmp & (1 << (k - 1)) ) ? 1 : 0;
            break;
        }
    }
    ASSERT( 0 != size );
    return size;
}

/*
 * サンプリングレートとサンプル数から、適切なウィンドウサイズを決める。
 * 1) サンプリングレートの 1/100 くらいの分解能を実現する。
 * 2) ウィンドウ幅で区切った時、端数が少なくなるように幅を定める。
 * 3) 少し余った場合は切り捨て、少し足りない場合は 0 埋めする。
 */
size_t GetWindowLength(size_t n, size_t samplingRate)
{
    const size_t upperWidth = ( 1 << GetU1Position(n * 2 - 1) );
    const size_t lowerWidth = GetLowerLimitSize(samplingRate);

    if (upperWidth < lowerWidth)
    {
        return upperWidth;
    }

    int caseCount = 0;
    DEBUG_LOG("upperWidth: %d (%08x)\n", DUPLICATE( static_cast<int>(upperWidth) ));
    DEBUG_LOG("lowerWidth: %d (%08x)\n", DUPLICATE( static_cast<int>(lowerWidth) ));

    // 31 は 32 bit での最大左シフト数 ( 1 << 31 )、念のため。
    for ( ;upperWidth != (lowerWidth << caseCount) && caseCount < 31; ++caseCount);
    caseCount++;
    DEBUG_LOG("case: %d\n", caseCount);

    mcon::Vector<double> ratio(caseCount);

    for (size_t w = lowerWidth, k = 0; w <= upperWidth; ++k, w *= 2)
    {
        const size_t rest = n % w;
        ratio[k] = static_cast<double>(rest) / w;
        DEBUG_LOG("n=%d, %5d: %f (%d)\n", static_cast<int>(n), static_cast<int>(w), static_cast<double>(rest) / w, static_cast<int>(rest));
    }
    const double max = ratio.GetMaximum();
    const double min = ratio.GetMinimum();

    // (1 - max) は「どれだけ足りないか」、min はどれだけ余っているか。
    // 小さい方を採用する。
    const double searchTarget = (1.0 - max < min) ? max : min;
    DEBUG_LOG("Cut or Shotage: %f\n", searchTarget);

    int index = -1;
    for (int k = ratio.GetLength() - 1; k >= 0 ; --k)
    {
        if (ratio[k] == searchTarget)
        {
            index = k;
            break;
        }
    }
    ASSERT(index != -1);
    DEBUG_LOG("Size: %d\n", static_cast<int>(lowerWidth << index) );
    return (lowerWidth << index);
}

}

/*
 * 階乗かどうか。
 * main からも呼ばれるので、無名空間には入れない。
 */
bool IsFactorial(const size_t _value, const size_t base)
{
    size_t value = _value;
    if (0 == base || 0 == value)
    {
        return false;
    }
    for ( ; value > 1; value /= base )
    {
        if (0 != (value % base))
        {
            return false;
        }
    }
    return true;
}

/*
 * サンプル数とウィンドウ長の調整をする。
 * 1) サンプル数は、信号長かユーザが指定した値のどちらかである。
 *    ユーザ指定値を優先する。
 * 2) ウィンドウ長が指定されている場合、これを尊重する (2 の階乗)。
 *    サンプル数が既定値より小さい場合にのみ、変更される。
 *    未指定時は、1 の情報に基づき決める。
 * 3) サンプル数調整では信号長を直接変更する。
 *    使用サンプル数と信号長の関係により、切り捨てか伸長 (0 埋め) が実施される。
 * 4) サンプル数が既定値 (512) より少ない場合、ft で処理する。
 *
 */
status_t PreProcess(ProgramParameter* param)
{
    const size_t signalLength = param->signal.GetColumnLength();
    const size_t sampleCount = param->sampleCount > 0 ? param->sampleCount : signalLength;
    //----------------------------------------------------------------
    // ウィンドウ長を決定する
    //----------------------------------------------------------------
    // ウィンドウサイズが未指定の場合
    if (0 == param->windowLength)
    {
        // サンプル長が2 の階乗なら FFT で良い。
        if ( IsFactorial(sampleCount, 2) )
        {
            param->isUsedOnlyFt = false;
            param->windowLength = sampleCount;
        }
        else
        {
            param->windowLength = (true == param->isUsedOnlyFt) ?
                sampleCount : GetWindowLength(sampleCount, param->samplingRate);
        }
    }
    // ウィンドウサイズが指定されている場合
    else
    {
        ASSERT( IsFactorial(param->windowLength, 2) );
        // ウィンドウ幅が無駄に長い場合は短くする。
        if (param->windowLength > (signalLength << 1))
        {
            param->windowLength = 1 << (GetU1Position(signalLength) + 1);
        }
    }
    // 既定数よりサプル数が小さい場合は ft で処理する。
    if (sampleCount < LowestSampleCount)
    {
        param->isUsedOnlyFt = true;
        param->windowLength = sampleCount;
    }

    //----------------------------------------------------------------
    // 使用サンプル数を決定する
    //----------------------------------------------------------------
    const size_t width = param->windowLength;
    const double ratio = static_cast<double>(sampleCount % width) / width;
    const int windowCount = (sampleCount / width) + (ratio >= 0.5f ? 1 : 0);
    const size_t sampleCountUsed = windowCount * width;

    //----------------------------------------------------------------
    // サイズを変更する (信号データを編集する)
    //----------------------------------------------------------------
    if (sampleCountUsed != signalLength)
    {
        const size_t ch = param->signal.GetRowLength();
        const size_t N = sampleCountUsed;
        const mcon::Matrix<double> tmp(param->signal);

        param->signal.Resize(ch, N);
        param->signal = 0;

        for (size_t k = 0; k < ch; ++k)
        {
            param->signal[k].Copy(tmp[k]);
        }
    }

    return NO_ERROR;
}
