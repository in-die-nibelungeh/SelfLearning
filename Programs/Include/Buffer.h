#pragma once

#include "types.h"
#include "debug.h"

namespace Container {

/*--------------------------------------------------------------------
 * Vector
 *--------------------------------------------------------------------*/
#define VECTOR_ITERATION(var, iter, statement)  \
    for (int var = 0; var < iter; ++var)        \
    {                                           \
        statement;                              \
    }

template <class T>
class Vector
{
public:

    explicit Vector(const int numData);

    template <class U>
    Vector(Vector<U>& vector);

    ~Vector();

/*
    template <class U>
    void Copy(Vector<U>& vector);
*/

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

    template<typename U> T& operator+=(U v) const { VECTOR_ITERATION(i, m_NumOfData, (*this)[i] += static_cast<T>(v)); }
    template<typename U> T& operator-=(U v) const { VECTOR_ITERATION(i, m_NumOfData, (*this)[i] -= static_cast<T>(v)); }
    template<typename U> T& operator*=(U v) const { VECTOR_ITERATION(i, m_NumOfData, (*this)[i] *= static_cast<T>(v)); }
    template<typename U> T& operator/=(U v) const { VECTOR_ITERATION(i, m_NumOfData, (*this)[i] /= static_cast<T>(v)); }

    template<typename U> T& operator+=(Vector<U>& v) const { VECTOR_ITERATION(i, Smaller(v.GetNumOfData()), (*this)[i] += static_cast<T>(v[i])); }
    template<typename U> T& operator-=(Vector<U>& v) const { VECTOR_ITERATION(i, Smaller(v.GetNumOfData()), (*this)[i] -= static_cast<T>(v[i])); }
    template<typename U> T& operator*=(Vector<U>& v) const { VECTOR_ITERATION(i, Smaller(v.GetNumOfData()), (*this)[i] *= static_cast<T>(v[i])); }
    template<typename U> T& operator/=(Vector<U>& v) const { VECTOR_ITERATION(i, Smaller(v.GetNumOfData()), (*this)[i] /= static_cast<T>(v[i])); }

    template <class U>
    Vector<T>& operator=(Vector<U>& vector);

    int GetNumOfData(void) const { return m_NumOfData; }
    bool Resize(const size_t numData);

private:
    T*     m_Address;
    int    m_NumOfData;
    T      m_Zero;
    T*     m_pZero;
    inline int min(int a, int b) const { return (a > b) ? b : a; };
    int    Smaller(int input) const { return m_NumOfData < input ? m_NumOfData : input; }
    //void   Allocate(void);
};

template <class T>
Vector<T>::Vector(int numData)
    : m_Address(new T[numData]),
    m_NumOfData(numData),
    m_Zero(0),
    m_pZero(&m_Zero)
{
    ASSERT(m_NumOfData > 0);
    ASSERT(NULL != m_Address);
}

template <class T>
template <class U>
Vector<T>::Vector(Vector<U>& v)
  : m_NumOfData(v.GetNumOfData()),
    m_Address(new T[v.GetNumOfData()]),
    m_Zero(0),
    m_pZero(&m_Zero)
{
    ASSERT(m_NumOfData > 0);
    ASSERT(NULL != m_Address);
    VECTOR_ITERATION(i, m_NumOfData, (*this)[i] = static_cast<T>(v[i]));
}

template <class T>
Vector<T>::~Vector()
{
    ASSERT(NULL != m_Address);
    delete[] m_Address;
    m_NumOfData = 0;
    m_Address = PTR_CAST(T*, NULL);
}

/*
template <class T>
template <class U>
void Vector<T>::Copy(Vector<U>& v)
{
    int iter = this->min(this->m_NumOfData, v.GetNumOfData());
    for (int i = 0; i < iter; ++i)
    {
        (*this)[i] = static_cast<T>(v[i]);
    }
}
*/

template <class T>
template <class U>
Vector<T>& Vector<T>::operator=(Vector<U>& v)
{
    // m_NumOfData is updated in Resize().
    VECTOR_ITERATION(i, Smaller(v.GetNumOfData()), (*this)[i] = static_cast<T>(v[i]));
    return *this;
}

/*
template <class T>
void Vector<T>::Allocate(void)
{
    ASSERT(m_NumOfData > 0);
    m_Address = new T[m_NumOfData];
    ASSERT(NULL != m_Address);
}
*/

template <class T>
bool Vector<T>::Resize(size_t numData)
{
    ASSERT(numData > 0);
    if (numData != m_NumOfData)
    {
        m_NumOfData = numData;
        ASSERT(NULL != m_Address);
        delete[] this->m_Address;
        m_Address = new T[m_NumOfData];
        ASSERT(NULL != m_Address);
        return true;
    }
    return false;
}

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

        Vector<T>* p = *m_ppZero;
        if (0 <= i && i < m_NumOfArray)
        {
            p = m_Array[i];
        }
        return *p;
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
    Vector<T>** m_ppZero;
    Vector<T>*  m_pZero;
    Vector<T>   m_Zero;
};

template <class T>
Matrix<T>::Matrix(int numArray, int numData)
  : m_Array(new Vector<T>*[numArray]),
    m_NumOfData(numData),
    m_NumOfArray(numArray),
    m_ppZero(&m_pZero),
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
    m_ppZero(&m_pZero),
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

} // namespace Container {
