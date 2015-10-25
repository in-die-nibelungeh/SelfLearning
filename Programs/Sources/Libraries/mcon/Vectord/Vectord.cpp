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

namespace mcon {

class Matrixd;

#define MCON_ITERATION(var, iter, statement)  \
    do {                                      \
        for (int var = 0; var < iter; ++var)  \
        {                                     \
            statement;                        \
        }                                     \
    } while(0)

template <typename Type>
class Vectord
{
public:

    explicit Vectord(const int length = 0);
    Vectord(const Vectord<Type>& v);
    template <typename U> Vectord(const Vectord<U>& v);
    ~Vectord();

    // For const object
    const Type& operator[](const int i) const
    {
        ASSERT (0 <= i && i < m_Length);
        return m_Address[i];
    }
    // For non-const object
    Type& operator[](const int i)
    {
        ASSERT (0 <= i && i < m_Length);
        return m_Address[i];
    }

    // Copy is to copy available data from src to dest without resizing the dest.
    const Vectord<Type>& Copy(const Vectord<Type>& Vectord);
    // operator= make the same Vectord as the input Vectord.
    Vectord<Type>& operator=(const Vectord<Type>& Vectord);

    template <typename U>
    operator Vectord<U>() const
    {
        Vectord<U> v(GetLength());
        MCON_ITERATION(i, m_Length, v[i] = static_cast<U>((*this)[i]));
        return v;
    }
    // This cast doesn't seem called... Why?
    // Are default ones already defined and called?
    operator Vectord<Type>() const
    {
        ASSERT(0);
        Vectord<Type> v(GetLength());
        MCON_ITERATION(i, m_Length, v[i] = (*this)[i]);
        return v;
    }

    operator void*() const
    {
        return reinterpret_cast<void*>(m_Address);
    }

