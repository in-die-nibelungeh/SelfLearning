#pragma once

#include "types.h"
#include "debug.h"

namespace Container {

#define VECTOR_ITERATION(var, statement)        \
    for (int var = 0; var < m_NumOfData; ++var) \
    {                                           \
        statement;                              \
    }

template <class T>
class Vector
{
public:
    template <class U>
    Vector(Vector<U>& vector);

    explicit Vector(const int numData);
    ~Vector();

    template <class U>
    void Copy(Vector<U>& vector);

    T& operator[](const int i) const
    {
        ASSERT(0 < m_NumOfData);
        ASSERT(NULL != m_Address);
        if (0 <= i && i < m_NumOfData)
        {
            return m_Address[i];
        }
        *m_pZero = 0;
        return *m_pZero;
    }

    operator void*() const
    {
        return reinterpret_cast<void*>(m_Address);
    }

    template<typename U> T& operator+=(U v) const { VECTOR_ITERATION(i, (*this)[i] += static_cast<T>(v)); }
    template<typename U> T& operator-=(U v) const { VECTOR_ITERATION(i, (*this)[i] -= static_cast<T>(v)); }
    template<typename U> T& operator*=(U v) const { VECTOR_ITERATION(i, (*this)[i] *= static_cast<T>(v)); }
    template<typename U> T& operator/=(U v) const { VECTOR_ITERATION(i, (*this)[i] /= static_cast<T>(v)); }

    template<typename U> T& operator+=(Vector<U>& v) const
    {
        const int iter = min(m_NumOfData, v.GetNumOfData());
        for (int i = 0; i < iter; ++i) { (*this)[i] += static_cast<T>(v[i]); }
    }
    template<typename U> T& operator-=(Vector<U>& v) const
    {
        const int iter = min(m_NumOfData, v.GetNumOfData());
        for (int i = 0; i < iter; ++i) { (*this)[i] -= static_cast<T>(v[i]); }
    }
    template<typename U> T& operator*=(Vector<U>& v) const
    {
        const int iter = min(m_NumOfData, v.GetNumOfData());
        for (int i = 0; i < iter; ++i) { (*this)[i] *= static_cast<T>(v[i]); }
    }
    template<typename U> T& operator/=(Vector<U>& v) const
    {
        const int iter = min(m_NumOfData, v.GetNumOfData());
        for (int i = 0; i < iter; ++i) { (*this)[i] /= static_cast<T>(v[i]); }
    }

    template <class U>
    Vector<T>& operator=(Vector<U>& vector);

    int GetNumOfData(void) const { return m_NumOfData; }
    void Reallocate(const size_t numData);

private:
    T*     m_Address;
    int    m_NumOfData;
    T      m_Zero;
    T*     m_pZero;
    inline int min(int a, int b) const { return (a > b) ? b : a; };
};

template <class T>
Vector<T>::Vector(int numData)
    :
    m_Address(PTR_CAST(T*, NULL)),
    m_NumOfData(numData),
    m_Zero(0),
    m_pZero(&m_Zero)
{
    if (m_NumOfData > 0)
    {
        m_Address = new T[m_NumOfData];
        ASSERT(NULL != m_Address);
    }
}

template <class T>
template <class U>
Vector<T>::Vector(Vector<U>& b)
  : m_Address(PTR_CAST(T*, NULL)),
    m_Zero(0),
    m_pZero(&m_Zero)
{
    *this = b;
}

template <class T>
Vector<T>::~Vector()
{
    if (NULL != m_Address)
    {
        delete[] m_Address;
        m_Address = PTR_CAST(T*, NULL);
    }
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
    // m_NumOfData is updated in Reallocate().
    Reallocate(b.GetNumOfData());

    if (m_NumOfData > 0)
    {
        for (int i = 0; i < m_NumOfData; ++i)
        {
            (*this)[i] = static_cast<T>(b[i]);
        }
    }
    return *this;
}

template <class T>
void Vector<T>::Reallocate(size_t numData)
{
    if (numData == m_NumOfData)
    {
        return ;
    }
    if (NULL != m_Address)
    {
        delete[] this->m_Address;
        m_Address = PTR_CAST(T*, NULL);
    }
    m_NumOfData = numData;
    if (m_NumOfData > 0)
    {
        m_Address = new T[m_NumOfData];
        ASSERT(NULL != m_Address);
    }
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
    void Reallocate(int, int);
};

template <class T>
Matrix<T>::Matrix(int numArray, int numData)
  : m_Array(PTR_CAST(Vector<T>**, NULL)),
    m_NumOfData(numData),
    m_NumOfArray(numArray)
{
    if (m_NumOfArray > 0)
    {
        m_Array = new Vector<T>*[m_NumOfArray];
        ASSERT(m_Array != NULL);
        for (int i = 0; i < m_NumOfArray; ++i)
        {
            if (m_NumOfData > 0)
            {
                m_Array[i] = new Vector<T>(m_NumOfData);
                ASSERT(m_Array[i] != NULL);
            }
            else
            {
                m_Array[i] = PTR_CAST(Vector<T>*, NULL);
            }
        }
    }
}

template <class T>
Matrix<T>::~Matrix()
{

    if (m_Array != NULL)
    {
        for (int i = 0; i < m_NumOfArray; ++i)
        {
            if (PTR_CAST(Vector<T>*, NULL) != m_Array[i])
            {
                delete m_Array[i];
            }
        }
        delete[] m_Array;
        m_Array = PTR_CAST(Vector<T>**, NULL);
    }
}

template <class T>
void Matrix<T>::Reallocate(int numArray, int numData)
{
    if (numArray != m_NumOfArray)
    {
        for (int i = 0; i < m_NumOfArray; ++i)
        {
            if (NULL != m_Array[i])
            {
                delete m_Array[i];
            }
        }
        delete[] m_Array;
        m_Array = PTR_CAST(Vector<T>**, NULL);

        m_NumOfArray = numArray;

        if (m_NumOfArray > 0)
        {
            m_Array = new Vector<T>*[m_NumOfArray];
            ASSERT(NULL != m_Array);
            m_NumOfData = numData;
            for (int i = 0; i < m_NumOfArray; ++i)
            {
                if (m_NumOfData > 0)
                {
                    m_Array[i] = new Vector<T>(m_NumOfData);
                }
                else
                {
                    m_Array[i] = PTR_CAST(Vector<T>*, NULL);
                }
                ASSERT(NULL != m_Array[i]);
            }
        }
    }
    else if (numData != m_NumOfData)
    {
        m_NumOfData = numData;

        for (int i = 0; i < m_NumOfArray; ++i)
        {
            if (NULL != m_Array[i])
            {
                delete m_Array[i];
                m_Array[i] = PTR_CAST(Vector<T>*, NULL);
            }
            if (m_NumOfData > 0)
            {
                m_Array[i] = new Vector<T>(m_NumOfData);
                ASSERT(m_Array[i] != PTR_CAST(Vector<T>*, NULL));
            }
        }
    }
}

template <class T>
template <class U>
Matrix<T>& Matrix<T>::operator=(Matrix<U>& m)
{
    Reallocate(m.GetNumOfArray(), m.GetNumOfData());
    for (int i = 0; i < m_NumOfArray; ++i)
    {
        *m_Array[i] = m[i];
    }
    return *this;
}

template <class T>
template <class U>
Matrix<T>::Matrix(Matrix<U>& m)
  : m_Array(NULL),
    m_NumOfArray(0),
    m_NumOfData(0)
{
    *this = m;
}

} // namespace Container {
