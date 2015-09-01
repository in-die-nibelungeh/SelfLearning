#pragma once

#include "types.h"
#include "debug.h"

#include <stdlib.h>

namespace Container {

template <class T>
class Vector
{
    T*     m_Address;
    int    m_NumOfData;
    inline int min(int a, int b) const { return (a > b) ? b : a; };

public:
    template <class U>
    Vector(Vector<U>& );

    Vector(int numData);
    ~Vector();

    template <class U>
    void Copy(Vector<U>& b);

    T& operator[](int i) const
    {
        ASSERT(0 < m_NumOfData);
        ASSERT(NULL != m_Address);
        if (0 > i)
        {
            return m_Address[0];
        }
        if (i >= m_NumOfData)
        {
            return m_Address[m_NumOfData-1];
        }
        return m_Address[i];
    }

    template <class U>
    Vector<T>& operator=(Vector<U>& b);

    int GetNumOfData(void) const
    {
        return m_NumOfData;
    }
};

template <class T>
Vector<T>::Vector(int numData)
    :
    m_Address(new T[numData]),
    m_NumOfData(numData)
{
    ASSERT(NULL != m_Address);
}

template <class T>
template <class U>
Vector<T>::Vector(Vector<U>& b)
  : m_Address(NULL)
{
    *this = b;
}

template <class T>
Vector<T>::~Vector()
{
    ASSERT(NULL != m_Address);
    delete[] m_Address;
    m_Address = PTR_CAST(T*, NULL);
}

template <class T>
template <class U>
void Vector<T>::Copy(Vector<U>& b)
{
    int iter = this->min(this->m_NumOfData, b.GetNumOfData());
    for (int i = 0; i < iter; ++i)
    {
        (*this)[i] = static_cast<T>(b[i]);
    }
}

template <class T>
template <class U>
Vector<T>& Vector<T>::operator=(Vector<U>& b)
{
    if (NULL != m_Address)
    {
        delete[] this->m_Address;
    }

    m_NumOfData = b.GetNumOfData();
    m_Address = new T[m_NumOfData];

    ASSERT(NULL != m_Address);

    for (int i = 0; i < m_NumOfData; ++i)
    {
        (*this)[i] = static_cast<T>(b[i]);
    }
    return *this;
}


template <class T>
class Matrix
{
    int m_NumOfArray;
    int m_NumOfData;
    Vector<T>** m_Array;
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

        Vector<T>* p;
        if (0 > i)
        {
            p = m_Array[0];
        }
        else if (i >= m_NumOfArray)
        {
            p = m_Array[m_NumOfArray-1];
        }
        else
        {
            p = m_Array[i];
        }
        ASSERT(NULL != p);

        return *p;
    }

    template <class U>
    Matrix<T>& operator=(Matrix<U>& b);

    int GetNumOfArray(void) const
    {
        return m_NumOfArray;
    }
    int GetNumOfData(void) const
    {
        return m_NumOfData;
    }
};

template <class T>
Matrix<T>::Matrix(int numArray, int numData)
  : m_NumOfData(numData),
    m_NumOfArray(numArray),
    //m_Array(PTR_CAST(Vector<T>**, malloc(sizeof(Vector<T*>) * numArray)))
    m_Array(new Vector<T>*[numArray])
{
    ASSERT(m_Array != NULL);
    for (int i = 0; i < m_NumOfArray; ++i)
    {
        m_Array[i] = new Vector<T>(m_NumOfData);
        ASSERT(m_Array[i] != NULL);
    }
}

template <class T>
Matrix<T>::~Matrix()
{
    ASSERT(m_Array != NULL);
    for (int i = 0; i < m_NumOfArray; ++i)
    {
        delete m_Array[i];
    }
    delete[] m_Array;
    m_Array = NULL;
}

template <class T>
template <class U>
Matrix<T>& Matrix<T>::operator=(Matrix<U>& m)
{
    m_NumOfArray = m.GetNumOfArray();
    m_NumOfData = m.GetNumOfData();

    if (NULL != m_Array)
    {
        delete[] m_Array;
    }

    m_Array = new Vector<T>*[m_NumOfArray];

    for (int i = 0; i < m_NumOfArray; ++i)
    {
        m_Array[i] = new Vector<T>(m[i]);
    }
    return *this;
}

template <class T>
template <class U>
Matrix<T>::Matrix(Matrix<U>& m)
  : m_Array(NULL)
{
    *this = m;
}

} // namespace Container {
