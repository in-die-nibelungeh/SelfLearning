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
#include "Vector.h"
#include "Matrixd.h"

namespace mcon {

template <class Type> class Vector;

#define MCON_ITERATION(var, iter, statement)         \
    do {                                             \
        for (size_t var = 0; var < iter; ++var)      \
        {                                            \
            statement;                               \
        }                                            \
    } while(0)


/*--------------------------------------------------------------------
 * Matrix
 *--------------------------------------------------------------------*/
template <class Type>
class Matrix
{
public:
    void DumpMatrix(const Matrix<Type>&m, const char* fmt) const
    {
        for (size_t i = 0; i < m.GetRowLength(); ++i)
        {
            printf("| ");
            for (size_t j = 0; j < m.GetColumnLength(); ++j)
            {
                printf(fmt, m[i][j]);
                printf("\t");
            }
            printf(" |\n");
        }
    }
    Matrix(int rowLength = 0, int columnLength = 0);
    Matrix(const Matrix<Type>& m);
    template <typename U>
    Matrix(const Matrix<U>& m);
    Matrix(const Matrixd& m);

    ~Matrix();

    const Vector<Type>& operator[](size_t i) const
    {
        ASSERT(i < m_RowLength);
        return *m_Array[i];
    }

    Vector<Type>& operator[](size_t i)
    {
        ASSERT(i < m_RowLength);
        return *m_Array[i];
    }

    Matrix<Type>& operator=(const Matrix<Type>& m);
    Matrix<Type>& operator=(const Matrixd& m);
    Matrix<Type>& operator=(Type v) { MCON_ITERATION( i, GetRowLength(), (*this)[i] = v); return *this; }

    const Matrix<Type> operator+(Type v) const { Matrix<Type> mat(*this); MCON_ITERATION( i, GetRowLength(), mat[i] += v); return mat; }
    const Matrix<Type> operator-(Type v) const { Matrix<Type> mat(*this); MCON_ITERATION( i, GetRowLength(), mat[i] -= v); return mat; }
    const Matrix<Type> operator*(Type v) const { Matrix<Type> mat(*this); MCON_ITERATION( i, GetRowLength(), mat[i] *= v); return mat; }
    const Matrix<Type> operator/(Type v) const { Matrix<Type> mat(*this); MCON_ITERATION( i, GetRowLength(), mat[i] /= v); return mat; }

    const Matrix<Type> operator+(const Matrix<Type>& m) const { Matrix<Type> mat(*this); mat += m; return mat; }
    const Matrix<Type> operator-(const Matrix<Type>& m) const { Matrix<Type> mat(*this); mat -= m; return mat; }
    const Matrix<Type> operator*(const Matrix<Type>& m) const { Matrix<Type> mat(*this); mat *= m; return mat; }
    const Matrix<Type> operator/(const Matrix<Type>& m) const { Matrix<Type> mat(*this); mat /= m; return mat; }

    Matrix<Type>& operator+=(Type v) { MCON_ITERATION( i, GetRowLength(), (*this)[i] += v); return *this; }
    Matrix<Type>& operator-=(Type v) { MCON_ITERATION( i, GetRowLength(), (*this)[i] -= v); return *this; }
    Matrix<Type>& operator*=(Type v) { MCON_ITERATION( i, GetRowLength(), (*this)[i] *= v); return *this; }
    Matrix<Type>& operator/=(Type v) { MCON_ITERATION( i, GetRowLength(), (*this)[i] /= v); return *this; }

    Matrix<Type>& operator+=(const Matrix<Type>& m) { MCON_ITERATION( i, Smaller(m.GetRowLength()), (*this)[i] += m[i]); return *this; }
    Matrix<Type>& operator-=(const Matrix<Type>& m) { MCON_ITERATION( i, Smaller(m.GetRowLength()), (*this)[i] -= m[i]); return *this; }
    Matrix<Type>& operator*=(const Matrix<Type>& m) { MCON_ITERATION( i, Smaller(m.GetRowLength()), (*this)[i] *= m[i]); return *this; }
    Matrix<Type>& operator/=(const Matrix<Type>& m) { MCON_ITERATION( i, Smaller(m.GetRowLength()), (*this)[i] /= m[i]); return *this; }

