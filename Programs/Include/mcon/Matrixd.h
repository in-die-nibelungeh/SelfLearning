/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2015-2016 Ryosuke Kanata
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

#include "debug.h"
#include "VectordBase.h"

namespace mcon {

// こんなんしたい
// typedef Matrixd(1, n) Vectord(n);

/*--------------------------------------------------------------------
 * Matrixd
 *--------------------------------------------------------------------*/

class Matrixd
{
public:
    Matrixd(uint rowLength = 0, uint columnLength = 0)
        : m_RowLength(rowLength)
        , m_ColumnLength(columnLength)
        , m_Address(NULL)
        , m_ObjectBase(NULL)
    {
        ASSERT( (rowLength == 0 && columnLength == 0) || (rowLength != 0 && columnLength != 0));
        bool status = Allocate();
        UNUSED(status);
        ASSERT(true == status);
    }

    Matrixd(const VectordBase& v, bool transpose = false)
        : m_RowLength(transpose ? v.GetLength() : 1)
        , m_ColumnLength(transpose ? 1 : v.GetLength())
        , m_Address(NULL)
        , m_ObjectBase(NULL)
    {
        bool status = Allocate();
        UNUSED(status);
        ASSERT(true == status);
        if (true == transpose)
        {
            for (uint i = 0; i < v.GetLength(); ++i)
            {
                m_ObjectBase[i][0] = v[i];
            }
        }
        else
        {
            m_ObjectBase[0] = v;
        }
    }

    Matrixd(const Matrixd& m)
        : m_RowLength(m.GetRowLength())
        , m_ColumnLength(m.GetColumnLength())
        , m_Address(NULL)
        , m_ObjectBase(NULL)
    {
        bool status = Allocate();
        UNUSED(status);
        ASSERT(true == status);
        for (uint i = 0; i < GetRowLength(); ++i)
        {
            m_ObjectBase[i] = m[i];
        }
    }

    template <typename U>
    Matrixd(const Matrix<U>& m)
        : m_RowLength(m.GetRowLength())
        , m_ColumnLength(m.GetColumnLength())
        , m_Address(NULL)
        , m_ObjectBase(NULL)
    {
        bool status = Allocate();
        UNUSED(status);
        ASSERT(true == status);
        for (uint i = 0; i < GetRowLength(); ++i)
        {
            m_ObjectBase[i] = m[i];
        }
    }

    ~Matrixd();

    const VectordBase& operator[](uint i) const
    {
        ASSERT(i < m_RowLength);
        return *(m_ObjectBase + i);
    }

    VectordBase& operator[](uint i)
    {
        ASSERT(i < m_RowLength);
        return *(m_ObjectBase + i);
    }

    Matrixd& operator=(double v);

    const Matrixd operator+(double v) const;
    const Matrixd operator-(double v) const;
    const Matrixd operator*(double v) const;
    const Matrixd operator/(double v) const;

    Matrixd& operator+=(double v);
    Matrixd& operator-=(double v);
    Matrixd& operator*=(double v);
    Matrixd& operator/=(double v);

    Matrixd& operator=(const Matrixd& m);

    const Matrixd operator+(const Matrixd& m) const;
    const Matrixd operator-(const Matrixd& m) const;
    const Matrixd operator*(const Matrixd& m) const;
    const Matrixd operator/(const Matrixd& m) const;

    Matrixd& operator+=(const Matrixd& m);
    Matrixd& operator-=(const Matrixd& m);
    Matrixd& operator*=(const Matrixd& m);
    Matrixd& operator/=(const Matrixd& m);

    double Determinant(void) const;
    Matrixd Transpose(void) const;
    Matrixd Multiply(const Matrixd& m) const;
    Matrixd Inverse(void) const;
    Matrixd GetCofactorMatrix(uint row, uint col) const;
    double GetCofactor(uint row, uint col) const;

    bool Resize(uint, uint);

    static Matrixd Identify(uint size);

    // Inline functions.
    inline bool IsNull(void) const { return m_Address == NULL; }
    inline uint GetRowLength(void) const { return m_RowLength; }
    inline uint GetColumnLength(void) const { return m_ColumnLength; }

    // Aliases
    inline Matrixd T(void) const { return Transpose(); }
    inline Matrixd I(void) const { return Inverse(); }
    inline double  D(void) const { return Determinant(); }
    inline static Matrixd E(uint size) { return Identify(size); }

private:
    // Member functions (private).
    bool Allocate(void);
    uint Smaller(uint length) const { return (length > m_RowLength) ? m_RowLength : length; };

    static const uint g_Alignment = 32;
    // Member variables (private).
    uint m_RowLength;
    uint m_ColumnLength;
    void* m_Address;
    VectordBase* m_ObjectBase;
};

} // namespace mcon {
