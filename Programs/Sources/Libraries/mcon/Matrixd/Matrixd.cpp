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


#include <new>
#include <stdint.h>
#include <sys/types.h>

#include "debug.h"
#include "mcon.h"

namespace {

    double* Align(void* ptr, int align)
    {
        const uint8_t* aligned = reinterpret_cast<const uint8_t*>(ptr);
        while (reinterpret_cast<long unsigned int>(aligned) % align) { ++aligned; }
        return const_cast<double*>(reinterpret_cast<const double*>(aligned));
    }
} // anonymous

namespace mcon {

// こんなんしたい
// typedef Matrixd(1, n) Vectord(n);

Matrixd& Matrixd::operator=(double v) { MCON_ITERATION( i, GetRowLength(), (*this)[i] = v); return *this; }

const Matrixd Matrixd::operator+(double v) const { Matrixd mat(*this); MCON_ITERATION( i, GetRowLength(), mat[i] += v); return mat; }
const Matrixd Matrixd::operator-(double v) const { Matrixd mat(*this); MCON_ITERATION( i, GetRowLength(), mat[i] -= v); return mat; }
const Matrixd Matrixd::operator*(double v) const { Matrixd mat(*this); MCON_ITERATION( i, GetRowLength(), mat[i] *= v); return mat; }
const Matrixd Matrixd::operator/(double v) const { Matrixd mat(*this); MCON_ITERATION( i, GetRowLength(), mat[i] /= v); return mat; }

const Matrixd Matrixd::operator+(const Matrixd& m) const { Matrixd mat(*this); mat += m; return mat; }
const Matrixd Matrixd::operator-(const Matrixd& m) const { Matrixd mat(*this); mat -= m; return mat; }
const Matrixd Matrixd::operator*(const Matrixd& m) const { Matrixd mat(*this); mat *= m; return mat; }
const Matrixd Matrixd::operator/(const Matrixd& m) const { Matrixd mat(*this); mat /= m; return mat; }

Matrixd& Matrixd::operator+=(double v) { MCON_ITERATION( i, GetRowLength(), (*this)[i] += v); return *this; }
Matrixd& Matrixd::operator-=(double v) { MCON_ITERATION( i, GetRowLength(), (*this)[i] -= v); return *this; }
Matrixd& Matrixd::operator*=(double v) { MCON_ITERATION( i, GetRowLength(), (*this)[i] *= v); return *this; }
Matrixd& Matrixd::operator/=(double v) { MCON_ITERATION( i, GetRowLength(), (*this)[i] /= v); return *this; }

Matrixd& Matrixd::operator+=(const Matrixd& m) { MCON_ITERATION( i, Smaller(m.GetRowLength()), (*this)[i] += m[i]); return *this; }
Matrixd& Matrixd::operator-=(const Matrixd& m) { MCON_ITERATION( i, Smaller(m.GetRowLength()), (*this)[i] -= m[i]); return *this; }
Matrixd& Matrixd::operator*=(const Matrixd& m) { MCON_ITERATION( i, Smaller(m.GetRowLength()), (*this)[i] *= m[i]); return *this; }
Matrixd& Matrixd::operator/=(const Matrixd& m) { MCON_ITERATION( i, Smaller(m.GetRowLength()), (*this)[i] /= m[i]); return *this; }

Matrixd Matrixd::Identify(uint size)
{
    Matrixd I(size, size);
    for (uint i = 0; i < size; ++i)
    {
        I[i] = 0;
    }
    for (uint i = 0; i < size; ++i)
    {
        I[i][i] = 1;
    }
    return I;
}

bool Matrixd::Allocate(void)
{
    const int align = g_Alignment;
    const unsigned int unit = align / sizeof(double);

    if ( m_RowLength == 0 )
    {
        return true;
    }
    const int lengthAligned = ((m_ColumnLength + (unit - 1)) / unit) * unit;
    int size = m_RowLength * sizeof(VectordBase)
                   + (align - 1)
                   + lengthAligned * m_RowLength * sizeof(double);
    m_Address = reinterpret_cast<void*>(new uint8_t[size]);
    if (m_Address == NULL)
    {
        return false;
    }
    m_ObjectBase = reinterpret_cast<VectordBase*>(m_Address);
    double* bufferBase = Align(m_ObjectBase + m_RowLength, align);
    for (uint k = 0; k < m_RowLength; ++k)
    {
        new (m_ObjectBase + k) VectordBase(
            bufferBase + lengthAligned * k, m_ColumnLength);
    }
    return true;
}

Matrixd::~Matrixd()
{
    if (m_Address != NULL)
    {
        delete[] reinterpret_cast<uint8_t*>(m_Address);
        m_Address = NULL;
        m_ObjectBase = NULL;
    }
    m_RowLength = 0;
    m_ColumnLength = 0;
}

bool Matrixd::Resize(uint rowLength, uint columnLength)
{
    if (rowLength < 0 || columnLength < 0)
    {
        return false;
    }
    if (m_Address != NULL)
    {
        delete[] reinterpret_cast<uint8_t*>(m_Address);
        m_Address = NULL;
        m_ObjectBase = NULL;
    }
    m_RowLength = rowLength;
    m_ColumnLength = columnLength;
    return Allocate();
}

Matrixd& Matrixd::operator=(const Matrixd& m)
{
    Resize(m.GetRowLength(), m.GetColumnLength());
    VectordBase* ptr = m_ObjectBase;
    for (uint i = 0; i < GetRowLength(); ++i, ++ptr )
    {
        *ptr = m[i];
    }
    return *this;
}


Matrixd Matrixd::Transpose(void) const
{
    Matrixd transposed(GetColumnLength(), GetRowLength());
    for (uint i = 0; i < GetRowLength(); ++i)
    {
        for (uint j = 0; j < GetColumnLength(); ++j)
        {
            transposed[j][i] = (*this)[i][j];
        }
    }
    return transposed;
}


Matrixd Matrixd::Multiply(const Matrixd& m) const
{
    if ( GetColumnLength() != m.GetRowLength() )
    {
        Matrixd null;
        return null;
    }
    Matrixd multiplied(GetRowLength(), m.GetColumnLength());
#if 0
    for (uint row = 0; row < multiplied.GetRowLength(); ++row)
    {
        for (uint col = 0; col < multiplied.GetColumnLength(); ++col)
        {
            double v = 0;
            for (uint k = 0; k < GetColumnLength(); ++k)
            {
                v += (*this)[row][k] * m[k][col];
            }
            multiplied[row][col] = v;
        }
    }
#else
    Matrixd transposed(m.T());
    for (uint row = 0; row < multiplied.GetRowLength(); ++row)
    {
        for (uint col = 0; col < multiplied.GetColumnLength(); ++col)
        {
            multiplied[row][col] = (*this)[row].Dot(transposed[col]);
        }
    }
#endif
    return multiplied;
}


Matrixd Matrixd::GetCofactorMatrix(uint row, uint col) const
{
    const uint rowCount = GetRowLength();
    const uint colCount = GetColumnLength();
    Matrixd cofactorMatrix(rowCount-1, colCount-1);

    for (uint r = 0, ri = 0; ri < rowCount; ++ri)
    {
        if (ri == row)
        {
            continue;
        }
        for (uint c = 0, ci = 0; ci < colCount; ++ci)
        {
            if (ci != col)
            {
                cofactorMatrix[r][c] = (*this)[ri][ci];
                ++c;
            }
        }
        ++r;
    }
    return cofactorMatrix;
}


double Matrixd::GetCofactor(uint row, uint col) const
{
    int sign = ( (row + col) & 1) ? -1 : 1;
    return GetCofactorMatrix(row, col).Determinant() * sign;
}


double Matrixd::Determinant(void) const
{
    if ( GetColumnLength() != GetRowLength() )
    {
        return 0;
    }
    int dimension = GetColumnLength();
    double det = 0;

    // Sarrus
    if ( 3 == dimension )
    {
        const Matrixd&m = *this;
        det = m[0][0] * m[1][1] * m[2][2]
              + m[0][1] * m[1][2] * m[2][0]
              + m[0][2] * m[1][0] * m[2][1]
              - m[0][2] * m[1][1] * m[2][0]
              - m[0][1] * m[1][0] * m[2][2]
              - m[0][0] * m[1][2] * m[2][1];
    }
    else if ( 2 == dimension )
    {
        const Matrixd&m = *this;
        det = m[0][0] * m[1][1]
              - m[0][1] * m[1][0];
    }
    else if ( 1 == dimension )
    {
        const Matrixd&m = *this;
        det = m[0][0];
    }
    else
    {
        for (uint row = 0; row < GetRowLength(); ++row)
        {
            det += GetCofactor(row, 0) * (*this)[row][0];
        }
    }
    return det;
}


Matrixd Matrixd::Inverse(void) const
{
    if ( GetColumnLength() != GetRowLength() )
    {
        return *this;
    }
    // Calculate Inversed-Matrixd by Cofactors.
    const uint rowCount = GetRowLength();
    const uint colCount = GetColumnLength();
    Matrixd inversed(rowCount, colCount);

#if 0
    double det = Determinant();
    if ( 0 == det )
    {
        return *this;
    }
    for (uint row = 0; row < rowCount; ++row)
    {
        for (uint col = 0; col < colCount; ++col)
        {
            int sign = ((row + col) & 1) ? -1 : 1;
            Matrixd m(GetCofactorMatrix(row, col));
            inversed[col][row] = GetCofactor(row, col) / det;
        }
    }
#else
    //--------------------------------
    // Gauss-Jordan
    //--------------------------------
    const double threshold = 1.0e-10; // TBD

    Matrixd m(rowCount, colCount * 2);
    if ( m.IsNull() )
    {
        return m;
    }
    for (uint row = 0; row < rowCount; ++row)
    {
        for (uint col = 0; col < colCount; ++col)
        {
            m[row][col] = (*this)[row][col];
        }
    }
    for (uint row = 0; row < rowCount; ++row)
    {
        for (uint col = colCount; col < colCount * 2; ++col)
        {
            if (row == (col - colCount))
            {
                m[row][col] = 1.0;
            }
            else
            {
                m[row][col] = 0.0;
            }
        }
    }

    // 各行を正規化
    for (uint r = 0; r < m.GetRowLength(); ++r )
    {
        m[r] /= m[r].GetMaximumAbsolute();
    }
    for (uint r = 0; r < m.GetRowLength(); ++r )
    {
        // 注目している列の中で、最大値を持つ列を探す。
        uint index = r;
        double maximum = fabs(m[r][r]);
        // "k < m.GetRowLength() - 1" となっていたが、意図不明なので修正。
        for (uint k = index + 1; k < m.GetRowLength(); ++k )
        {
            if ( fabs(m[k][r]) > maximum )
            {
                index = k;
                maximum = fabs(m[k][r]);
            }
        }
        if ( maximum < threshold )
        {
            // ここに到達することはないはず。
            DEBUG_LOG(" ~ 0 at r=%d\n", r);
            m.Resize(0, 0); // NULL を返す。
            return m;
        }
        // 必要なら入れ替える
        if ( r != index )
        {
            mcon::Vectord tmp(m[r]);
            m[r] = m[index];
            m[index] = tmp;
        }
        // 上で入れ替えたので 以降では index は不要、r を使用する。
        // 注目している行の、注目している列の値を 1.0 にする
        m[r] /= m[r][r]; // /= maximum; // maximum では符号が考慮されない。

        // 他の行から引く。
        for (uint k = 0; k < m.GetRowLength(); ++k )
        {
            if ( k == r )
            {
                continue;
            }
            const Vectord tmp(m[r]);
            m[k] -= (tmp * m[k][r]);
        }
    }
    for (uint row = 0; row < rowCount; ++row)
    {
        for (uint col = 0; col < colCount; ++col)
        {
            inversed[row][col] = m[row][col+colCount];
        }
    }
#endif
    return inversed;
}

} // namespace mcon {
