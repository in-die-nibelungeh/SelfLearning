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

#define DEBUG_LOG(...)

#include "status.h"
#include "types.h"
#include "debug.h"
#include "mcon.h"

#include "Spline.h"

namespace {
    static const double threshold = 1.0e-10;
    bool IsNearlyEqualZero(double v)
    {
        return fabs(v) < threshold;
    }
} // anonymous
namespace mutl {
namespace interp {

status_t Spline::Interpolate(mcon::Vector<double>& output, const mcon::Vector<double>& input, int sampleCount)
{
    if ( sampleCount <= 0 )
    {
        return -ERROR_INVALID_ARGUMENT;
    }
    if ( false == output.Resize(sampleCount) )
    {
        return -ERROR_CANNOT_ALLOCATE_MEMORY;
    }
    const int N = input.GetLength();
    mcon::Matrix<double> equation;
    if ( false == equation.Resize(N-2, N-1) )
    {
        return -ERROR_CANNOT_ALLOCATE_MEMORY;
    };
    // 全要素を初期化
    equation = 0;

    // r=0 (開始点の方程式)
    {
        const int r = 0;
        equation[r][r    ] = 4;
        equation[r][r + 1] = 1;
    }

    // r=N-3 (終端の方程式)
    {
        const int r = N - 3;
        equation[r][r - 1] = 1;
        equation[r][r    ] = 4;
    }

    // r=1:N-4 (端点以外の方程式を設定)
    for ( int r = 1; r < equation.GetRowLength() - 1; ++r )
    {
        equation[r][r - 1] = 1; // h[r];
        equation[r][r    ] = 4; // (h[r] + h[r+1]) * 2;
        equation[r][r + 1] = 1; // h[r+1];
    }
    // v を代入
    for ( int r = 0; r < equation.GetRowLength(); ++r )
    {
        equation[r][N - 2] = 6 * (input[r+2] - 2 * input[r+1] + input[r]);
    }
    //--------------------------------
    // Gauss-Jordan
    //--------------------------------
    // 各行を正規化
    for ( int r = 0; r < equation.GetRowLength(); ++r )
    {
        equation[r] /= equation[r].GetMaximumAbsolute();
    }
    for ( int r = 0; r < equation.GetRowLength(); ++r )
    {
        // 注目している列の中で、最大値を持つ列を探す。
        int index = r;
        double maximum = fabs(equation[r][r]);
        for ( int k = index + 1; k < equation.GetRowLength() - 1; ++k )
        {
            if ( fabs(equation[k][r]) > maximum )
            {
                index = k;
                maximum = fabs(equation[k][r]);
            }
        }
        if ( IsNearlyEqualZero(maximum) )
        {
            // ここに到達することはないはず。
            DEBUG_LOG(" ~ 0 at r=%d\n", r);
            output.Resize(0); // NULL を返す。
            return -ERROR_ILLEGAL;
        }
        // 必要なら入れ替える
        if ( r != index )
        {
            mcon::Vector<double> tmp(equation[r]);
            equation[r] = equation[index];
            equation[index] = tmp;
        }
        // 上で入れ替えたので 以降では index は不要、r を使用する。
        // 注目している行の、注目している列の値を 1.0 にする
        equation[r] /= maximum;

        // 他の行から引く。
        for ( int m = 0; m < equation.GetRowLength(); ++m )
        {
            if ( m == r )
            {
                continue;
            }
            equation[m] -= (equation[r] * equation[m][r]);
        }
    }
    mcon::Vector<double> u;
    if ( false == u.Resize(N) )
    {
        return -ERROR_CANNOT_ALLOCATE_MEMORY;
    };
    u[0] = 0;
    u[N-1] = 0;
    for ( int k = 0; k < u.GetLength() - 2; ++k )
    {
        u[k+1] = equation[k][N - 2];
    }
    // u を取り出したのでもう不要、メモリを解放しておく
    equation.Resize(0, 0);

    // 係数計算
    mcon::Matrix<double> coefficients;
    if ( false == coefficients.Resize(4, N-1) ) // 4 は a, b, c, d
    {
        return -ERROR_CANNOT_ALLOCATE_MEMORY;
    };
    // 補間計算
    mcon::Vector<double>& a = coefficients[0];
    mcon::Vector<double>& b = coefficients[1];
    mcon::Vector<double>& c = coefficients[2];
    mcon::Vector<double>& d = coefficients[3];
    for ( int k = 0; k < coefficients.GetColumnLength(); ++k )
    {
        a[k] = (u[k+1] - u[k]) / 6;
        b[k] = u[k] / 2;
        c[k] = (input[k+1] - input[k]) - (2 * u[k] + u[k+1]) / 6;
        d[k] = input[k];
    }
    // 両端の値はループ外で代入しておく。
    const double step = static_cast<double>(N - 1) / (sampleCount - 1);
    output[0] = input[0];
    output[sampleCount - 1] = input[N - 1];
    for ( int k = 1; k < sampleCount - 1; ++k )
    {
        const double position = k * step;             // 換算した位置 (小数)
        const int index = static_cast<int>(position); // 入力配列インデックス (整数)
        const double frac = position - index;         // 小数部
        DEBUG_LOG("k=%d, frac=%g, index=%d, (a, b, c, d)=(%g, %g, %g, %g)\n", k, frac, index, a[index], b[index], c[index], d[index]);
        const double frac2 = frac * frac;
        const double frac3 = frac * frac2;
        output[k] = a[index] * frac3 + b[index] * frac2 + c[index] * frac + d[index];
    }
    return NO_ERROR;
}

} // namespace interp {
} // namespace mutl {
