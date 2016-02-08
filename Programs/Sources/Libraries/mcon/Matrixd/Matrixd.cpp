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
        while (reinterpret_cast<uintptr_t>(aligned) % align) { ++aligned; }
        return const_cast<double*>(reinterpret_cast<const double*>(aligned));
    }
} // anonymous

namespace mcon {

// こんなんしたい
// typedef Matrix<double>(1, n) Vectord(n);

Matrix<double>& Matrix<double>::operator=(double v) { MCON_ITERATION( i, GetRowLength(), (*this)[i] = v); return *this; }

const Matrix<double> Matrix<double>::operator+(double v) const { Matrix<double> mat(*this); MCON_ITERATION( i, GetRowLength(), mat[i] += v); return mat; }
const Matrix<double> Matrix<double>::operator-(double v) const { Matrix<double> mat(*this); MCON_ITERATION( i, GetRowLength(), mat[i] -= v); return mat; }
const Matrix<double> Matrix<double>::operator*(double v) const { Matrix<double> mat(*this); MCON_ITERATION( i, GetRowLength(), mat[i] *= v); return mat; }
const Matrix<double> Matrix<double>::operator/(double v) const { Matrix<double> mat(*this); MCON_ITERATION( i, GetRowLength(), mat[i] /= v); return mat; }

const Matrix<double> Matrix<double>::operator+(const Matrix<double>& m) const { Matrix<double> mat(*this); mat += m; return mat; }
const Matrix<double> Matrix<double>::operator-(const Matrix<double>& m) const { Matrix<double> mat(*this); mat -= m; return mat; }
const Matrix<double> Matrix<double>::operator*(const Matrix<double>& m) const { Matrix<double> mat(*this); mat *= m; return mat; }
const Matrix<double> Matrix<double>::operator/(const Matrix<double>& m) const { Matrix<double> mat(*this); mat /= m; return mat; }

Matrix<double>& Matrix<double>::operator+=(double v) { MCON_ITERATION( i, GetRowLength(), (*this)[i] += v); return *this; }
Matrix<double>& Matrix<double>::operator-=(double v) { MCON_ITERATION( i, GetRowLength(), (*this)[i] -= v); return *this; }
Matrix<double>& Matrix<double>::operator*=(double v) { MCON_ITERATION( i, GetRowLength(), (*this)[i] *= v); return *this; }
Matrix<double>& Matrix<double>::operator/=(double v) { MCON_ITERATION( i, GetRowLength(), (*this)[i] /= v); return *this; }

Matrix<double>& Matrix<double>::operator+=(const Matrix<double>& m) { MCON_ITERATION( i, Smaller(m.GetRowLength()), (*this)[i] += m[i]); return *this; }
Matrix<double>& Matrix<double>::operator-=(const Matrix<double>& m) { MCON_ITERATION( i, Smaller(m.GetRowLength()), (*this)[i] -= m[i]); return *this; }
Matrix<double>& Matrix<double>::operator*=(const Matrix<double>& m) { MCON_ITERATION( i, Smaller(m.GetRowLength()), (*this)[i] *= m[i]); return *this; }
Matrix<double>& Matrix<double>::operator/=(const Matrix<double>& m) { MCON_ITERATION( i, Smaller(m.GetRowLength()), (*this)[i] /= m[i]); return *this; }

Matrix<double> Matrix<double>::Identify(size_t size)
{
    Matrix<double> I(size, size);
    for (size_t i = 0; i < size; ++i)
    {
        I[i] = 0;
    }
    for (size_t i = 0; i < size; ++i)
    {
        I[i][i] = 1;
    }
    return I;
}

bool Matrix<double>::Allocate(void)
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
    for (size_t k = 0; k < m_RowLength; ++k)
    {
        new (m_ObjectBase + k) VectordBase(
            bufferBase + lengthAligned * k, m_ColumnLength);
    }
    return true;
}

