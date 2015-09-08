#ifndef _DEBUG_H_
#define _DEBUG_H_

#include <stdio.h>

#if defined(DEBUG)

#define LOG(...) printf(__VA_ARGS__)
//#define DEBUG_LOG(fmt, ...) printf("%s [%s at %d]: " fmt "\n", __func__, __FILE__, __LINE__, __VA_ARGS__)

#define TOSTR(n) TOSTR_(n)
#define TOSTR_(n) #n
#define DEBUG_LOG(...) printf("[" __FILE__ " at " TOSTR(__LINE__) "]: " __VA_ARGS__)

#define ERROR_LOG(...) fprintf(stderr, __VA_ARGS__)

#define CHECK(cond)                \
    do {                           \
        char r[3] = "OK";          \
        if (!(cond))               \
        {                          \
            r[0] = 'N';            \
            r[1] = 'G';            \
        }                          \
        printf(#cond ": %s\n", r); \
    } while (0)


#define CHECK_VALUE_INT(var, ans)  \
    DEBUG_LOG("[%s] " #var"=%d (ans=%d)\n", \
        (var)==(ans) ? "OK" : "NG", var, ans)

#define CHECK_VALUE_FLT(var, ans)  \
    DEBUG_LOG("[%s] " #var"=%f (ans=%f)\n", \
        (var)==(ans) ? "OK" : "NG", var, ans)

#ifndef ASSERT
#define ASSERT(c) \
    do { if (!(c)) \
    {         \
        printf("ASSERT at %d in %s: " #c "\n", __LINE__, __FILE__); \
        *((int*)0) = 0; \
    } } while (0)

#endif // #ifndef ASSERT

#else  // #if defined(DEBUG)

#define LOG(...) printf(__VA_ARGS__)
#define DEBUG_LOG(...)
#define ERROR_LOG(...)
#define CHECK(cond)
#define ASSERT(c)
#define CHECK_VALUE_INT(var, ans)
#define CHECK_VALUE_DBL(var, ans)

#endif // #if defined(DEBUG)

#endif // #ifndef _DEBUG_H_
