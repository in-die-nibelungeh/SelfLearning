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

class VectordBase
{
    friend class Vectord;
public:

    VectordBase(double* addressAligned, int length)
        : m_AddressAligned(addressAligned)
        , m_Length(length)
    {
#ifdef DEBUG
        if ( (reinterpret_cast<int>(addressAligned) % g_Alignment) != 0 )
        {
            printf("%s: Not aligned at %d (%p)\n", __func__, g_Alignment, addressAligned);
        }
#endif
        //ASSERT_ALIGNED(addressAligned, g_Alignment);
    }
    ~VectordBase()
    {
        m_AddressAligned = NULL;
        m_Length = 0;
    }

    // For const object
    const double& operator[](const int i) const
    {
        ASSERT (0 <= i && i < m_Length);
        return m_AddressAligned[i];
    }
    // For non-const object
    double& operator[](const int i)
    {
        ASSERT (0 <= i && i < m_Length);
        return m_AddressAligned[i];
    }

    // Copy is to copy available data from src to dest without resizing the dest.
    const VectordBase& Copy(const VectordBase& VectordBase);

    template <typename U>
    operator Vector<U>() const
    {
        const int n = GetLength();
        Vector<U> v(n);
        for ( int i = 0; n; ++i )
        {
            v[i] = static_cast<U>((*this)[i]);
        }
        return v;
    }
    operator void*() const
    {
        return reinterpret_cast<void*>(m_AddressAligned);
    }
    operator double*() const
    {
        return reinterpret_cast<double*>(m_AddressAligned);
    }

    double PushFromFront(double v);
    double PushFromBack(double v);
    double PopFromFront(double v);
    double PopFromBack(double v);

    VectordBase& operator=(double v);

    const VectordBase operator+(double v) const;
    const VectordBase operator-(double v) const;
    const VectordBase operator*(double v) const;
    const VectordBase operator/(double v) const;

    VectordBase& operator+=(double v);
    VectordBase& operator-=(double v);
    VectordBase& operator*=(double v);
    VectordBase& operator/=(double v);

    const VectordBase operator+(const VectordBase& v) const;
    const VectordBase operator-(const VectordBase& v) const;
    const VectordBase operator*(const VectordBase& v) const;
    const VectordBase operator/(const VectordBase& v) const;

    VectordBase& operator+=(const VectordBase& v);
    VectordBase& operator-=(const VectordBase& v);
    VectordBase& operator*=(const VectordBase& v);
    VectordBase& operator/=(const VectordBase& v);

    double GetMaximum(void) const;
    double GetMaximumAbsolute(void) const;
    double GetMinimum(void) const;
    double GetMinimumAbsolute(void) const;

    double GetSum(void) const;

    double GetNorm(void) const;
    double GetDotProduct(VectordBase& v) const;
    VectordBase GetCrossProduct(VectordBase& v) const;

    // Inline functions.
    inline int GetLength(void) const
    {
        return m_Length;
    }
    inline double GetAverage(void) const
    {
        return GetSum()/GetLength();
    }
    // Will be depricated.
    inline double FifoIn(double v)
    {
        return PushFromBack(v);
    }
    inline double FifoOut(double v)
    {
        return PopFromFront(v);
    }
    inline double Push(double v)
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
        return m_AddressAligned == NULL;
    }
    inline double Dot(VectordBase& v) const
    {
        return GetDotProduct(v);
    }
    inline VectordBase Cross(VectordBase& v) const
    {
        return GetCrossProduct(v);
    }

private:
    // Not significant in VectordBase,
    // but not in Vectord.
    explicit VectordBase(int length = 0)
        : m_AddressAligned(NULL)
        , m_Length(length)
    {}

    // Forbidden to be called.
    VectordBase& operator=(const VectordBase& v) { return *this; }
    // Private member functions.
    inline int Smaller(int input)      const { return m_Length < input ? m_Length : input; }
    inline int Larger(int input)       const { return m_Length < input ? input : m_Length ; }
    inline int Smaller(int a1, int a2) const { return a1 < a2 ? a1 : a2; }
    inline int Larger(int a1, int a2)  const { return a1 > a2 ? a1 : a2; }
    inline double Absolute(double v)   const { return (v < 0) ? -v : v; }

    // Private class variables.
    static const int g_Alignment = 32;

    // Private member variables.
    double*  m_AddressAligned;
    int      m_Length;
};

} // namespace mcon {