Matrix<double>::~Matrix<double>()
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

void Matrix<double>::Initialize( double (*initializer)(size_t, size_t, size_t, size_t) )
{
    for (size_t r = 0; r < GetRowLength(); ++r)
    {
        for (size_t c = 0; c < GetColumnLength(); ++c)
        {
            (*this)[r][c] = initializer(r, GetRowLength(), c, GetColumnLength());
        }
    }
}

bool Matrix<double>::Resize(size_t rowLength, size_t columnLength)
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

Matrix<double>& Matrix<double>::operator=(const Matrix<double>& m)
{
    Resize(m.GetRowLength(), m.GetColumnLength());
    VectordBase* ptr = m_ObjectBase;
    for (size_t i = 0; i < GetRowLength(); ++i, ++ptr )
    {
        *ptr = m[i];
    }
    return *this;
}


Matrix<double> Matrix<double>::Transpose(void) const
{
    Matrix<double> transposed(GetColumnLength(), GetRowLength());
    for (size_t i = 0; i < GetRowLength(); ++i)
    {
        for (size_t j = 0; j < GetColumnLength(); ++j)
        {
            transposed[j][i] = (*this)[i][j];
        }
    }
    return transposed;
}


Matrix<double> Matrix<double>::Multiply(const Matrix<double>& m) const
{
    if ( GetColumnLength() != m.GetRowLength() )
    {
        Matrix<double> null;
        return null;
    }
    Matrix<double> multiplied(GetRowLength(), m.GetColumnLength());
#if 0
    for (size_t row = 0; row < multiplied.GetRowLength(); ++row)
    {
        for (size_t col = 0; col < multiplied.GetColumnLength(); ++col)
        {
            double v = 0;
            for (size_t k = 0; k < GetColumnLength(); ++k)
            {
                v += (*this)[row][k] * m[k][col];
            }
            multiplied[row][col] = v;
        }
    }
#else
    Matrix<double> transposed(m.T());
    for (size_t row = 0; row < multiplied.GetRowLength(); ++row)
    {
        for (size_t col = 0; col < multiplied.GetColumnLength(); ++col)
        {
            multiplied[row][col] = (*this)[row].Dot(transposed[col]);
        }
    }
#endif
    return multiplied;
}


Matrix<double> Matrix<double>::GetCofactorMatrix(size_t row, size_t col) const
{
    const size_t rowCount = GetRowLength();
    const size_t colCount = GetColumnLength();
    Matrix<double> cofactorMatrix(rowCount-1, colCount-1);

    for (size_t r = 0, ri = 0; ri < rowCount; ++ri)
    {
        if (ri == row)
        {
            continue;
        }
        for (size_t c = 0, ci = 0; ci < colCount; ++ci)
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


double Matrix<double>::GetCofactor(size_t row, size_t col) const
{
    int sign = ( (row + col) & 1) ? -1 : 1;
    return GetCofactorMatrix(row, col).Determinant() * sign;
}


double Matrix<double>::Determinant(void) const
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
        const Matrix<double>&m = *this;
        det = m[0][0] * m[1][1] * m[2][2]
              + m[0][1] * m[1][2] * m[2][0]
              + m[0][2] * m[1][0] * m[2][1]
              - m[0][2] * m[1][1] * m[2][0]
              - m[0][1] * m[1][0] * m[2][2]
              - m[0][0] * m[1][2] * m[2][1];
    }
    else if ( 2 == dimension )
    {
        const Matrix<double>&m = *this;
        det = m[0][0] * m[1][1]
              - m[0][1] * m[1][0];
    }
    else if ( 1 == dimension )
    {
        const Matrix<double>&m = *this;
        det = m[0][0];
    }
    else
    {
        for (size_t row = 0; row < GetRowLength(); ++row)
        {
            det += GetCofactor(row, 0) * (*this)[row][0];
        }
    }
    return det;
}


