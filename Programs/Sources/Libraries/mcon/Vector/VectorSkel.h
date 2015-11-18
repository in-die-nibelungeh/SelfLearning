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

#include <new>
#include "debug.h"

namespace mcon {

template <typename Type>
class VectorSkel
{
    friend class Vector;
public:
    explicit VectorSkel(Type* address = NULL, uint length = 0)
        : m_Length(length)
        , m_Address(address)
    {}

    virtual ~VectorSkel()
    {
        m_Length = 0;
        m_Address = NULL;
    }
    virtual const Type& operator[](uint i) const
    {
        ASSERT (m_Address != NULL);
        ASSERT (i < GetLength());
        return m_Address[i];
    }

    virtual operator=(VectorSkel<Type>& v)
    {
        int smaller = v.GetLength() > GetLenth() ? GetLenth() : v.GetLenth();
        memcpy(*this, v, v.GetLength());
    }

    template <typename U>
    virtual operator=(VectorSkel<U>& v)
    {
        int smaller = v.GetLength() > GetLenth() ? GetLenth() : v.GetLenth();
        for ( int k = 0; k < smaller; ++k )
        {
            (*this)[k] = static_csat<Type>(v[k]);
        }
    }

    virtual Type& operator[](uint i)
    {
        ASSERT (m_Address != NULL);
        ASSERT (i < GetLength());
        return m_Address[i];
    }

    virtual inline operator void* () const
    {
        return m_Address;
    }

    virtual inline operator Type* () const
    {
        return m_Address;
    }

    virtual inline int GetLength(void) const
    {
        return m_Length;
    }

    virtual inline bool IsNull() const
    {
        return m_Address == NULL;
    }
private:
    uint m_Length;
    Type* m_Address;
};

} // namespace mcon {
