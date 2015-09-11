#pragma once

#include "types.h"
#include "debug.h"
#include "Vector.h"

namespace container {

/*--------------------------------------------------------------------
 * Matrix
 *--------------------------------------------------------------------*/
template <class T>
class Matrix
{
public:
    Matrix(int ,int);

    template <class U>
    Matrix(Matrix<U>& m);

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
    Matrix<T>& operator=(Matrix<U>& b);

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
template <class U>
Matrix<T>::Matrix(Matrix<U>& m)
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
template <class U>
Matrix<T>& Matrix<T>::operator=(Matrix<U>& m)
{
    Resize(m.GetNumOfArray(), m.GetNumOfData());
    for (int i = 0; i < m_NumOfArray; ++i)
    {
        *m_Array[i] = m[i];
    }
    return *this;
}

} // namespace container {
