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
#include <stdint.h>

#include "masp.h"
#include "mcon.h"
#include "mfio.h"

static const double g_Pi(M_PI);

static void test_sinc(void)
{
    int SamplingRate = 8000;
    int PassBand =  500;
    int StopBand = 1500;
    int UpperBand = 2000;
    int TransferBandWidth = StopBand - PassBand;
    int cutOff = PassBand + TransferBandWidth/2;
    double fe = (double)cutOff/SamplingRate;
    double fe2 = (double)UpperBand/SamplingRate;
    double scoef[256];
    double lcoef[256];
    size_t N = 24;
    mcon::Vector<double> slpf(N), shpf(N), sbpf(N), sbef(N), han(N);
    masp::window::Hanning(han);
    masp::fir::FilterSinc(scoef, N, fe);
    masp::fir::GetCoefficientsLpfSinc(slpf, fe);
    masp::fir::GetCoefficientsHpfSinc(shpf, fe);
    masp::fir::GetCoefficientsBpfSinc(sbpf, fe, fe2);
    masp::fir::GetCoefficientsBefSinc(sbef, fe, fe2);
    masp::fir::FilterLanczos(lcoef, N, fe, 2.0);

    FILE* fh = fopen("result.csv", "wb");
    ASSERT(fh != NULL);

    slpf *= han;
    fprintf(fh, "N,i,sinc,sinc_n,lanczos\n");
    for (size_t i = 0; i < N; ++i)
    {
        fprintf(fh, "%d,%d,%f,%f,%f\n", static_cast<int>(N), static_cast<int>(i), scoef[i], slpf[i], lcoef[i]);
    }
    fprintf(fh, "\n");
    fprintf(fh, "N,i,slpf,shpf,sbpf,sbef\n");
    for (size_t i = 0; i < N; ++i)
    {
        fprintf(fh, "%d,%d,%f,%f,%f,%f\n", static_cast<int>(N), static_cast<int>(i), slpf[i], shpf[i], sbpf[i], sbef[i]);
    }
    N = 25;
    han.Resize(N);
    slpf.Resize(N);
    shpf.Resize(N);
    sbpf.Resize(N);
    sbef.Resize(N);
    masp::window::Hanning(han);
    masp::fir::FilterSinc(scoef, N, fe);
    masp::fir::GetCoefficientsLpfSinc(slpf, fe);
    masp::fir::GetCoefficientsHpfSinc(shpf, fe);
    masp::fir::GetCoefficientsBpfSinc(sbpf, fe, fe2);
    masp::fir::GetCoefficientsBefSinc(sbef, fe, fe2);
    masp::fir::FilterLanczos(lcoef, N, fe, 2.0);
    slpf *= han;
    fprintf(fh, "\n");
    fprintf(fh, "N,i,sinc,sinc_n,lanczos\n");
    for (size_t i = 0; i < N; ++i)
    {
        fprintf(fh, "%d,%d,%f,%f,%f\n", static_cast<int>(N), static_cast<int>(i), scoef[i], slpf[i], lcoef[i]);
    }
    fprintf(fh, "\n");
    fprintf(fh, "N,i,slpf,shpf,sbpf,sbef\n");
    for (size_t i = 0; i < N; ++i)
    {
        fprintf(fh, "%d,%d,%f,%f,%f,%f\n", static_cast<int>(N), static_cast<int>(i), slpf[i], shpf[i], sbpf[i], sbef[i]);
    }
    return ;
}

int main(void)
{
    test_sinc();
}
