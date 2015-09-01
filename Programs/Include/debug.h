#ifndef _DEBUG_H_
#define _DEBUG_H_

#include <stdio.h>

#if defined(DEBUG)

#define LOG(...) printf(__VA_ARGS__)
#define DEBUG_LOG(...) printf(__VA_ARGS__)

#define CHECK(cond)                \
    {                              \
        char r[3] = "OK";          \
        if (!(cond))               \
        {                          \
            r[0] = 'N';            \
            r[1] = 'G';            \
        }                          \
        printf(#cond ": %s\n", r); \
    }


#ifndef ASSERT
#define ASSERT(c) \
    if (!(c)) \
    {         \
        printf("ASSERT at %d in %s: " #c "\n", __LINE__, __FILE__); \
        *((int*)0) = 0; \
    }
#endif // #ifndef ASSERT

#else  // #if defined(DEBUG)

#define LOG(...) printf(__VA_ARGS__)
#define DEBUG_LOG(...)
#define CHECK(cond)
#define ASSERT(c)

#endif // #if defined(DEBUG)

#endif // #ifndef _DEBUG_H_