    inline Matrix<Type> T(void) const { return Transpose(); }
    inline Matrix<Type> I(void) const { return Inverse(); }
    Type D(void) const { return Determinant(); }
    inline static Matrix<Type> E(int size) { return Identify(size); }

    Matrix<Type> Transpose(void) const;
    Matrix<Type> Multiply(const Matrix<Type>& m) const;
    Matrix<Type> Inverse(void) const;
    Type Determinant(void) const;
    Matrix<Type> GetCofactorMatrix(size_t row, size_t col) const;
    Type GetCofactor(size_t row, size_t col) const;

    static Matrix<Type> Identify(int size)
    {
        Matrix<Type> I(size, size);
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
/*
    void Initialize(int offset = 0, int step = 1)
    {
        for ( int r = 0; r < GetRowLength(); ++r )
        {
            (*this)[r].Initialize(offset, step);
        }
    }
*/
    void Initialize( Type (*initializer)(size_t, size_t, size_t, size_t) )
    {
        for (size_t r = 0; r < GetRowLength(); ++r )
        {
            for (size_t c = 0; c < GetColumnLength(); ++c )
            {
                (*this)[r][c] = initializer(r, GetRowLength(), c, GetColumnLength());
            }
        }
    }

    bool IsNull(void) const { return m_RowLength == 0; }
    size_t GetRowLength(void) const { return m_RowLength; }
    size_t GetColumnLength(void) const { return m_ColumnLength; }
    bool Resize(size_t, size_t);
private:
    // Member functions (private).
    void AllocateRow   (void);
    void AllocateColumn(void);
    void Allocate      (void);
    inline size_t Smaller(size_t length) const { return (length > m_RowLength) ? m_RowLength : length; };

    // Member variables (private).
    size_t m_RowLength;
    size_t m_ColumnLength;
    Vector<Type>** m_Array;
};


template <class Type>
void Matrix<Type>::Allocate(void)
{
    AllocateRow();
    AllocateColumn();
}

template <class Type>
void Matrix<Type>::AllocateRow(void)
{
    if (0 < m_RowLength)
    {
        m_Array = new Vector<Type>*[m_RowLength];
        ASSERT(NULL != m_Array);
    }
}

template <class Type>
void Matrix<Type>::AllocateColumn(void)
{
    for (size_t i = 0; i < m_RowLength; ++i)
    {
        m_Array[i] = new Vector<Type>(m_ColumnLength);
        ASSERT(m_Array[i] != NULL);
    }
}

template <class Type>
Matrix<Type>::Matrix(int rowLength, int columnLength)
    : m_RowLength(rowLength),
    m_ColumnLength(columnLength),
    m_Array(NULL)
{
    Allocate();
}

template <class Type>
Matrix<Type>::Matrix(const Matrix<Type>& m)
    : m_RowLength(m.GetRowLength()),
    m_ColumnLength(m.GetColumnLength()),
    m_Array(NULL)
{
    AllocateRow();
    for (size_t i = 0; i < m_RowLength; ++i)
    {
        m_Array[i] = new Vector<Type>(m[i]);
        ASSERT(m_Array[i] != NULL);
    }
}

template <class Type>
template <typename U>
Matrix<Type>::Matrix(const Matrix<U>& m)
    : m_RowLength(m.GetRowLength()),
    m_ColumnLength(m.GetColumnLength()),
    m_Array(NULL)
{
    AllocateRow();
    for (size_t i = 0; i < m_RowLength; ++i)
    {
        m_Array[i] = new Vector<Type>(m[i]);
        ASSERT(m_Array[i] != NULL);
    }
}

template <class Type>
Matrix<Type>::~Matrix()
{
    if (m_Array != NULL)
    {
        for (size_t i = 0; i < m_RowLength; ++i)
        {
            delete m_Array[i];
        }
        delete[] m_Array;
    }
    m_Array = NULL;
    m_RowLength = 0;
    m_ColumnLength = 0;
}

template <class Type>
Matrix<Type>::Matrix(const Matrixd& m)
    : m_RowLength(m.GetRowLength()),
    m_ColumnLength(m.GetColumnLength()),
    m_Array(NULL)
{
    AllocateRow();
    for (size_t i = 0; i < m_RowLength; ++i)
    {
        m_Array[i] = new Vector<Type>(m[i]);
        ASSERT(m_Array[i] != NULL);
    }
    for ( int r = 0; r < GetRowLength(); ++r )
    {
        (*this)[r] = m[r];
    }
}

template <class Type>
bool Matrix<Type>::Resize(size_t rowLength, size_t columnLength)
{
    if (rowLength < 0 || columnLength < 0)
    {
        return false;
    }

    if (rowLength != m_RowLength)
    {
        Vector<Type>** ptr = NULL;
        if (rowLength > 0)
        {
            ptr = new Vector<Type>*[rowLength];
            ASSERT(NULL != ptr);
            for (size_t i = 0; i < rowLength; ++i)
            {
                ptr[i] = NULL;
            }
        }

        for (size_t i = 0; i < Smaller(rowLength); ++i)
        {
            ptr[i] = m_Array[i];
            m_Array[i] = NULL;
        }
        for (size_t i = 0; i < rowLength; ++i)
        {
            if (ptr[i] == NULL)
            {
                ptr[i] = new Vector<Type>(columnLength);
                ASSERT(ptr[i] != NULL);
            }
        }
        for (size_t i = 0; i < m_RowLength; ++i)
        {
            if (m_Array[i] != NULL)
            {
                delete m_Array[i];
                m_Array[i] = NULL;
            }
        }
        delete[] m_Array;
        m_Array = ptr;
        m_RowLength = rowLength;
    }
    m_ColumnLength = columnLength;

    bool status = true;
    for (size_t i = 0; i < m_RowLength; ++i)
    {
        status &= (*m_Array[i]).Resize(m_ColumnLength);
    }
    return status;
}

template <class Type>
Matrix<Type>& Matrix<Type>::operator=(const Matrix<Type>& m)
{
    Resize(m.GetRowLength(), m.GetColumnLength());
    for (size_t i = 0; i < m_RowLength; ++i)
    {
        *m_Array[i] = m[i];
    }
    return *this;
}

template <class Type>
Matrix<Type>& Matrix<Type>::operator=(const Matrixd& m)
{
    Resize(m.GetRowLength(), m.GetColumnLength());
    for (size_t i = 0; i < m_RowLength; ++i)
    {
        *m_Array[i] = m[i];
    }
    return *this;
}

template <class Type>
Matrix<Type> Matrix<Type>::Transpose(void) const
{
    Matrix<Type> transposed(GetColumnLength(), GetRowLength());
    for (size_t i = 0; i < GetRowLength(); ++i)
    {
        for (size_t j = 0; j < GetColumnLength(); ++j)
        {
            transposed[j][i] = (*this)[i][j];
        }
    }
    return transposed;
}

template <class Type>
Matrix<Type> Matrix<Type>::Multiply(const Matrix<Type>& m) const
{
    if ( GetColumnLength() != m.GetRowLength() )
    {
        return *this;
    }
    Matrix<Type> multiplied(GetRowLength(), m.GetColumnLength());
    for (size_t row = 0; row < multiplied.GetRowLength(); ++row)
    {
        for (size_t col = 0; col < multiplied.GetColumnLength(); ++col)
        {
            Type v = 0;
            for (size_t k = 0; k < GetColumnLength(); ++k)
            {
                v += (*this)[row][k] * m[k][col];
            }
            multiplied[row][col] = v;
        }
    }
    return multiplied;
}

template <class Type>
Matrix<Type> Matrix<Type>::GetCofactorMatrix(size_t row, size_t col) const
{
    const size_t rowCount = GetRowLength();
    const size_t colCount = GetColumnLength();
    Matrix<Type> cofactorMatrix(rowCount-1, colCount-1);

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

template <class Type>
Type Matrix<Type>::GetCofactor(size_t row, size_t col) const
{
    int sign = ( (row + col) & 1) ? -1 : 1;
    return GetCofactorMatrix(row, col).Determinant() * sign;
}

template <class Type>
Type Matrix<Type>::Determinant(void) const
{
    if ( GetColumnLength() != GetRowLength() )
    {
        return 0;
    }
    int dimension = GetColumnLength();
    Type det = 0;

    // Sarrus
    if ( 3 == dimension )
    {
        const Matrix<Type>&m = *this;
        det = m[0][0] * m[1][1] * m[2][2]
              + m[0][1] * m[1][2] * m[2][0]
              + m[0][2] * m[1][0] * m[2][1]
              - m[0][2] * m[1][1] * m[2][0]
              - m[0][1] * m[1][0] * m[2][2]
              - m[0][0] * m[1][2] * m[2][1];
    }
    else if ( 2 == dimension )
    {
        const Matrix<Type>&m = *this;
        det = m[0][0] * m[1][1]
              - m[0][1] * m[1][0];
    }
    else if ( 1 == dimension )
    {
        const Matrix<Type>&m = *this;
        det = m[0][0];
    }
    else
    {
        for (size_t row = 0; row < GetRowLength(); ++row)
        {
            //Matrix<Type> m(CoFactor(row, 0));
            //DumpMatrix(m, "%f");
            //double v = m.Determinant() * sign * (*this)[row][0];
            det += GetCofactor(row, 0) * (*this)[row][0];
        }
    }
    return det;
}

template <class Type>
Matrix<Type> Matrix<Type>::Inverse(void) const
{
    if ( GetColumnLength() != GetRowLength() )
    {
        return *this;
    }

    Type det = Determinant();
    if ( 0 == det )
    {
        printf("Det=Zero\n");
        return *this;
    }
    // Calculate Inversed-Matrix by Cofactors.
    printf("Determinant=%f\n", det);
    const size_t rowCount = GetRowLength();
    const size_t colCount = GetColumnLength();
    Matrix<Type> inversed(rowCount, colCount);

#if 0
    for (size_t row = 0; row < rowCount; ++row)
    {
        for (size_t col = 0; col < colCount; ++col)
        {
            int sign = ((row + col) & 1) ? -1 : 1;
            Matrix<Type> m(GetCofactorMatrix(row, col));
            printf("cof[%d, %d]\n", row, col);
            //DumpMatrix(m, "%f");
            inversed[col][row] = GetCofactor(row, col) / det;
            printf("det=%f\n", inversed[row][col]);
        }
    }
#else
    // Calculate by Wipe-out
    static const Type threshold = 1.0e-10; // TBD

    Matrix<Type> m(rowCount, colCount*2);
    for (size_t row = 0; row < rowCount; ++row)
    {
        for (size_t col = 0; col < colCount; ++col)
        {
            m[row][col] = (*this)[row][col];
        }
    }
    for (size_t row = 0; row < rowCount; ++row)
    {
        for (size_t col = colCount; col < colCount*2; ++col)
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
    for (size_t iter = 0; iter < rowCount; ++iter)
    {
        // Find a row which m[row][i] is zero.
        bool isFound = false;
        for (size_t i = iter; i < rowCount; ++i)
        {
            if (fabs(m[i][iter]) > threshold)
            {
                Vector<Type> vec(m[i]);
                m[i] = m[iter];
                vec /= vec[iter];
                m[iter] = vec;
                isFound = true;
                break;
            }
        }
        // Give-up...
        if (false == isFound)
        {
            return *this;
        }
        for (size_t i = 0; i < rowCount; ++i)
        {
            if (i != iter)
            {
                Vector<Type> vec(m[iter]);
                vec *= m[i][iter];
                m[i] -= vec;
            }
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

} // namespace mcon {

#include "Matrixd.h"