Matrix<double> Matrix<double>::Inverse(void) const
{
    if ( GetColumnLength() != GetRowLength() )
    {
        return *this;
    }
    // Calculate Inversed-Matrix<double> by Cofactors.
    const size_t rowCount = GetRowLength();
    const size_t colCount = GetColumnLength();
    Matrix<double> inversed(rowCount, colCount);

#if 0
    double det = Determinant();
    if ( 0 == det )
    {
        return *this;
    }
    for (size_t row = 0; row < rowCount; ++row)
    {
        for (size_t col = 0; col < colCount; ++col)
        {
            int sign = ((row + col) & 1) ? -1 : 1;
            Matrix<double> m(GetCofactorMatrix(row, col));
            inversed[col][row] = GetCofactor(row, col) / det;
        }
    }
#else
    //--------------------------------
    // Gauss-Jordan
    //--------------------------------
    const double threshold = 1.0e-10; // TBD

    Matrix<double> m(rowCount, colCount * 2);
    if ( m.IsNull() )
    {
        return m;
    }
    for (size_t row = 0; row < rowCount; ++row)
    {
        for (size_t col = 0; col < colCount; ++col)
        {
            m[row][col] = (*this)[row][col];
        }
    }
    for (size_t row = 0; row < rowCount; ++row)
    {
        for (size_t col = colCount; col < colCount * 2; ++col)
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
    for (size_t r = 0; r < m.GetRowLength(); ++r )
    {
        m[r] /= m[r].GetMaximumAbsolute();
    }
    for (size_t r = 0; r < m.GetRowLength(); ++r )
    {
        // 注目している列の中で、最大値を持つ列を探す。
        size_t index = r;
        double maximum = fabs(m[r][r]);
        // "k < m.GetRowLength() - 1" となっていたが、意図不明なので修正。
        for (size_t k = index + 1; k < m.GetRowLength(); ++k )
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
            DEBUG_LOG(" ~ 0 at r=%lld\n", static_cast<int64_t>(r));
            m.Resize(0, 0); // NULL を返す。
            return m;
        }
        // 必要なら入れ替える
        if ( r != index )
        {
            const Vectord tmp(m[r]);
            m[r] = m[index];
            m[index] = tmp;
        }
        // 上で入れ替えたので 以降では index は不要、r を使用する。
        // 注目している行の、注目している列の値を 1.0 にする
        m[r] /= m[r][r]; // /= maximum; // maximum では符号が考慮されない。

        // 他の行から引く。
        for (size_t k = 0; k < m.GetRowLength(); ++k )
        {
            if ( k == r )
            {
                continue;
            }
            const Vectord tmp(m[r]);
            m[k] -= (tmp * m[k][r]);
        }
    }
    for (size_t row = 0; row < rowCount; ++row)
    {
        for (size_t col = 0; col < colCount; ++col)
        {
            inversed[row][col] = m[row][col+colCount];
        }
    }
#endif
    return inversed;
}

const Matrix<double> Matrix<double>::SubMatrix(
    size_t rowBegin,
    size_t rowEnd,
    size_t columnBegin,
    size_t columnEnd
) const
{
    Matrix<double> m;
    if (rowEnd < rowBegin || columnEnd < columnBegin)
    {
        return m;
    }
    const size_t rowLength = rowEnd - rowBegin + 1;
    const size_t columnLength = columnEnd - columnBegin + 1;
    const size_t copyRowLength = std::min(rowLength, GetRowLength() - rowBegin);
    const size_t copyColumnLength = std::min(columnLength, GetColumnLength() - columnBegin);
    if (false == m.Resize(copyRowLength, copyColumnLength))
    {
        return m;
    }
    for (size_t r = 0; r < copyRowLength; ++r)
    {
        const double* ptr = (*this)[r + rowBegin];
        std::memcpy(m[r], ptr + columnBegin, copyColumnLength * sizeof(double));
    }
    return m;
}

} // namespace mcon {
