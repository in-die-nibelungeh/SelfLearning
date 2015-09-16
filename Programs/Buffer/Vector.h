#pragma once

#include "types.h"
#include "debug.h"

namespace mcon {

#define VECTOR_ITERATION(var, iter, statement)  \
    for (int var = 0; var < iter; ++var)        \
    {                                           \
        statement;                              \
    }

template <class T>
class Vector
{
public:

    explicit Vector(const int numData = 0);
    Vector(const Vector<T>& v);
    template <typename U> Vector(const Vector<U>& v);
    ~Vector();


    T& operator[](const int i) const
    {
        if (0 <= i && i < m_NumOfData)
        {
            ASSERT(NULL != m_Address);
            return m_Address[i];
        }
        *m_pZero = 0;
        return *m_pZero;
    }

    // Copy is to copy available data from src to dest without resizing the dest.
    const Vector<T>& Copy(Vector<T>& vector) const;
    // operator= make the same vector as the input vector.
    const Vector<T>& operator=(Vector<T>& vector);

    template <typename U>
    operator U() const
    {
        Vector<U> v;
        VECTOR_ITERATION(i, GetLength(), v[i] = static_cast<U>((*this)[i]));
        return v;
    }
    /*
    const Vector<int> operator int() const
    {
        Vector<int> v;
        VECTOR_ITERATION(i, GetLength(), v[i] = static_cast<int>((*this)[i]));
        return v;
    }
    */

    operator void*() const
    {
        return reinterpret_cast<void*>(m_Address);
    }

    Vector<T>& operator =(T v) { VECTOR_ITERATION(i, m_NumOfData, (*this)[i]  = v); return *this; };

    Vector<T>& operator+=(T v) { VECTOR_ITERATION(i, m_NumOfData, (*this)[i] += v); return *this; }
    Vector<T>& operator-=(T v) { VECTOR_ITERATION(i, m_NumOfData, (*this)[i] -= v); return *this; }
    Vector<T>& operator*=(T v) { VECTOR_ITERATION(i, m_NumOfData, (*this)[i] *= v); return *this; }
    Vector<T>& operator/=(T v) { VECTOR_ITERATION(i, m_NumOfData, (*this)[i] /= v); return *this; }

    Vector<T>& operator+=(Vector<T>& v) { VECTOR_ITERATION(i, Smaller(v.GetNumOfData()), (*this)[i] += v[i]); return *this; }
    Vector<T>& operator-=(Vector<T>& v) { VECTOR_ITERATION(i, Smaller(v.GetNumOfData()), (*this)[i] -= v[i]); return *this; }
    Vector<T>& operator*=(Vector<T>& v) { VECTOR_ITERATION(i, Smaller(v.GetNumOfData()), (*this)[i] *= v[i]); return *this; }
    Vector<T>& operator/=(Vector<T>& v) { VECTOR_ITERATION(i, Smaller(v.GetNumOfData()), (*this)[i] /= v[i]); return *this; }

    int GetLength(void) const { return GetNumOfData(); }
    int GetNumOfData(void) const { return m_NumOfData; }
    bool Resize(const size_t numData);

private:
    T*     m_Address;
    int    m_NumOfData;
    T      m_Zero;
    T*     m_pZero;
    int    Smaller(int input) const { return m_NumOfData < input ? m_NumOfData : input; }
    void   Allocate(int);
};

template <class T>
void Vector<T>::Allocate(int length)
{
    m_Address = PTR_CAST(T*, NULL);
    if (length > 0)
    {
        m_Address = new T[length];
        ASSERT(NULL != m_Address);
    }
}

template <class T>
Vector<T>::Vector(int numData)
    : m_Address(NULL),
    m_NumOfData(numData),
    m_Zero(0),
    m_pZero(&m_Zero)
{
    Allocate(numData);
}

template <class T>
Vector<T>::Vector(const Vector<T>& v)
  : m_NumOfData(v.GetNumOfData()),
    m_Address(PTR_CAST(T*, NULL)),
    m_Zero(0),
    m_pZero(&m_Zero)
{
    Allocate(v.GetNumOfData());
    VECTOR_ITERATION(i, m_NumOfData, (*this)[i] = v[i]);
}

template <class T>
template <typename U>
Vector<T>::Vector(const Vector<U>& v)
  : m_NumOfData(v.GetNumOfData()),
    m_Address(PTR_CAST(T*, NULL)),
    m_Zero(0),
    m_pZero(&m_Zero)
{
    Allocate(v.GetNumOfData());
    VECTOR_ITERATION(i, m_NumOfData, (*this)[i] = static_cast<T>(v[i]));
}

template <class T>
Vector<T>::~Vector()
{
    if (NULL != m_Address)
    {
        delete[] m_Address;
        m_Address = PTR_CAST(T*, NULL);
    }
    m_NumOfData = 0;
}

template <class T>
const Vector<T>& Vector<T>::Copy(Vector<T>& v) const
{
    VECTOR_ITERATION(i, Smaller(v.GetNumOfData()), (*this)[i] = v[i]);
    return *this;
}

template <class T>
const Vector<T>& Vector<T>::operator=(Vector<T>& v)
{
    // m_NumOfData is updated in Resize().
    Resize(v.GetLength());
    VECTOR_ITERATION(i, v.GetLength(), (*this)[i] = v[i]);
    return *this;
}


template <class T>
bool Vector<T>::Resize(size_t numData)
{
    if (numData <= 0)
    {
        return false;
    }
    if (numData == m_NumOfData)
    {
        return true;
    }
    if (NULL != m_Address)
    {
        delete[] m_Address;
    }
    m_NumOfData = numData;
    m_Address = new T[numData];
    if (NULL == m_Address)
    {
        return false;
    }
    return true;
}

} // namespace mcon {
