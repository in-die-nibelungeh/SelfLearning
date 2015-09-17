#pragma once

#include "types.h"
#include "debug.h"
#include "Vector.h"

namespace mcon {

/*--------------------------------------------------------------------
 * Matrix
 *--------------------------------------------------------------------*/
template <class T>
class Matrix
{
public:
    void DumpMatrix(const Matrix<T>&m, const char* fmt) const
    {
        for (int i = 0; i < m.GetNumOfArray(); ++i)
        {
            printf("| ");
            for (int j = 0; j < m.GetNumOfData(); ++j)
            {
                printf(fmt, m[i][j]);
                printf("\t");
            }
            printf(" |\n");
        }
    }
    Matrix(int ,int);
    Matrix(const Matrix<T>& m);
    ~Matrix();

    const Vector<T>& operator[](int i) const
    {
        ASSERT(0 < m_NumOfData);
        ASSERT(0 < m_NumOfArray);
        ASSERT(NULL != m_Array);

        if (0 <= i && i < m_NumOfArray)
        {
            return *m_Array[i];
        }
        return m_Zero;
    }

    Vector<T>& operator[](int i)
    {
        ASSERT(0 < m_NumOfData);
        ASSERT(0 < m_NumOfArray);
        ASSERT(NULL != m_Array);

        if (0 <= i && i < m_NumOfArray)
        {
            return *m_Array[i];
        }
        return m_Zero;
    }

    Matrix<T>& operator+=(T v) { for (int i = 0; i < GetNumOfArray(); ++i) { (*this)[i] += v; } return *this; }
    Matrix<T>& operator-=(T v) { (*this) += (-v); return *this; }
    Matrix<T>& operator*=(T v) { for (int i = 0; i < GetNumOfArray(); ++i) { (*this)[i] *= v; } }
    Matrix<T>& operator/=(T v) { (*this) *= (1/v); return *this; }

    const Matrix<T> operator+(const Matrix<T>& m) const { Matrix<T> mat(*this); for (int i = 0; i < GetNumOfArray(); ++i) { mat[i] += m[i]; } return mat; }
    const Matrix<T> operator-(const Matrix<T>& m) const { Matrix<T> mat(*this); for (int i = 0; i < GetNumOfArray(); ++i) { mat[i] -= m[i]; } return mat; }

    const Matrix<T> operator*(T v)
    {
        Matrix<T> m(*this);
        for (int row = 0; row < GetNumOfArray(); ++row)
        {
            for (int col = 0; col < GetNumOfData(); ++col)
            {
                m[row][col] *= v;
            }
        }
        return m;
    }

    const Matrix<T>& operator=(const Matrix<T>& m);

    const Matrix<T> Transpose(void) const;
    const Matrix<T> Multiply(const Matrix<T>& m) const ;
    const Matrix<T> Inverse(void) const ;
    T Determinant(void) const;
    const Matrix<T> GetCofactorMatrix(int row, int col) const;
    T GetCofactor(int row, int col) const;

    static Matrix<T> Identify(int size)
    {
        Matrix<T> I(size, size);
        for (int i = 0; i < size; ++i)
        {
            I[i] = 0;
        }
        for (int i = 0; i < size; ++i)
        {
            I[i][i] = 1;
        }
        return I;
    }

