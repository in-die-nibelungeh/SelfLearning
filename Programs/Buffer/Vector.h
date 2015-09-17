#pragma once

#include "types.h"
#include "debug.h"

namespace mcon {

template <class Type> class Matrix;

#define MCON_ITERATION(var, iter, statement)  \
    do {                                             \
        for (int var = 0; var < iter; ++var)         \
        {                                            \
            statement;                               \
        }                                            \
    } while(0)

template <class Type>
class Vector
{
public:

    explicit Vector(const int length = 0);
    Vector(const Vector<Type>& v);
    template <typename U> Vector(const Vector<U>& v);
    ~Vector();

    // For const object
    const Type& operator[](const int i) const
    {
        if (0 <= i && i < m_Length)
        {
            return m_Address[i];
        }
        return m_Zero;
    }
    // For non-const object
    Type& operator[](const int i)
    {
        if (0 <= i && i < m_Length)
        {
            return m_Address[i];
        }
        m_Zero = 0;
        return m_Zero;
    }

    // Copy is to copy available data from src to dest without resizing the dest.
    const Vector<Type>& Copy(const Vector<Type>& vector);
    // operator= make the same vector as the input vector.
    Vector<Type>& operator=(const Vector<Type>& vector);

    operator void*() const
    {
        return reinterpret_cast<void*>(m_Address);
    }

    Vector<Type> operator()(int offset, int length) const
    {
        Vector<Type> carveout;
        if (offset < 0 || GetLength() <= offset || length < 0)
        {
            // Null object.
            return carveout;
        }
        // Smaller value as length
        carveout.Resize( Smaller(GetLength() - offset, length) );
        for (int i = offset; i < Smaller(offset + length); ++i)
        {
            carveout[i-offset] = (*this)[i];
        }
        return carveout;
    }

    Type Fifo(Type v)
    {
        Type ret = (*this)[0];
        for (int i = 0; i < GetLength(); ++i)
        {
            (*this)[i] = (*this)[i+1];
        }
        (*this)[GetLength()-1] = v;
        return ret;
    }

    Vector<Type>& operator=(Type v) { MCON_ITERATION(i, m_Length, (*this)[i] = v); return *this; };

    const Vector<Type> operator+(Type v) const { Vector<Type> vec(*this);  vec += v; return vec; }
    const Vector<Type> operator-(Type v) const { Vector<Type> vec(*this);  vec -= v; return vec; }
    const Vector<Type> operator*(Type v) const { Vector<Type> vec(*this);  vec *= v; return vec; }
    const Vector<Type> operator/(Type v) const { Vector<Type> vec(*this);  vec /= v; return vec; }

    const Vector<Type> operator+(const Vector<Type>& v) const { Vector<Type> vec(*this);  vec += v; return vec; }
    const Vector<Type> operator-(const Vector<Type>& v) const { Vector<Type> vec(*this);  vec -= v; return vec; }
    const Vector<Type> operator*(const Vector<Type>& v) const { Vector<Type> vec(*this);  vec *= v; return vec; }
    const Vector<Type> operator/(const Vector<Type>& v) const { Vector<Type> vec(*this);  vec /= v; return vec; }

    Vector<Type>& operator+=(Type v) { MCON_ITERATION(i, m_Length, (*this)[i] += v); return *this; }
    Vector<Type>& operator-=(Type v) { MCON_ITERATION(i, m_Length, (*this)[i] -= v); return *this; }
    Vector<Type>& operator*=(Type v) { MCON_ITERATION(i, m_Length, (*this)[i] *= v); return *this; }
    Vector<Type>& operator/=(Type v) { MCON_ITERATION(i, m_Length, (*this)[i] /= v); return *this; }

    Vector<Type>& operator+=(const Vector<Type>& v) { MCON_ITERATION(i, Smaller(v.GetLength()), (*this)[i] += v[i]); return *this; }
    Vector<Type>& operator-=(const Vector<Type>& v) { MCON_ITERATION(i, Smaller(v.GetLength()), (*this)[i] -= v[i]); return *this; }
    Vector<Type>& operator*=(const Vector<Type>& v) { MCON_ITERATION(i, Smaller(v.GetLength()), (*this)[i] *= v[i]); return *this; }
    Vector<Type>& operator/=(const Vector<Type>& v) { MCON_ITERATION(i, Smaller(v.GetLength()), (*this)[i] /= v[i]); return *this; }

    inline Matrix<Type> T(void) const { return Transpose(); }
    Matrix<Type> Transpose(void) const
    {
        Matrix<Type> m(GetLength(), 1);
        MCON_ITERATION(i, GetLength(), m[i][0] = (*this)[i]);
        return m;
    }

    int GetLength(void) const { return m_Length; }
    bool IsNull(void) const { return m_Length == 0; }
    bool Resize(int length);

private:
    // Private member functions.
    int    Smaller(int a1, int a2) const { return a1 < a2 ? a1 : a2; }
    int    Smaller(int input) const { return m_Length < input ? m_Length : input; }
    int    Larger(int a1, int a2) const { return a1 > a2 ? a1 : a2; }
    int    Larger(int input) const { return m_Length < input ? input : m_Length ; }
    void   Allocate(void);

    // Private member variables.
    Type*     m_Address;
    int    m_Length;
    Type      m_Zero;
};

template <class Type>
void Vector<Type>::Allocate(void)
{
    m_Address = PTR_CAST(Type*, NULL);
    if (m_Length > 0)
    {
        m_Address = new Type[m_Length];
        ASSERT(NULL != m_Address);
    }
}

template <class Type>
Vector<Type>::Vector(int length)
    : m_Address(NULL),
    m_Length(length),
    m_Zero(0)
{
    Allocate();
}

template <class Type>
Vector<Type>::Vector(const Vector<Type>& v)
  : m_Length(v.GetLength()),
    m_Address(PTR_CAST(Type*, NULL)),
    m_Zero(0)
{
    Allocate();
    MCON_ITERATION(i, m_Length, (*this)[i] = v[i]);
}

template <class Type>
template <typename U>
Vector<Type>::Vector(const Vector<U>& v)
  : m_Length(v.GetLength()),
    m_Address(PTR_CAST(Type*, NULL)),
    m_Zero(0)
{
    Allocate();
    MCON_ITERATION(i, m_Length, (*this)[i] = static_cast<Type>(v[i]));
}

template <class Type>
Vector<Type>::~Vector()
{
    if (NULL != m_Address)
    {
        delete[] m_Address;
        m_Address = PTR_CAST(Type*, NULL);
    }
    m_Length = 0;
}

template <class Type>
const Vector<Type>& Vector<Type>::Copy(const Vector<Type>& v)
{
    MCON_ITERATION(i, Smaller(v.GetLength()), (*this)[i] = v[i]);
    return *this;
}

template <class Type>
Vector<Type>& Vector<Type>::operator=(const Vector<Type>& v)
{
    // m_Length is updated in Resize().
    Resize(v.GetLength());
    MCON_ITERATION(i, v.GetLength(), (*this)[i] = v[i]);
    return *this;
}

template <class Type>
bool Vector<Type>::Resize(int length)
{
    if (length < 0)
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
        m_Address = NULL;
    }
    m_Length = length;
    m_Address = new Type[length];
    ASSERT (NULL != m_Address);
    return true;
}

} // namespace mcon {
