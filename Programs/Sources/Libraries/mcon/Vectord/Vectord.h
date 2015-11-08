/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2015 Ryosuke Kanata
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

#pragma once

#include <string.h>

#include "debug.h"
#include "Vector.h"
#include "VectordBase.h"

namespace mcon {

class Vectord : public VectordBase
{
public:

    explicit Vectord(int length = 0)
        : VectordBase(length)
        , m_AddressBase(NULL)
    {
        ASSERT(0 <= length);
        bool status = Allocate();
        UNUSED(status);
        ASSERT(status == true);
    }

    Vectord(const Vectord& v)
        : VectordBase(v.GetLength())
        , m_AddressBase(NULL)
    {
        bool status = Allocate();
        UNUSED(status);
        ASSERT(status == true);
        memcpy(m_AddressAligned, v, GetLength() * sizeof(double));
    }

    Vectord(const VectordBase& v)
        : VectordBase(v.GetLength())
        , m_AddressBase(NULL)
    {
        bool status = Allocate();
        UNUSED(status);
        ASSERT(status == true);
        memcpy(m_AddressAligned, v, GetLength() * sizeof(double));
    }

    template <typename U> Vectord(const Vector<U>& v)
        : VectordBase(v.GetLength())
        , m_AddressBase(NULL)
    {
        bool status = Allocate();
        UNUSED(status);
        ASSERT(status == true);
        for ( int i = 0; i < GetLength(); ++i )
        {
            (*this)[i] = static_cast<double>(v[i]);
        }
    }
    ~Vectord();

    // operator= make the same Vectord as the input Vectord.
    Vectord& operator=(const Vectord& Vectord);
    Vectord& operator=(double v)
    {
        *dynamic_cast<VectordBase*>(this) = v;
        return *this;
    }

    Vectord operator()(int offset, int length) const;
    bool Resize(int length);

    // Inline functions.
    inline bool IsNull(void) const
    {
        return m_AddressBase == NULL;
    }

private:
    // Private member functions.
    bool  Allocate(void);
    double* Aligned(double* ptr, int align);

    // Private member variables.
    double*  m_AddressBase;
};

} // namespace mcon {
