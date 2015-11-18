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

#pragma once

#include "types.h"
#include "mcon.h"

namespace masp {
namespace ft {

status_t Fft (double realPart[], double imaginaryPart[], double timeSeries[], int numData);
status_t Ifft(double timeSeries[], double realPart[], double imaginaryPart[], int numData);
status_t Ft  (double realPart[], double imaginaryPart[], const double timeSeries[], int numData);
status_t Ift (double timeSeries[], const double realPart[], const double imaginaryPart[], int numData);

status_t Ft  (mcon::Matrix<double>& complex, const mcon::Vector<double>& timeSeries);
status_t Fft (mcon::Matrix<double>& complex, const mcon::Vector<double>& timeSeries);
status_t Ift (mcon::Vector<double>& timeSeries, const mcon::Matrix<double>& complex);
status_t Ifft(mcon::Vector<double>& timeSeries, const mcon::Matrix<double>& complex);

status_t ConvertToPolarCoords(mcon::Matrix<double>& gainPhase, const mcon::Matrix<double>& complex);

inline status_t ConvertToGainPhase(mcon::Matrix<double>& gainPhase, const mcon::Matrix<double>& complex)
{
    return ConvertToPolarCoords(gainPhase, complex);
}

status_t ConvertToComplex(mcon::Matrix<double>& complex, const mcon::Matrix<double>& polar);

} // namespace ft {
} // namespace masp {

