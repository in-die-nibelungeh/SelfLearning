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


#define CHECK_VALUE(var, ans)  \
    DEBUG_LOG("[%s] " #var"=%g (ans=%g)\n", \
        (var)==(ans) ? "OK" : "NG", static_cast<double>(var), static_cast<double>(ans))

#define CHECK_VALUE_INT(var, ans)  \
    DEBUG_LOG("[%s] " #var"=%d (ans=%d)\n", \
        (var)==(ans) ? "OK" : "NG", var, static_cast<int>(ans))

#define CHECK_VALUE_FLT(var, ans)  \
    DEBUG_LOG("[%s] " #var"=%f (ans=%f)\n", \
        (var)==(ans) ? "OK" : "NG", var, static_cast<double>(ans))

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
