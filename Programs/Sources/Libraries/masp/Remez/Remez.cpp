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

#include "status.h"

namespace masp {
namespace remez {

namespace {
    mcon::Vectord FindLocalMinima(const mcon::Vectord& v)
    {
        const int N = v.GetLength();
        mcon::Vectord _indices(N);
        int count = 0;

        for (int k = 1; k < N-1; ++k)
        {
            if ( v[k-1] > v[k] && v[k] < v[k+1])
            {
                _indices[count] = k;
                count++;
            }
        }
        mcon::Vectord indices(count);
        indices.Copy(_indices);
        return indices;
    }
    mcon::Vectord FindLocalMaxima(const mcon::Vectord& v)
    {
        const int N = v.GetLength();
        mcon::Vectord _indices(N);
        int count = 0;

        for (int k = 1; k < N-1; ++k)
        {
            if ( v[k-1] < v[k] && v[k] > v[k+1])
            {
                _indices[count] = k;
                count++;
            }
        }
        mcon::Vectord indices(count);
        indices.Copy(_indices);
        return indices;
    }
}

int EstimateFilterOrder(double fp, double _dp, double fs, double _ds)
{
    if ( fp > fs ||
         fp < 0  ||
         fs < 0  ||
         fp > 1  ||
         fs > 1  )
    {
        return -ERROR_ILLEGAL;
    }

    double dp, ds;
    if (_dp < _ds)
    {
        dp = _ds;
        ds = _dp;
    }
    else
    {
        dp = _dp;
        ds = _ds;
    }
    const double B = (fs - fp) / 2.0;
    const double D = (0.005309 * POW2(log10(dp)
                 + 0.071141 * log10(dp) - 0.4761) * log10(ds)
               - (0.00266 * POW2(log(dp) + 0.5941 * log10(dp) + 0.4278);
    const double F = 0.51244 * (log10(dp) - log10(ds)) + 11.01217;
    return static_cast<int>( (D - F * POW2(B)) / B + 1.5);
}

void CalculateCoefficients(mcon::Vectord& v);
{
    const int N = 13;
    const int Kp = 1;
    const int Ks = 2;
    const double wp = 0.4 * M_PI;
    const double ws = 0.5 * M_PI;
    const double wo = (ws + wp) / 2.0;
    const int L = 1000;
    mcon::Vectord w(L+1);
    for ( int k = 0; k <= w.GetLength(); ++k ) { w[k] = k * M_PI / L; }
    mcon::Vectord W(L+1);
    W = 0;
    for ( int k = 0; k <= W.GetLength(); ++k )
    {
        if ( w[k] <= wp )
        {
            W[k] = Kp;
        }
        else if ( ws <= w[k] )
        {
            W[k] = Ks;
        }
    }
    mcon::Vectord D(L+1);
    D = 0;
    for ( int k = 0; k < D.GetLength(); ++k )
    {
        if ( w[k] <= wo )
        {
            D[k] = 1;
        }
    }
    const int M = (N-1) / 2;
    const int R = M + 2;
    mcon::Vectord ki(R);
    ki[0] =  51;
    ki[1] = 101;
    ki[2] = 341;
    ki[3] = 361;
    ki[4] = 531;
    ki[5] = 671;
    ki[6] = 701;
    ki[7] = 851;

    for ( int k = 0; k < R; ++k )
    {
        printf("w[%d]=%g\n", k, w[k]);
    }

    mcon::Vectord m(M+1);
    for ( int k = 0; k < m.GetLength(); ++k )
    {
        m[k] = k;
    }
    const mcon::Matrixd matrix(R, R);

    for ( int i = 0; i < matrix.GetRowLength(); ++i )
    {
        const int last = matrix.GetColumnLength();
        const double omega = w[i];
        matrix[i][0] = 1;
        for ( int k = 1; k < last - 1; ++k )
        {
            matrix[i][k] = cos(omega * k);
        }
        matrix[i][last] = (i & 0x1 ? -1 : 1) / W[i];
    }
    const mcon::Matrix Dk(R, 1);
    for ( int k = 0; k < Dk.GetRowLength(); ++k )
    {
        Dk[k][0] = D[ki[k]];
    }
    mcon::Matrixd x( (matrix.I() * Dk).T() );

    printf("x=(%d, %d)\n", x.GetRowLength(), x.GetColumnLength());

    mcon::Vectord h(N);

    h[M] = x[0][0];
    for (int k = 0; k < M; ++k)
    {
        h[M-k-1] = h[M+k+1] = x[0][M-k];
    }
    {
        const int N = h.GetLength();
    }
}

}} // masp::remez
