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

#include "mcon.h"

namespace masp {
namespace window {

void Rectangular(double w[], size_t N);
void Rectangular(mcon::Vector<double>& w);
void Hanning(double w[], size_t N);
void Hanning(mcon::Vector<double>& w);
void Hamming(double w[], size_t N);
void Hamming(mcon::Vector<double>& w);
void GeneralizedHamming(double w[], size_t N, double a);
void GeneralizedHamming(mcon::Vector<double>& w, double a);
void Blackman(double w[], size_t N);
void Blackman(mcon::Vector<double>& w);
void BlackmanHarris(double w[], size_t N);
void BlackmanHarris(mcon::Vector<double>& w);
void Nuttall(double w[], size_t N);
void Nuttall(mcon::Vector<double>& w);
void Kaiser(double w[], size_t N, double a);
void Kaiser(mcon::Vector<double>& w, double a);
void Flattop(double w[], size_t N);
void Flattop(mcon::Vector<double>& w);

} // namespace window {
} // namespace masp {
