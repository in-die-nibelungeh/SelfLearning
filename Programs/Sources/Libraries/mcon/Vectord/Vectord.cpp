/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2015-2016 Ryosuke Kanata
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include <string>
#include <cstring>

#include "debug.h"
#include "mcon.h"

namespace mcon {

Vector<double>::~Vector<double>()
{
    if (NULL != m_AddressBase)
    {
        delete[] m_AddressBase;
    }
}

const Vector<double> Vector<double>::operator()(uint offset, uint length) const
{
    Vector<double> carveout;
    if (GetLength() <= offset)
    {
        // Null object.
        return carveout;
    }
    // Smaller value as length
    carveout.Resize( std::min(GetLength() - offset, length) );
    for (uint i = offset; i < std::min(GetLength(), offset + length); ++i)
    {
        carveout[i-offset] = (*this)[i];
    }
    return carveout;
}


Vector<double>& Vector<double>::operator=(const Vector<double>& v)
{
    // m_Length is updated in Resize().
    const uint n = v.GetLength();
    Resize(n);
    std::memcpy(*this, v, n * sizeof(double));
    return *this;
}

const Vector<double> Vector<double>::operator+(double v) const { Vector<double> vec(*this);  vec += v; return vec; }
const Vector<double> Vector<double>::operator-(double v) const { Vector<double> vec(*this);  vec -= v; return vec; }
const Vector<double> Vector<double>::operator*(double v) const { Vector<double> vec(*this);  vec *= v; return vec; }
const Vector<double> Vector<double>::operator/(double v) const { Vector<double> vec(*this);  vec /= v; return vec; }

const Vector<double> Vector<double>::operator+(const VectordBase& v) const { Vector<double> vec(*this);  vec += v; return vec; }
const Vector<double> Vector<double>::operator-(const VectordBase& v) const { Vector<double> vec(*this);  vec -= v; return vec; }
const Vector<double> Vector<double>::operator*(const VectordBase& v) const { Vector<double> vec(*this);  vec *= v; return vec; }
const Vector<double> Vector<double>::operator/(const VectordBase& v) const { Vector<double> vec(*this);  vec /= v; return vec; }

double* Align(double* ptr, int align)
{
    const unsigned char* aligned = reinterpret_cast<const unsigned char*>(ptr);
    while (reinterpret_cast<long unsigned int>(aligned) % align) { ++aligned; }
    return const_cast<double*>(reinterpret_cast<const double*>(aligned));
}

bool Vector<double>::Allocate(void)
{
    m_AddressBase = NULL;
    m_AddressAligned = NULL;
    if (m_Length > 0)
    {
        m_AddressBase = new double[m_Length + g_Alignment/sizeof(double)];
        if (NULL == m_AddressBase)
        {
            return false;
        }
        m_AddressAligned = Align(m_AddressBase, g_Alignment);
    }
    return true;
}

const Matrix<double> Vector<double>::ToMatrix(void) const
{
    Matrix<double> m(1, GetLength());
    m[0] = *this;
    return m;
}

bool Vector<double>::Resize(uint length)
{
    if (length == m_Length)
    {
        return true;
    }
    if (NULL != m_AddressBase)
    {
        delete[] m_AddressBase;
        m_AddressBase = NULL;
    }
    m_Length = length;
    return Allocate();
}

} // namespace mcon {
