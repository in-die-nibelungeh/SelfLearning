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

#include "Clockwatch.h"

namespace {
    inline uint64_t rdtsc()
    {
        uint64_t ret;
//        __asm__ volatile ("rdtsc" : "=A" (ret));
        __asm__ volatile ("rdtsc; shlq $32, %%rdx; orq %%rdx, %%rax" : "=A" (ret) :: "%rdx");
        return ret;
    }
} // anonymous

namespace mutl {

Clockwatch::Clockwatch()
    : m_LastScore(0)
    , m_Base(rdtsc())
{}

Clockwatch::~Clockwatch()
{}

uint64_t Clockwatch::Tick(void)
{
    uint64_t end = rdtsc();
    if (end <= m_Base)
    {
        m_LastScore = __LONG_MAX__ - m_Base + end + 1;
    }
    else
    {
        m_LastScore = end - m_Base;
    }
    m_Base = end;

    return m_LastScore;
}

uint64_t Clockwatch::GetLastRecord(void) const
{
   return m_LastScore;
}

} // namespace mutl {
