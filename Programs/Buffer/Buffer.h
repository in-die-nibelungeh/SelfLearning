#pragma once

#include "types.h"
#include "debug.h"

// To use malloc/free
#include <stdlib.h>

namespace Container {

template <class T>
class Buffer
{
    T*     m_Address;
    int    m_NumOfData;

    inline int min(int a, int b) const { return (a > b) ? b : a; };

public:
    template <class U>
    Buffer(Buffer<U>& );

    Buffer(int numData);
    ~Buffer();

    template <class U>
    void Copy(Buffer<U>& b);

    T& operator[](int i) const
    {
        ASSERT(0 < m_NumOfData);
        ASSERT(NULL != m_Address);
        if (0 > i)
        {
            return m_Address[0];
        }
        if (i >= m_NumOfData)
        {
            return m_Address[m_NumOfData-1];
        }
        return m_Address[i];
    }

    template <class U>
    Buffer<T>& operator=(Buffer<U>& b);

    intGetNumOfData(void) const
    {
        return m_NumOfData;
    }
};

template <class T>
Buffer<T>::Buffer(int numData)
    :
    m_Address(PTR_CAST(T*, malloc(numData * sizeof(T)))),
    m_NumOfData(numData)
{
    ASSERT(NULL != m_Address);
}

template <class T>
template <class U>
Buffer<T>::Buffer(Buffer<U>& b)
  : m_Address(NULL)
{
    *this = b;
}

template <class T>
Buffer<T>::~Buffer()
{
    ASSERT(NULL != m_Address);
    free(m_Address);
    m_Address = NULL;
}

template <class T>
template <class U>
void Buffer<T>::Copy(Buffer<U>& b)
{
    int iter = this->min(this->m_NumOfData, b.GetNumOfData());
    for (int i = 0; i < iter; ++i)
    {
        (*this)[i] = static_cast<T>(b[i]);
    }
}

template <class T>
template <class U>
Buffer<T>& Buffer<T>::operator=(Buffer<U>& b)
{
    ASSERT(NULL != m_Address);
    free(this->m_Address);

    m_NumOfData = b.GetNumOfData();
    m_Address = PTR_CAST(T*, malloc(m_NumOfData * sizeof(T)));

    ASSERT(NULL != m_Address);

    for (int i = 0; i < m_NumOfData; ++i)
    {
        (*this)[i] = static_cast<T>(b[i]);
    }
    return *this;
}

} // namespace Container {