    int GetNumOfArray(void) const { return m_NumOfArray; }
    int GetNumOfData(void) const { return m_NumOfData; }
    void Resize(int, int);
private:
    int m_NumOfArray;
    int m_NumOfData;
    Vector<T>** m_Array;
    Vector<T>   m_Zero;
};

template <class T>
Matrix<T>::Matrix(int numArray, int numData)
  : m_Array(new Vector<T>*[numArray]),
    m_NumOfData(numData),
    m_NumOfArray(numArray),
    m_Zero(0)
{
    ASSERT(m_NumOfArray > 0);
    ASSERT(m_NumOfData > 0);
    ASSERT(NULL != m_Array);

    printf("%s(int,int): Begin (%p)\n", __func__, this);
    for (int i = 0; i < m_NumOfArray; ++i)
    {
        m_Array[i] = new Vector<T>(m_NumOfData);
        ASSERT(m_Array[i] != NULL);
    }
}

template <class T>
Matrix<T>::Matrix(const Matrix<T>& m)
  : m_Array(new Vector<T>*[m.GetNumOfArray()]),
    m_NumOfArray(m.GetNumOfArray()),
    m_NumOfData(m.GetNumOfData()),
    m_Zero(0)
{
    ASSERT(m_NumOfArray > 0);
    ASSERT(m_NumOfData > 0);
    ASSERT(NULL != m_Array);

    printf("%s(Matrix): Begin (%p)\n", __func__, this);
    for (int i = 0; i < m_NumOfArray; ++i)
    {
        m_Array[i] = new Vector<T>(m_NumOfData);
        ASSERT(m_Array[i] != NULL);
        *m_Array[i] = m[i];
    }
}

template <class T>
Matrix<T>::~Matrix()
{
    ASSERT(m_Array != NULL);
    printf("%s: Begin (%p)\n", __func__, this);
    for (int i = 0; i < m_NumOfArray; ++i)
    {
        ASSERT(PTR_CAST(Vector<T>*, NULL) != m_Array[i]);
        delete m_Array[i];
    }
    delete[] m_Array;
    m_Array = PTR_CAST(Vector<T>**, NULL);
    m_NumOfArray = 0;
    m_NumOfData = 0;
}

template <class T>
void Matrix<T>::Resize(int numArray, int numData)
{
    if (numArray <= 0 || numData <= 0)
    {
        return ;
    }
    ASSERT(m_NumOfArray > 0);
    if (numArray != m_NumOfArray)
    {
        for (int i = 0; i < m_NumOfArray; ++i)
        {
            ASSERT(NULL != m_Array[i]);
            delete m_Array[i];
        }
        delete[] m_Array;
        m_NumOfArray = numArray;
        m_NumOfData = numData;

        m_Array = new Vector<T>*[m_NumOfArray];
        ASSERT(NULL != m_Array);
        for (int i = 0; i < m_NumOfArray; ++i)
        {
            m_Array[i] = new Vector<T>(m_NumOfData);
            ASSERT(NULL != m_Array[i]);
        }
    }
    else if (numData != m_NumOfData)
    {
        m_NumOfData = numData;

        for (int i = 0; i < m_NumOfArray; ++i)
        {
            ASSERT(NULL != m_Array[i]);
            delete m_Array[i];
            m_Array[i] = new Vector<T>(m_NumOfData);
            ASSERT(m_Array[i] != PTR_CAST(Vector<T>*, NULL));
        }
    }
}

template <class T>
const Matrix<T>& Matrix<T>::operator=(const Matrix<T>& m)
{
    Resize(m.GetNumOfArray(), m.GetNumOfData());
    for (int i = 0; i < m_NumOfArray; ++i)
    {
        *m_Array[i] = m[i];
    }
    return *this;
}

template <class T>
const Matrix<T> Matrix<T>::Transpose(void) const
{
    Matrix<T> transposed(GetNumOfData(), GetNumOfArray());
    for (int i = 0; i < GetNumOfArray(); ++i)
    {
        for (int j = 0; j < GetNumOfData(); ++j)
        {
            transposed[j][i] = (*this)[i][j];
        }
    }
    return transposed;
}

template <class T>
const Matrix<T> Matrix<T>::Multiply(const Matrix<T>& m) const
{
    if ( GetNumOfData() != m.GetNumOfArray() )
    {
        return *this;
    }
    Matrix<T> multiplied(GetNumOfArray(), m.GetNumOfData());
    for (int row = 0; row < multiplied.GetNumOfArray(); ++row)
    {
        for (int col = 0; col < multiplied.GetNumOfData(); ++col)
        {
            T v = 0;
            for (int k = 0; k < GetNumOfData(); ++k)
            {
                v += (*this)[row][k] * m[k][col];
            }
            multiplied[row][col] = v;
        }
    }
    return multiplied;
}

template <class T>
const Matrix<T> Matrix<T>::GetCofactorMatrix(int row, int col) const
{
    int rowCount = GetNumOfArray();
    int colCount = GetNumOfData();
    Matrix<T> cofactorMatrix(rowCount-1, colCount-1);

    for (int r = 0, ri = 0; ri < rowCount; ++ri)
    {
        if (ri == row)
        {
            continue;
        }
        for (int c = 0, ci = 0; ci < colCount; ++ci)
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

template <class T>
T Matrix<T>::GetCofactor(int row, int col) const
{
    int sign = ( (row + col) & 1) ? -1 : 1;
    return GetCofactorMatrix(row, col).Determinant() * sign;
}

template <class T>
T Matrix<T>::Determinant(void) const
{
    if ( GetNumOfData() != GetNumOfArray() )
    {
        return 0;
    }
    int dimension = GetNumOfData();
    T det = 0;

    // Sarrus
    if ( 3 == dimension )
    {
        const Matrix<T>&m = *this;
        det = m[0][0] * m[1][1] * m[2][2]
              + m[0][1] * m[1][2] * m[2][0]
              + m[0][2] * m[1][0] * m[2][1]
              - m[0][2] * m[1][1] * m[2][0]
              - m[0][1] * m[1][0] * m[2][2]
              - m[0][0] * m[1][2] * m[2][1];
    }
    else if ( 2 == dimension )
    {
        const Matrix<T>&m = *this;
        det = m[0][0] * m[1][1]
              - m[0][1] * m[1][0];
    }
    else if ( 1 == dimension )
    {
        const Matrix<T>&m = *this;
        det = m[0][0];
    }
    else
    {
        for (int row = 0; row < GetNumOfArray(); ++row)
        {
            //Matrix<T> m(CoFactor(row, 0));
            //DumpMatrix(m, "%f");
            //double v = m.Determinant() * sign * (*this)[row][0];
            det += GetCofactor(row, 0) * (*this)[row][0];
        }
    }
    return det;
}

template <class T>
const Matrix<T> Matrix<T>::Inverse(void) const
{
    if ( GetNumOfData() != GetNumOfArray() )
    {
        return *this;
    }

    T det = Determinant();
    if ( 0 == det )
    {
        printf("Det=Zero\n");
        return *this;
    }
    // Calculate Inversed-Matrix by Cofactors.
    printf("Determinant=%f\n", det);
    int rowCount = GetNumOfArray();
    int colCount = GetNumOfData();
    Matrix<T> inversed(rowCount, colCount);

#if 0
    for (int row = 0; row < rowCount; ++row)
    {
        for (int col = 0; col < colCount; ++col)
        {
            int sign = ((row + col) & 1) ? -1 : 1;
            Matrix<T> m(GetCofactorMatrix(row, col));
            printf("cof[%d, %d]\n", row, col);
            //DumpMatrix(m, "%f");
            inversed[col][row] = GetCofactor(row, col) / det;
            printf("det=%f\n", inversed[row][col]);
        }
    }
#else
    // Calculate by Wipe-out
    static const T threshold = 1.0e-10; // TBD

    Matrix<T> m(rowCount, colCount*2);
    for (int row = 0; row < rowCount; ++row)
    {
        for (int col = 0; col < colCount; ++col)
        {
            m[row][col] = (*this)[row][col];
        }
    }
    for (int row = 0; row < rowCount; ++row)
    {
        for (int col = colCount; col < colCount*2; ++col)
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
    for (int iter = 0; iter < rowCount; ++iter)
    {
        // Find a row which m[row][i] is zero.
        bool isFound = false;
        for (int i = iter; i < rowCount; ++i)
        {
            if (fabs(m[i][iter]) > threshold)
            {
                Vector<T> vec(m[i]);
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
        for (int i = 0; i < rowCount; ++i)
        {
            if (i != iter)
            {
                Vector<T> vec(m[iter]);
                vec *= m[i][iter];
                m[i] -= vec;
            }
        }
    }
    for (int row = 0; row < rowCount; ++row)
    {
        for (int col = 0; col < colCount; ++col)
        {
            inversed[row][col] = m[row][col+colCount];
        }
    }
#endif
    return inversed;
}

} // namespace mcon {
