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

#include <new>
#include <stdint.h>
#include <sys/types.h>

#include "debug.h"
#include "VectordBase.h"

namespace mcon {

// ‚±‚ñ‚È‚ñ‚µ‚½‚¢
// typedef Matrixd(1, n) Vectord(n);

/*--------------------------------------------------------------------
 * Matrixd
 *--------------------------------------------------------------------*/

class Matrixd
{
public:
    Matrixd(int rowLength = 0, int columnLength = 0);
    Matrixd(const Matrixd& m);
    template <typename U>
    Matrixd(const Matrix<U>& m);

    ~Matrixd();

    const VectordBase& operator[](int i) const
    {
        ASSERT(0 <= i && i < m_RowLength);
        return *(reinterpret_cast<VectordBase*>(m_Address) + i);
    }

    VectordBase& operator[](int i)
    {
        ASSERT(0 <= i && i < m_RowLength);
        return *(reinterpret_cast<VectordBase*>(m_Address) + i);
    }

    Matrixd& operator=(const Matrixd& m);

    const Matrixd operator+(double v) const;
    const Matrixd operator-(double v) const;
    const Matrixd operator*(double v) const;
    const Matrixd operator/(double v) const;

    const Matrixd operator+(const Matrixd& m) const;
    const Matrixd operator-(const Matrixd& m) const;
    const Matrixd operator*(const Matrixd& m) const;
    const Matrixd operator/(const Matrixd& m) const;

    Matrixd& operator+=(double v);
    Matrixd& operator-=(double v);
    Matrixd& operator*=(double v);
    Matrixd& operator/=(double v);

    Matrixd& operator+=(const Matrixd& m);
    Matrixd& operator-=(const Matrixd& m);
    Matrixd& operator*=(const Matrixd& m);
    Matrixd& operator/=(const Matrixd& m);

    inline Matrixd T(void) const { return Transpose(); }
    inline Matrixd I(void) const { return Inverse(); }
    double D(void) const { return Determinant(); }
    inline static Matrixd E(int size) { return Identify(size); }

    Matrixd Transpose(void) const;
    Matrixd Multiply(const Matrixd& m) const;
    Matrixd Inverse(void) const;
    double Determinant(void) const;
    Matrixd GetCofactorMatrix(int row, int col) const;
    double GetCofactor(int row, int col) const;

    static Matrixd Identify(int size);
    bool IsNull(void) const { return m_RowLength == 0; }
    int GetRowLength(void) const { return m_RowLength; }
    int GetColumnLength(void) const { return m_ColumnLength; }
    bool Resize(int, int);
private:
    // Member functions (private).
    void Allocate      (void);
    int Smaller(int length) const { return (length > m_RowLength) ? m_RowLength : length; };

    // Member variables (private).
    int m_RowLength;
    int m_ColumnLength;
    uint8_t* m_Address;
};

template <typename U>
Matrixd::Matrixd(const Matrix<U>& m)
    : m_RowLength(m.GetRowLength())
    , m_ColumnLength(m.GetColumnLength())
    , m_Address(NULL)
{
    Allocate();
    VectordBase* ptr = reinterpret_cast<VectordBase*>(m_Address);
    for (int i = 0; i < GetRowLength(); ++i, ++ptr)
    {
        *ptr = m[i];
    }
}

} // namespace mcon {
