#include "Buffer.h"

#include <stdlib.h>

Buffer::Buffer(size_t size) : m_Size(size), m_Address(malloc(size))
{
}

Buffer::Buffer() : m_Size(0), m_Address(NULL)
{
}

Buffer::~Buffer()
{
    if (NULL != m_Address)
    {
        free(m_Address);
    }
}
