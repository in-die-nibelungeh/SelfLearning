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

#pragma once

#include "types.h"

#include "mcon.h"

namespace masp {
namespace fir {

//void GetCoefficients(double coef[], size_t N, double fe, int filterType, int functionId);
//void HpfSinc(double coef[], size_t N, double fe);
void FilterSinc(double coef[], size_t N, double fe);
void FilterLanczos(double coef[], size_t N, double fe, double n);

void GetCoefficientsLpfSinc(mcon::Vector<double>& coef, double fe);
void GetCoefficientsHpfSinc(mcon::Vector<double>& coef, double fe);
void GetCoefficientsBpfSinc(mcon::Vector<double>& coef, double fe1, double fe2);
void GetCoefficientsBefSinc(mcon::Vector<double>& coef, double fe1, double fe2);
void GetCoefficientsLpfLanczos(mcon::Vector<double>& coef, double fe, double n);
void GetCoefficientsHpfLanczos(mcon::Vector<double>& coef, double fe, double n);
void GetCoefficientsBpfLanczos(mcon::Vector<double>& coef, double fe1, double fe2, double n);
void GetCoefficientsBefLanczos(mcon::Vector<double>& coef, double fe1, double fe2, double n);

inline void GetCoefficientsLpfSinc(mcon::Vector<double>& coef, int cutOff, int samplingRate)
{
    GetCoefficientsLpfSinc(coef, static_cast<double>(cutOff)/samplingRate);
}
inline void GetCoefficientsHpfSinc(mcon::Vector<double>& coef, int cutOff, int samplingRate)
{
    GetCoefficientsHpfSinc(coef, static_cast<double>(cutOff)/samplingRate);
}
inline void GetCoefficientsBpfSinc(mcon::Vector<double>& coef, int cutOff1, int cutOff2, int samplingRate)
{
    GetCoefficientsBpfSinc(coef, static_cast<double>(cutOff1)/samplingRate, static_cast<double>(cutOff2)/samplingRate);
}
inline void GetCoefficientsBefSinc(mcon::Vector<double>& coef, int cutOff1, int cutOff2, int samplingRate)
{
    GetCoefficientsBefSinc(coef, static_cast<double>(cutOff1)/samplingRate, static_cast<double>(cutOff2)/samplingRate);
}
inline void GetCoefficientsLpfLanczos(mcon::Vector<double>& coef, int cutOff, int samplingRate, double n)
{
    GetCoefficientsLpfLanczos(coef, static_cast<double>(cutOff)/samplingRate, n);
}
inline void GetCoefficientsHpfLanczos(mcon::Vector<double>& coef, int cutOff, int samplingRate, double n)
{
    GetCoefficientsHpfLanczos(coef, static_cast<double>(cutOff)/samplingRate, n);
}
inline void GetCoefficientsBpfLanczos(mcon::Vector<double>& coef, int cutOff1, int cutOff2, int samplingRate, double n)
{
    GetCoefficientsBpfLanczos(coef, static_cast<double>(cutOff1)/samplingRate, static_cast<double>(cutOff2)/samplingRate, n);
}
inline void GetCoefficientsBefLanczos(mcon::Vector<double>& coef, int cutOff1, int cutOff2, int samplingRate, double n)
{
    GetCoefficientsBefLanczos(coef, static_cast<double>(cutOff1)/samplingRate, static_cast<double>(cutOff2)/samplingRate, n);
}

// This is for Hanning, not the others.
int GetNumOfTapps(double delta);
inline int GetNumOfTapps(double passband, int samplingRate)
{
    return GetNumOfTapps(static_cast<double>(passband)/samplingRate);
}

status_t Convolution(mcon::Vector<double>& out, const mcon::Vector<double>& in, const mcon::Vector<double>& impluse);

} // namespace fir {
} // namespace masp {
