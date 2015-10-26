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

#include "Vector.h"
#include "debug.h"

namespace mcon {

class Matrixd;

class Vectord
{
public:

    explicit Vectord(const int length = 0);
    Vectord(const Vectord& v);
    template <typename U> Vectord(const Vectord& v)
        : m_Address(NULL),
        m_Length(v.GetLength())
    {
        Allocate();
        for ( int i = 0; m_Length; ++i )
        {
            (*this)[i] = static_cast<double>(v[i]);
        }
    }
    ~Vectord();

    // For const object
    const double& operator[](const int i) const
    {
        ASSERT (0 <= i && i < m_Length);
        return m_Address[i];
    }
    // For non-const object
    double& operator[](const int i)
    {
        ASSERT (0 <= i && i < m_Length);
        return m_Address[i];
    }

    // Copy is to copy available data from src to dest without resizing the dest.
    const Vectord& Copy(const Vectord& Vectord);
    // operator= make the same Vectord as the input Vectord.
    Vectord& operator=(const Vectord& Vectord);

    template <typename U>
    operator Vector<U>() const
    {
        Vector<U> v(GetLength());
        for ( int i = 0; m_Length; ++i )
        {
            v[i] = static_cast<U>((*this)[i]);
        }
        return v;
    }
    operator void*() const
    {
        return reinterpret_cast<void*>(m_Address);
    }
    operator double() const
    {
        return (*this)[0];
    }

    Vectord operator()(int offset, int length) const;
    double PushFromFront(double v);
    double PushFromBack(double v);
    double PopFromFront(double v);
    double PopFromBack(double v);

    Vectord& operator=(double v);

    const Vectord operator+(double v) const;
    const Vectord operator-(double v) const;
    const Vectord operator*(double v) const;
    const Vectord operator/(double v) const;

    const Vectord operator+(const Vectord& v) const;
    const Vectord operator-(const Vectord& v) const;
    const Vectord operator*(const Vectord& v) const;
    const Vectord operator/(const Vectord& v) const;

    Vectord& operator+=(double v);
    Vectord& operator-=(double v);
    Vectord& operator*=(double v);
    Vectord& operator/=(double v);

    Vectord& operator+=(const Vectord& v);
    Vectord& operator-=(const Vectord& v);
    Vectord& operator*=(const Vectord& v);
    Vectord& operator/=(const Vectord& v);
    double GetMaximum(void) const;
    double GetMaximumAbsolute(void) const;
    double GetMinimum(void) const;
    double GetMinimumAbsolute(void) const;

    double GetSum(void) const;

    double GetNorm(void) const;
    bool Resize(int length);

    inline int GetLength(void) const
    {
        return m_Length;
    }
    inline double GetAverage(void) const
    {
        return GetSum()/GetLength();
    }
    // Will be depricated.
    inline double Fifo(double v)
    {
        return PushFromBack(v);
    }
    inline double Stack(double v)
    {
        return PushFromBack(v);
    }
    inline double Pop(double v)
    {
        return PopFromBack(v);
    }
    inline double Shift(double v)
    {
        return PopFromFront(v);
    }
    inline double Unshift(double v)
    {
        return PushFromFront(v);
    }
    inline bool IsNull(void) const
    {
        return m_Length == 0;
    }

private:
    // Private member functions.
    inline int Smaller(int a1, int a2) const { return a1 < a2 ? a1 : a2; }
    inline int Smaller(int input) const { return m_Length < input ? m_Length : input; }
    inline int Larger(int a1, int a2) const { return a1 > a2 ? a1 : a2; }
    inline int Larger(int input) const { return m_Length < input ? input : m_Length ; }
    inline double Absolute(double v) const { return (v < 0) ? -v : v; }
    void  Allocate(void);
    // Private member variables.
    double*  m_Address;
    int      m_Length;
};

} // namespace mcon {
