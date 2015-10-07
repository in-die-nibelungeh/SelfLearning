#pragma once

#include "types.h"
#include "debug.h"

namespace mcon {

template <typename Type> class Matrix;

#define MCON_ITERATION(var, iter, statement)  \
    do {                                      \
        for (int var = 0; var < iter; ++var)  \
        {                                     \
            statement;                        \
        }                                     \
    } while(0)

template <typename Type>
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

    template <typename U>
    operator Vector<U>() const
    {
        Vector<U> v(GetLength());
        MCON_ITERATION(i, m_Length, v[i] = static_cast<U>((*this)[i]));
        return v;
    }
    // This cast doesn't seem called... Why?
    // Are default ones already defined and called?
    operator Vector<Type>() const
    {
        ASSERT(0);
        Vector<Type> v(GetLength());
        MCON_ITERATION(i, m_Length, v[i] = (*this)[i]);
        return v;
    }

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
        for (int i = 0; i < GetLength() - 1; ++i)
        {
            (*this)[i] = (*this)[i+1];
        }
        (*this)[GetLength()-1] = v;
        return ret;
    }

    Type Unshift(Type v)
    {
        Type ret = (*this)[GetLength()-1];
        for (int i = GetLength() - 1; i > 0; --i)
        {
            (*this)[i] = (*this)[i-1];
        }
        (*this)[0] = v;
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
    Matrix<Type> ToMatrix(void) const
    {
        Matrix<Type> m(1, GetLength());
        m[0] = *this;
        return m;
    }

    inline Type GetMaximum(void) const
    {
        Type max = (*this)[0];
        for (int i = 1; i < GetLength(); ++i)
        {
            if (max < (*this)[i])
            {
                max = (*this)[i];
            }
        }
        return max;
    }

    inline Type GetMaximumAbsolute(void) const
    {
        Type max = Absolute((*this)[0]);
        for (int i = 1; i < GetLength(); ++i)
        {
            const Type v = Absolute((*this)[i]);
            if (max < v)
            {
                max = v;
            }
        }
        return max;
    }

    inline Type GetMinimum(void) const
    {
        Type min = (*this)[0];
        for (int i = 1; i < GetLength(); ++i)
        {
            if (min > (*this)[i])
            {
                min = (*this)[i];
            }
        }
        return min;
    }

    inline Type GetMinimumAbsolute(void) const
    {
        Type min = Absolute((*this)[0]);
        for (int i = 1; i < GetLength(); ++i)
        {
            const Type v = Absolute((*this)[i]);
            if (min > v)
            {
                min = v;
            }
        }
        return min;
    }


    inline Type GetSum(void) const
    {
        Type sum = 0;
        MCON_ITERATION( i, GetLength(), sum += (*this)[i] );
        return sum;
    }

    inline double GetAverage(void) const
    {
        return GetSum()/GetLength();
    }

    inline double GetNorm(void) const
    {
        double squareSum = 0;
        MCON_ITERATION( i, GetLength(), squareSum += (*this)[i] * (*this)[i]);
        return sqrt(squareSum);
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
    Type   Absolute(Type v) const { return (v < 0) ? -v : v; }
    // Private member variables.
    Type*  m_Address;
    int    m_Length;
    Type   m_Zero;
};

template <typename Type>
void Vector<Type>::Allocate(void)
{
    m_Address = PTR_CAST(Type*, NULL);
    if (m_Length > 0)
    {
        m_Address = new Type[m_Length];
        ASSERT(NULL != m_Address);
    }
}

template <typename Type>
Vector<Type>::Vector(int length)
    : m_Address(NULL),
    m_Length(length),
    m_Zero(0)
{
    Allocate();
}

template <typename Type>
Vector<Type>::Vector(const Vector<Type>& v)
  : m_Length(v.GetLength()),
    m_Address(PTR_CAST(Type*, NULL)),
    m_Zero(0)
{
    Allocate();
    MCON_ITERATION(i, m_Length, (*this)[i] = v[i]);
}

template <typename Type>
template <typename U>
Vector<Type>::Vector(const Vector<U>& v)
  : m_Length(v.GetLength()),
    m_Address(PTR_CAST(Type*, NULL)),
    m_Zero(0)
{
    Allocate();
    MCON_ITERATION(i, m_Length, (*this)[i] = static_cast<Type>(v[i]));
}

template <typename Type>
Vector<Type>::~Vector()
{
    if (NULL != m_Address)
    {
        delete[] m_Address;
        m_Address = PTR_CAST(Type*, NULL);
    }
    m_Length = 0;
}

template <typename Type>
const Vector<Type>& Vector<Type>::Copy(const Vector<Type>& v)
{
    MCON_ITERATION(i, Smaller(v.GetLength()), (*this)[i] = v[i]);
    return *this;
}

template <typename Type>
Vector<Type>& Vector<Type>::operator=(const Vector<Type>& v)
{
    // m_Length is updated in Resize().
    Resize(v.GetLength());
    MCON_ITERATION(i, v.GetLength(), (*this)[i] = v[i]);
    return *this;
}

template <typename Type>
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
