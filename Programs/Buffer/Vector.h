#pragma once

#include "types.h"
#include "debug.h"

namespace container {

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

} // namespace container {
