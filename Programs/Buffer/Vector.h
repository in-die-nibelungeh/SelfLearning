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

    explicit Vector(const int length = 0);
    Vector(const Vector<T>& v);
    template <typename U> Vector(const Vector<U>& v);
    ~Vector();

    // For const object
    const T& operator[](const int i) const
    {
        if (0 <= i && i < m_Length)
        {
            ASSERT(NULL != m_Address);
            return m_Address[i];
        }
        return m_Zero;
    }
    // For non-const object
    T& operator[](const int i)
    {
        if (0 <= i && i < m_Length)
        {
            ASSERT(NULL != m_Address);
            return m_Address[i];
        }
        m_Zero = 0;
        return m_Zero;
    }

    // Copy is to copy available data from src to dest without resizing the dest.
    const Vector<T>& Copy(const Vector<T>& vector);
    // operator= make the same vector as the input vector.
    Vector<T>& operator=(const Vector<T>& vector);

    operator void*() const
    {
        return reinterpret_cast<void*>(m_Address);
    }

    Vector<T>& operator =(T v) { VECTOR_ITERATION(i, m_Length, (*this)[i]  = v); return *this; };

    const Vector<T> operator +(T v) const { Vector<T> vec(*this);  vec += v; return vec; }
    const Vector<T> operator -(T v) const { Vector<T> vec(*this);  vec -= v; return vec; }
    const Vector<T> operator *(T v) const { Vector<T> vec(*this);  vec *= v; return vec; }
    const Vector<T> operator /(T v) const { Vector<T> vec(*this);  vec /= v; return vec; }

    const Vector<T> operator +(const Vector<T>& v) const { Vector<T> vec(*this);  vec += v; return vec; }
    const Vector<T> operator -(const Vector<T>& v) const { Vector<T> vec(*this);  vec -= v; return vec; }
    const Vector<T> operator *(const Vector<T>& v) const { Vector<T> vec(*this);  vec *= v; return vec; }
    const Vector<T> operator /(const Vector<T>& v) const { Vector<T> vec(*this);  vec /= v; return vec; }

    Vector<T>& operator+=(T v) { VECTOR_ITERATION(i, m_Length, (*this)[i] += v); return *this; }
    Vector<T>& operator-=(T v) { VECTOR_ITERATION(i, m_Length, (*this)[i] -= v); return *this; }
    Vector<T>& operator*=(T v) { VECTOR_ITERATION(i, m_Length, (*this)[i] *= v); return *this; }
    Vector<T>& operator/=(T v) { VECTOR_ITERATION(i, m_Length, (*this)[i] /= v); return *this; }

    Vector<T>& operator+=(const Vector<T>& v) { VECTOR_ITERATION(i, Smaller(v.GetLength()), (*this)[i] += v[i]); return *this; }
    Vector<T>& operator-=(const Vector<T>& v) { VECTOR_ITERATION(i, Smaller(v.GetLength()), (*this)[i] -= v[i]); return *this; }
    Vector<T>& operator*=(const Vector<T>& v) { VECTOR_ITERATION(i, Smaller(v.GetLength()), (*this)[i] *= v[i]); return *this; }
    Vector<T>& operator/=(const Vector<T>& v) { VECTOR_ITERATION(i, Smaller(v.GetLength()), (*this)[i] /= v[i]); return *this; }

    int GetLength(void) const { return m_Length; }

    bool Resize(const size_t length);

private:
    T*     m_Address;
    int    m_Length;
    T      m_Zero;
    int    Smaller(int input) const { return m_Length < input ? m_Length : input; }
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
Vector<T>::Vector(int length)
    : m_Address(NULL),
    m_Length(length),
    m_Zero(0)
{
    Allocate(length);
}

template <class T>
Vector<T>::Vector(const Vector<T>& v)
  : m_Length(v.GetLength()),
    m_Address(PTR_CAST(T*, NULL)),
    m_Zero(0)
{
    Allocate(v.GetLength());
    VECTOR_ITERATION(i, m_Length, (*this)[i] = v[i]);
}

template <class T>
template <typename U>
Vector<T>::Vector(const Vector<U>& v)
  : m_Length(v.GetLength()),
    m_Address(PTR_CAST(T*, NULL)),
    m_Zero(0)
{
    Allocate(v.GetLength());
    VECTOR_ITERATION(i, m_Length, (*this)[i] = static_cast<T>(v[i]));
}

template <class T>
Vector<T>::~Vector()
{
    if (NULL != m_Address)
    {
        delete[] m_Address;
        m_Address = PTR_CAST(T*, NULL);
    }
    m_Length = 0;
}

template <class T>
const Vector<T>& Vector<T>::Copy(const Vector<T>& v)
{
    VECTOR_ITERATION(i, Smaller(v.GetLength()), (*this)[i] = v[i]);
    return *this;
}

template <class T>
Vector<T>& Vector<T>::operator=(const Vector<T>& v)
{
    // m_Length is updated in Resize().
    Resize(v.GetLength());
    VECTOR_ITERATION(i, v.GetLength(), (*this)[i] = v[i]);
    return *this;
}

template <class T>
bool Vector<T>::Resize(size_t length)
{
    if (length <= 0)
    {
        return false;
    }
    if (length == m_Length)
    {
        return true;
    }
    if (NULL != m_Address)
    {
        delete[] m_Address;
    }
    m_Length = length;
    m_Address = new T[length];
    if (NULL == m_Address)
    {
        return false;
    }
    return true;
}

} // namespace mcon {
