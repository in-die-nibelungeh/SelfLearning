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


#include <unistd.h> // sleep

#include "debug.h"
#include "Clockwatch.h"
#include "Stopwatch.h"

namespace mutl {

Stopwatch::Stopwatch()
    : m_LastScore(0)
    , m_Base()
    , m_CorrelationFactor(0.0)
    , m_Correlated(false)
    , m_Clockwatch()
{
    clock_gettime(CLOCK_REALTIME, &m_Base.ts);
}

Stopwatch::~Stopwatch()
{}

double Stopwatch::Tick(void)
{
    if (IsCorrelated())
    {
        m_LastScore = m_CorrelationFactor  * m_Clockwatch.Tick();
    }
    else
    {
        struct timespec ts;

        clock_gettime(CLOCK_REALTIME, &ts);

        long int sec  = ts.tv_sec - m_Base.ts.tv_sec;
        long int nsec = ts.tv_nsec - m_Base.ts.tv_nsec;
        if (nsec < 0)
        {
            nsec += 1000000000L;
            sec  -= 1;
        }
        m_LastScore = sec + nsec * 1.0e-9;

        m_Base.ts = ts;
    }

    return m_LastScore;
}

void Stopwatch::Correlate()
{
    const int sleepPeriod = 1;
    m_CorrelationFactor = sleepPeriod;
    m_Clockwatch.Tick();
    sleep(sleepPeriod);
    m_CorrelationFactor /= m_Clockwatch.Tick();
    m_Correlated = true;
}

} // namespace mutl {