    Vectord<Type> operator()(int offset, int length) const
    {
        Vectord<Type> carveout;
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

    Type Fifo(Type v)
    {
        Type ret = (*this)[0];
        for (int i = 0; i < GetLength() - 1; ++i)
        {
            (*this)[i] = (*this)[i+1];
        }
        (*this)[GetLength()-1] = v;
        return ret;
    }

    Type Unshift(Type v)
    {
        Type ret = (*this)[GetLength()-1];
        for (int i = GetLength() - 1; i > 0; --i)
        {
            (*this)[i] = (*this)[i-1];
        }
        (*this)[0] = v;
        return ret;
    }

    Vectord<Type>& operator=(Type v) { MCON_ITERATION(i, m_Length, (*this)[i] = v); return *this; };

    const Vectord<Type> operator+(Type v) const { Vectord<Type> vec(*this);  vec += v; return vec; }
    const Vectord<Type> operator-(Type v) const { Vectord<Type> vec(*this);  vec -= v; return vec; }
    const Vectord<Type> operator*(Type v) const { Vectord<Type> vec(*this);  vec *= v; return vec; }
    const Vectord<Type> operator/(Type v) const { Vectord<Type> vec(*this);  vec /= v; return vec; }

    const Vectord<Type> operator+(const Vectord<Type>& v) const { Vectord<Type> vec(*this);  vec += v; return vec; }
    const Vectord<Type> operator-(const Vectord<Type>& v) const { Vectord<Type> vec(*this);  vec -= v; return vec; }
    const Vectord<Type> operator*(const Vectord<Type>& v) const { Vectord<Type> vec(*this);  vec *= v; return vec; }
    const Vectord<Type> operator/(const Vectord<Type>& v) const { Vectord<Type> vec(*this);  vec /= v; return vec; }

    Vectord<Type>& operator+=(Type v) { MCON_ITERATION(i, m_Length, (*this)[i] += v); return *this; }
    Vectord<Type>& operator-=(Type v) { MCON_ITERATION(i, m_Length, (*this)[i] -= v); return *this; }
    Vectord<Type>& operator*=(Type v) { MCON_ITERATION(i, m_Length, (*this)[i] *= v); return *this; }
    Vectord<Type>& operator/=(Type v) { MCON_ITERATION(i, m_Length, (*this)[i] /= v); return *this; }

    Vectord<Type>& operator+=(const Vectord<Type>& v) { MCON_ITERATION(i, Smaller(v.GetLength()), (*this)[i] += v[i]); return *this; }
    Vectord<Type>& operator-=(const Vectord<Type>& v) { MCON_ITERATION(i, Smaller(v.GetLength()), (*this)[i] -= v[i]); return *this; }
    Vectord<Type>& operator*=(const Vectord<Type>& v) { MCON_ITERATION(i, Smaller(v.GetLength()), (*this)[i] *= v[i]); return *this; }
    Vectord<Type>& operator/=(const Vectord<Type>& v) { MCON_ITERATION(i, Smaller(v.GetLength()), (*this)[i] /= v[i]); return *this; }

    inline Matrix<Type> T(void) const { return Transpose(); }
    Matrix<Type> Transpose(void) const
    {
        Matrix<Type> m(GetLength(), 1);
        MCON_ITERATION(i, GetLength(), m[i][0] = (*this)[i]);
        return m;
    }
    Matrix<Type> ToMatrix(void) const
    {
        Matrix<Type> m(1, GetLength());
        m[0] = *this;
        return m;
    }

    inline Type GetMaximum(void) const
    {
        Type max = (*this)[0];
        for (int i = 1; i < GetLength(); ++i)
        {
            if (max < (*this)[i])
            {
                max = (*this)[i];
            }
        }
        return max;
    }

    inline Type GetMaximumAbsolute(void) const
    {
        Type max = Absolute((*this)[0]);
        for (int i = 1; i < GetLength(); ++i)
        {
            const Type v = Absolute((*this)[i]);
            if (max < v)
            {
                max = v;
            }
        }
        return max;
    }

    inline Type GetMinimum(void) const
    {
        Type min = (*this)[0];
        for (int i = 1; i < GetLength(); ++i)
        {
            if (min > (*this)[i])
            {
                min = (*this)[i];
            }
        }
        return min;
    }

    inline Type GetMinimumAbsolute(void) const
    {
        Type min = Absolute((*this)[0]);
        for (int i = 1; i < GetLength(); ++i)
        {
            const Type v = Absolute((*this)[i]);
            if (min > v)
            {
                min = v;
            }
        }
        return min;
    }


    inline Type GetSum(void) const
    {
        Type sum = 0;
        MCON_ITERATION( i, GetLength(), sum += (*this)[i] );
        return sum;
    }

    inline double GetAverage(void) const
    {
        return GetSum()/GetLength();
    }

    inline double GetNorm(void) const
    {
        double squareSum = 0;
        MCON_ITERATION( i, GetLength(), squareSum += (*this)[i] * (*this)[i]);
        return sqrt(squareSum);
    }

    int GetLength(void) const { return m_Length; }
    bool IsNull(void) const { return m_Length == 0; }
    bool Resize(int length);

private:
    // Private member functions.
    int    Smaller(int a1, int a2) const { return a1 < a2 ? a1 : a2; }
    int    Smaller(int input) const { return m_Length < input ? m_Length : input; }
    int    Larger(int a1, int a2) const { return a1 > a2 ? a1 : a2; }
    int    Larger(int input) const { return m_Length < input ? input : m_Length ; }
    void   Allocate(void);
    Type   Absolute(Type v) const { return (v < 0) ? -v : v; }
    // Private member variables.
    Type*  m_Address;
    int    m_Length;
};

template <typename Type>
void Vectord<Type>::Allocate(void)
{
    m_Address = PTR_CAST(Type*, NULL);
    if (m_Length > 0)
    {
        m_Address = new Type[m_Length];
        ASSERT(NULL != m_Address);
    }
}

template <typename Type>
Vectord<Type>::Vectord(int length)
    : m_Address(NULL),
    m_Length(length)
{
    Allocate();
}

template <typename Type>
Vectord<Type>::Vectord(const Vectord<Type>& v)
    : m_Address(PTR_CAST(Type*, NULL)),
    m_Length(v.GetLength())
{
    Allocate();
    MCON_ITERATION(i, m_Length, (*this)[i] = v[i]);
}

template <typename Type>
template <typename U>
Vectord<Type>::Vectord(const Vectord<U>& v)
    : m_Address(PTR_CAST(Type*, NULL)),
    m_Length(v.GetLength())
{
    Allocate();
    MCON_ITERATION(i, m_Length, (*this)[i] = static_cast<Type>(v[i]));
}

template <typename Type>
Vectord<Type>::~Vectord()
{
    if (NULL != m_Address)
    {
        delete[] m_Address;
        m_Address = PTR_CAST(Type*, NULL);
    }
    m_Length = 0;
}

template <typename Type>
const Vectord<Type>& Vectord<Type>::Copy(const Vectord<Type>& v)
{
    MCON_ITERATION(i, Smaller(v.GetLength()), (*this)[i] = v[i]);
    return *this;
}

template <typename Type>
Vectord<Type>& Vectord<Type>::operator=(const Vectord<Type>& v)
{
    // m_Length is updated in Resize().
    Resize(v.GetLength());
    MCON_ITERATION(i, v.GetLength(), (*this)[i] = v[i]);
    return *this;
}

template <typename Type>
bool Vectord<Type>::Resize(int length)
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
    m_Address = new Type[length];
    ASSERT (NULL != m_Address);
    return true;
}

} // namespace mcon {
