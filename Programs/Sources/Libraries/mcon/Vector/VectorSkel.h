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
public:
    explicit VectorSkel(int length = 0)
        : m_Length(length)
        , m_Address(NULL)
    {}
    ~VectorSkel()
    {
        m_Length = 0;
        m_Address = NULL;
    }
    const Type& operator[](const int i) const
    {
        ASSERT (m_Address != NULL);
        ASSERT (0 <= i && i < GetLength());
        return m_Address[i];
    }

    Type& operator[](const int i)
    {
        ASSERT (m_Address != NULL);
        ASSERT (0 <= i && i < GetLength());
        return m_Address[i];
    }

    inline int GetLength(void) const
    {
        return m_Length;
    }

    inline operator void* () const
    {
        return m_Address;
    }

    inline operator Type* () const
    {
        return m_Address;
    }

    virtual bool Resize(int length)
    {
        if (length < 0)
        {
            return false;
        }
        if (length == GetLength())
        {
            return true;
        }
        if (NULL != m_Address)
        {
            delete[] m_Address;
            m_Address = NULL;
        }
        if (0 < length)
        {
            m_Address = new Type[length];
            if (NULL == m_Address)
            {
                return false;
            }
            m_Length = length;
        }
        return true;
    }
    inline bool IsNull() const
    {
        return m_Address == NULL;
    }
private:
    int m_Length;
    Type* m_Address;
};

} // namespace mcon {
