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
#include <stdint.h>

#include "masp.h"
#include "mfio.h"

static void test_window(void)
{
    double coef[256];
    size_t N = 24;

    mcon::Vector<double> s(N), han(N), ham(N), b(N), bh(N), n(N), k(N), f(N);
    masp::window::Hanning(coef, N);
    masp::window::Rectangular(s);
    masp::window::Hanning(han);
    masp::window::Hamming(ham);
    masp::window::Blackman(b);
    masp::window::BlackmanHarris(bh);
    masp::window::Nuttall(n);
    masp::window::Kaiser(k, 3.0);
    masp::window::Flattop(f);
    printf(",Hanning,Hamming,Blackman,BlackmanHarris,Nuttall,Kaiser,Flattop,Rectangular\n");
    for (size_t i = 0; i < N; ++i)
    {
        printf("%d,%f,%f,%f,%f,%f,%f,%f,%f\n", static_cast<int>(i), han[i], ham[i], b[i], bh[i], n[i], k[i], f[i], s[i]);
    }
    N = 25;
    s.Resize(N);
    han.Resize(N);
    ham.Resize(N);
    b.Resize(N);
    bh.Resize(N);
    n.Resize(N);
    k.Resize(N);
    f.Resize(N);
    masp::window::Hanning(coef, N);
    masp::window::Rectangular(s);
    masp::window::Hanning(han);
    masp::window::Hamming(ham);
    masp::window::Blackman(b);
    masp::window::BlackmanHarris(bh);
    masp::window::Nuttall(n);
    masp::window::Kaiser(k, 3.0);
    masp::window::Flattop(f);
    printf("\n");
    printf(",Hanning,Hamming,Blackman,BlackmanHarris,Nuttall,Kaiser,Flattop,Rectangular\n");
    for (size_t i = 0; i < N; ++i)
    {
        printf("%d,%f,%f,%f,%f,%f,%f,%f,%f\n", static_cast<int>(i), han[i], ham[i], b[i], bh[i], n[i], k[i], f[i], s[i]);
    }
    return ;
}

static void test_tapps(void)
{
    const int Ms[] = {2, 4, 8, 16, 32, 64, 128};
    const int baseM = 1024;
    //mfio::Csv csv("hanning.csv");
    //mfio::Csv csv("rectangular.csv");
    mfio::Csv csv("sinc.csv");

    for (size_t i = 0; i < sizeof(Ms)/sizeof(int); ++i )
    {
        const double fe = 0.25;
        const int N = Ms[i] + 1;
        mcon::Vector<double> sinc(N);
        mcon::Vector<double> window(N);
        mcon::Vector<double> response(baseM);
        masp::fir::GetCoefficientsLpfSinc(sinc, fe);
        mcon::Matrix<double> complex;
        mcon::Matrix<double> gp;
        //masp::window::Hanning(window);
        masp::window::Rectangular(window);
        response = 0;
        window *= sinc;
        response.Copy(window);
        for ( int i = 0; i < (baseM - N)/2; ++i )
        {
            response.Unshift(0);
        }

        masp::ft::Ft(complex, response);
        masp::ft::ConvertToPolarCoords(gp, complex);
        gp[0] /= gp[0].GetMaximumAbsolute();
        {
            const int n = response.GetLength();
            mcon::Matrix<double> matrix(3, n);
            matrix[1] = gp[0];
            for ( int k = 0; k < n; ++k )
            {
                matrix[0][k] = k * 1.0 / n;
                //matrix[1][k] = 20 * log10(matrix[1][k]);
            }
            matrix[2] = response;
            csv.Write(matrix);
            csv.Crlf();
        }
        //mfio::Csv::Write("fft.csv", gp[0]);
    }
}

int main(void)
{
    test_tapps();
    test_window();
    return 0;
}
