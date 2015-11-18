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

#include <sys/types.h>

namespace masp {
namespace iir {

class Biquad
{
public:
    enum FilterTypeId
    {
        LPF = 1,
        HPF,
        BPF,
        BEF
    };

    struct Coefficients
    {
        double a[2];
        double b[3];
    };

    struct Context
    {
        double x[2];
        double y[2];
    };

    Biquad();
    Biquad(double Q, double fc, int type, int samplingRate);
    ~Biquad() {};

    void Initialize(void);
    void CalculateCoefficients(double Q, double fc, int type, int samplingRate);

    const struct Coefficients& GetCoefficients(void) { return m_Coefficients; }
    const struct Context& GetContext(void) { return m_Context; }

    double ApplyFilter(double x0);

    static double ConvertD2A(double fd, int fs);

private:

    struct Coefficients m_Coefficients;
    struct Context m_Context;
};

} // namespace iir {
} // namespace masp {
