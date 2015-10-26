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

//#include <x86intrin.h>

#include "debug.h"
#include "Vectord.h"

#define ALIGN(n) //__attribute__((aligned(n)))

namespace mcon {

class Matrixd;

#define MCON_ITERATION(var, iter, statement)  \
    do {                                      \
        for (int var = 0; var < iter; ++var)  \
        {                                     \
            statement;                        \
        }                                     \
    } while(0)

Vectord::Vectord(int length)
    : m_Address(NULL),
    m_Length(length)
{
    Allocate();
}


Vectord::Vectord(const Vectord& v)
    : m_Address(NULL),
    m_Length(v.GetLength())
{
    Allocate();
    MCON_ITERATION(i, m_Length, (*this)[i] = v[i]);
}


Vectord::~Vectord()
{
    if (NULL != m_Address)
    {
        delete[] m_Address;
        m_Address = NULL;
    }
    m_Length = 0;
}

Vectord Vectord::operator()(int offset, int length) const
{
    Vectord carveout;
    if (offset < 0 || GetLength() <= offset || length < 0)
    {
        // Null object.
        return carveout;
    }
    // Smaller value as length
    carveout.Resize( Smaller(GetLength() - offset, length) );
    for (int i = offset; i < Smaller(offset + length); ++i)
    {
        carveout[i-offset] = (*this)[i];
    }
    return carveout;
}

double Vectord::PushFromBack(double v)
{
    double ret = (*this)[0];
    for (int i = 0; i < GetLength() - 1; ++i)
    {
        (*this)[i] = (*this)[i+1];
    }
    (*this)[GetLength()-1] = v;
    return ret;
}

double Vectord::PushFromFront(double v)
{
    double ret = (*this)[GetLength()-1];
    for (int i = GetLength() - 1; i > 0; --i)
    {
        (*this)[i] = (*this)[i-1];
    }
    (*this)[0] = v;
    return ret;
}

Vectord& Vectord::operator=(double v) { MCON_ITERATION(i, m_Length, (*this)[i] = v); return *this; };

const Vectord Vectord::operator+(double v) const { Vectord vec(*this);  vec += v; return vec; }
const Vectord Vectord::operator-(double v) const { Vectord vec(*this);  vec -= v; return vec; }
const Vectord Vectord::operator*(double v) const { Vectord vec(*this);  vec *= v; return vec; }
const Vectord Vectord::operator/(double v) const { Vectord vec(*this);  vec /= v; return vec; }

const Vectord Vectord::operator+(const Vectord& v) const { Vectord vec(*this);  vec += v; return vec; }
const Vectord Vectord::operator-(const Vectord& v) const { Vectord vec(*this);  vec -= v; return vec; }
const Vectord Vectord::operator*(const Vectord& v) const { Vectord vec(*this);  vec *= v; return vec; }
const Vectord Vectord::operator/(const Vectord& v) const { Vectord vec(*this);  vec /= v; return vec; }

Vectord& Vectord::operator+=(double v) { MCON_ITERATION(i, m_Length, (*this)[i] += v); return *this; }
Vectord& Vectord::operator-=(double v) { MCON_ITERATION(i, m_Length, (*this)[i] -= v); return *this; }
Vectord& Vectord::operator*=(double v) { MCON_ITERATION(i, m_Length, (*this)[i] *= v); return *this; }
Vectord& Vectord::operator/=(double v) { MCON_ITERATION(i, m_Length, (*this)[i] /= v); return *this; }

Vectord& Vectord::operator+=(const Vectord& v) { MCON_ITERATION(i, Smaller(v.GetLength()), (*this)[i] += v[i]); return *this; }
Vectord& Vectord::operator-=(const Vectord& v) { MCON_ITERATION(i, Smaller(v.GetLength()), (*this)[i] -= v[i]); return *this; }
Vectord& Vectord::operator*=(const Vectord& v) { MCON_ITERATION(i, Smaller(v.GetLength()), (*this)[i] *= v[i]); return *this; }
Vectord& Vectord::operator/=(const Vectord& v) { MCON_ITERATION(i, Smaller(v.GetLength()), (*this)[i] /= v[i]); return *this; }

double Vectord::GetMaximum(void) const
{
    double max = (*this)[0];
    for (int i = 1; i < GetLength(); ++i)
    {
        if (max < (*this)[i])
        {
            max = (*this)[i];
        }
    }
    return max;
}

double Vectord::GetMaximumAbsolute(void) const
{
    double max = Absolute((*this)[0]);
    for (int i = 1; i < GetLength(); ++i)
    {
        const double v = Absolute((*this)[i]);
        if (max < v)
        {
            max = v;
        }
    }
    return max;
}

double Vectord::GetMinimum(void) const
{
    double min = (*this)[0];
    for (int i = 1; i < GetLength(); ++i)
    {
        if (min > (*this)[i])
        {
            min = (*this)[i];
        }
    }
    return min;
}

double Vectord::GetMinimumAbsolute(void) const
{
    double min = Absolute((*this)[0]);
    for (int i = 1; i < GetLength(); ++i)
    {
        const double v = Absolute((*this)[i]);
        if (min > v)
        {
            min = v;
        }
    }
    return min;
}


double Vectord::GetSum(void) const
{
    double sum = 0;
    MCON_ITERATION( i, GetLength(), sum += (*this)[i] );
    return sum;
}

double Vectord::GetNorm(void) const
{
    double squareSum = 0;
    MCON_ITERATION( i, GetLength(), squareSum += (*this)[i] * (*this)[i]);
    return sqrt(squareSum);
}

void Vectord::Allocate(void)
{
    m_Address = NULL;
    if (m_Length > 0)
    {
        m_Address = new ALIGN(32) double[m_Length];
        ASSERT(NULL != m_Address);
    }
}


const Vectord& Vectord::Copy(const Vectord& v)
{
    MCON_ITERATION(i, Smaller(v.GetLength()), (*this)[i] = v[i]);
    return *this;
}


Vectord& Vectord::operator=(const Vectord& v)
{
    // m_Length is updated in Resize().
    Resize(v.GetLength());
    MCON_ITERATION(i, v.GetLength(), (*this)[i] = v[i]);
    return *this;
}


bool Vectord::Resize(int length)
{
    if (length < 0)
    {
        return false;
    }
    if (length == m_Length)
    {
        return true;
    }
    if (NULL != m_Address)
    {
        delete[] m_Address;
        m_Address = NULL;
    }
    m_Length = length;
    m_Address = new double[length];
    ASSERT (NULL != m_Address);
    return true;
}

} // namespace mcon {
