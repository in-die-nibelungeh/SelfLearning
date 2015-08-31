#pragma once

#include "types.h"

#include <stdlib.h>

#define DECL_CAST_OPERATOR(t) \
    operator t*() { return PTR_CAST(t*, m_Address); }

//template <class T>
class Buffer
{
    void* m_Address;
    size_t m_Size;

public:
    Buffer();
    Buffer(size_t size);
    ~Buffer();

/*
    DECL_CAST_OPERATOR(f64*);
    DECL_CAST_OPERATOR(f32*);
    DECL_CAST_OPERATOR(s32*);
    DECL_CAST_OPERATOR(u32*);
    DECL_CAST_OPERATOR(s16*);
    DECL_CAST_OPERATOR(u16*);
    DECL_CAST_OPERATOR(s8*);
    DECL_CAST_OPERATOR(u8*);
*/

    int& operator[](int i) { return PTR_CAST(int*, m_Address)[i]; }
    f64& operator[](f64 i) { return PTR_CAST(f64*, m_Address)[static_cast<int>(i)]; }
    size_t GetSize(void) const { return m_Size; }
};

#if 1
template <class T>
class TBuffer
{
    T* m_Address;
    size_t m_Size;

public:
    TBuffer();
    TBuffer(size_t size);
    ~TBuffer();

    T& operator[](int i) { return m_Address[i]; }
    size_t GetSize(void) const { return m_Size; }

};

template <class T>
TBuffer<T>::TBuffer() : m_Size(0), m_Address(NULL)
{}

template <class T>
TBuffer<T>::TBuffer(size_t size) : m_Size(size), m_Address(reinterpret_cast<T*>(malloc(size)))
{}

template <class T>
TBuffer<T>::~TBuffer()
{
    if (NULL != m_Address)
    {
        free(m_Address);
    }
}
#endif
