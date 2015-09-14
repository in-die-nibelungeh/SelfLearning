#pragma once

#include "types.h"
#include "debug.h"
#include "Vector.h"

namespace cod {

/*--------------------------------------------------------------------
 * Matrix
 *--------------------------------------------------------------------*/
template <class T>
class Matrix
{
public:
    Matrix(int ,int);

    Matrix(const Matrix<T>& m);

    template <class U>
    Matrix(const Matrix<U>& m);

    ~Matrix();

    Vector<T>& operator[](int i) const
    {
        ASSERT(0 < m_NumOfData);
        ASSERT(0 < m_NumOfArray);
        ASSERT(NULL != m_Array);

        if (0 <= i && i < m_NumOfArray)
        {
            return *m_Array[i];
        }
        m_Zero[0] = 0;
        return *m_pZero;
    }

    template <class U>
    const Matrix<T>& operator=(const Matrix<U>& m);
    const Matrix<T>& operator=(const Matrix<T>& m);

    const Matrix<T> Transpose(void) const;
    const Matrix<T> Multiply(const Matrix<T>& m) const ;
    T Determinant(void) const;

    int GetNumOfArray(void) const { return m_NumOfArray; }
    int GetNumOfData(void) const { return m_NumOfData; }
    void Resize(int, int);
private:
    int m_NumOfArray;
    int m_NumOfData;
    Vector<T>** m_Array;
    Vector<T>*  m_pZero;
    Vector<T>   m_Zero;
};

template <class T>
Matrix<T>::Matrix(int numArray, int numData)
  : m_Array(new Vector<T>*[numArray]),
    m_NumOfData(numData),
    m_NumOfArray(numArray),
    m_pZero(&m_Zero),
    m_Zero(1)
{
    ASSERT(m_NumOfArray > 0);
    ASSERT(m_NumOfData > 0);
    ASSERT(NULL != m_Array);

    for (int i = 0; i < m_NumOfArray; ++i)
    {
        m_Array[i] = new Vector<T>(m_NumOfData);
        ASSERT(m_Array[i] != NULL);
    }
    m_Zero[0] = 0;
}

template <class T>
Matrix<T>::Matrix(const Matrix<T>& m)
  : m_Array(new Vector<T>*[m.GetNumOfArray()]),
    m_NumOfArray(m.GetNumOfArray()),
    m_NumOfData(m.GetNumOfData()),
    m_pZero(&m_Zero),
    m_Zero(1)
{
    ASSERT(m_NumOfArray > 0);
    ASSERT(m_NumOfData > 0);
    ASSERT(NULL != m_Array);

    for (int i = 0; i < m_NumOfArray; ++i)
    {
        m_Array[i] = new Vector<T>(m_NumOfData);
        ASSERT(m_Array[i] != NULL);
        *m_Array[i] = m[i];
    }
    m_Zero[0] = 0;
}

template <class T>
template <class U>
Matrix<T>::Matrix(const Matrix<U>& m)
  : m_Array(new Vector<T>*[m.GetNumOfArray()]),
    m_NumOfArray(m.GetNumOfArray()),
    m_NumOfData(m.GetNumOfData()),
    m_pZero(&m_Zero),
    m_Zero(1)
{
    ASSERT(m_NumOfArray > 0);
    ASSERT(m_NumOfData > 0);
    ASSERT(NULL != m_Array);

    for (int i = 0; i < m_NumOfArray; ++i)
    {
        m_Array[i] = new Vector<T>(m_NumOfData);
        ASSERT(m_Array[i] != NULL);
        *m_Array[i] = m[i];
    }
    m_Zero[0] = 0;
}

template <class T>
Matrix<T>::~Matrix()
{
    ASSERT(m_Array != NULL);
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
template <class U>
const Matrix<T>& Matrix<T>::operator=(const Matrix<U>& m)
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
T Matrix<T>::Determinant(void) const
{
    if ( GetNumOfData() != GetNumOfArray() )
    {
        return 0;
    }
    T ret = 0;

    for (int col = 0; col < GetNumOfData(); ++col)
    {
        T v = 1;
        int numCol = GetNumOfData();
        for (int k = 0; k < GetNumOfArray(); ++k)
        {
            v *= (*this)[k][(k+col) % numCol];
        }
        ret += v;
    }
    for (int col = 0; col < GetNumOfData(); ++col)
    {
        T v = 1;
        int numCol = GetNumOfData();
        for (int k = 0; k < GetNumOfArray(); ++k)
        {
            v *= (*this)[k][(col-k+numCol) % numCol];
        }
        ret -= v;
    }
    return ret;
}


} // namespace cod {
