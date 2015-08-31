#pragma once

#include "types.h"

#include <stdlib.h>

template <class T>
class Buffer
{
    T*     m_Address;
    int    m_NumOfData;
    size_t m_Size;

    int min(int a, int b) const { return (a > b) ? b : a; };

public:
    Buffer(int numData);
    ~Buffer();

    template <class U>
    void Copy(Buffer<U>& b);

    T& operator[](int i)
    {
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

    size_t GetSize     (void) const { return m_Size; }
    int    GetNumOfData(void) const { return m_NumOfData; }
};

template <class T>
Buffer<T>::Buffer(int numData)
    :
    m_Size(numData * sizeof(T)),
    m_Address(reinterpret_cast<T*>(malloc(numData * sizeof(T)))),
    m_NumOfData(numData)
{
}

template <class T>
Buffer<T>::~Buffer()
{
    if (NULL != m_Address)
    {
        free(m_Address);
        m_Address = NULL;
    }
    m_NumOfData = 0;
    m_Size = 0;
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
    if (NULL != this->m_Address)
    {
        free(this->m_Address);
    }
    m_NumOfData = b.GetNumOfData();
    m_Size = m_NumOfData * sizeof(T);
    m_Address = PTR_CAST(T*, malloc(m_Size));

    if (NULL != m_Address)
    {
        for (int i = 0; i < m_NumOfData; ++i)
        {
            (*this)[i] = static_cast<T>(b[i]);
        }
    }
    return *this;
}
