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
#include "debug.h"
#include "mcon.h"

namespace masp {

class Remez
{
public:

    static int EstimateOrder() const
    {
        const double wa, wp;
        const double dp;
        double B = fabs(wa - wp) / 2 / M_PI;
        double D = (0.005309 * POW2(log(dp))
                     + 0.071141 * log(dp) - 0.4761) * log(da);
                   - (0.00266 * POW2(log(dp)) + 0.5941 * log(dp) + 0.4278);
        double F = 0.51244 * (log(dp) - log(da)) + 11.012;
        return static_cast<int>( (D - F * POW2(B)) / B + 1.5);
    }
    Remez(int targetFs, int baseFs, double nFpass, double nFstop)
        : m_TargetFs(targetFs),
        m_BaseFs(baseFs),
        m_PassBandFrequency(nFpass),
        m_StopBandFrequency(nFstop)
        //m_PassBandRipple(0.01),
        //m_StopBandDecay(40)
    {
        ASSERT(targetFs > 0);
        ASSERT(baseFs > 0);
        ASSERT(0.0 < nFpass && nFpass < 1.0);
        ASSERT(0.0 < nFstop && nFstop < 1.0);
        ASSERT(nFpass < nFstop);

        Initialize(targetFs, baseFs, nFpass, nFstop);
    }

    ~Remez() {};
    status_t Initialize(int targetFs, int baseFs, double nFpass, double nFstop);
    status_t SetSamplingRates(int targetFs, int baseFs);
    status_t SetFilterParams(double nFpass, double nFstop);

    status_t GetCoefficients(mcon::Vector<double>& coefficients) const;

    status_t MakeFilterByWindowType(int windowType, double alpha = 2.0);
    status_t MakeFilterBySpec(double pbRipple, double sbDecay);

    status_t Convert(mcon::Vector<double>& output, const mcon::Vector<double>& input) const;

private:
    status_t UpdateCoefficients(int windowType, double alpha);
    int m_TargetFs;
    int m_BaseFs;
    int m_L;
    int m_M;
    double m_PassBandFrequency;
    double m_StopBandFrequency;
    mcon::Vector<double> m_Coefficients;
};

} // namespace masp {
