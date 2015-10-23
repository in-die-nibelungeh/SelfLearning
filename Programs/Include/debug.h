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

#include <stdio.h>

#define TOSTR(n) TOSTR_(n)
#define TOSTR_(n) #n

#if defined(DEBUG)

#define DEBUG_LOG(...) printf("[" __FILE__ " at " TOSTR(__LINE__) "]: " __VA_ARGS__)

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


#ifndef ASSERT
#define ASSERT(c) \
    do { if (!(c)) \
    {         \
        printf("ASSERT at %d in %s: " #c "\n", __LINE__, __FILE__); \
        *((int*)0) = 0; \
    } } while (0)

#endif // #ifndef ASSERT

#else  // #if defined(DEBUG)

#define DEBUG_LOG(...)
#define CHECK(cond)
#define ASSERT(c)

#endif // #if defined(DEBUG)

#define LOG(...) printf(__VA_ARGS__)
#define ERROR_LOG(...) fprintf(stderr, "[" __FILE__ " at " TOSTR(__LINE__) "]: " __VA_ARGS__)

#define CHECK_VALUE(var, ans)  \
    LOG("[%s] " #var"=%g (ans=%g)\n", \
        (var)==(ans) ? "OK" : "NG", static_cast<double>(var), static_cast<double>(ans))

#define UNUSED(v) (void)(v)
