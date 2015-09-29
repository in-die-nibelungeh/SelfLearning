/*
dot product (SIMD)

VC++ : cl /Ox /wd4752 sdot_simd.c
gcc  : gcc -O3 -mavx sdot_simd.c -o sdot_simd

Usage:
> sdot_simd <num> <loop> [sse|avx]
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <immintrin.h>

static float sdot(int simd, int n, const float *a, const float *b)
{
    int    i;
    int    ne = 0;
    float  sum = 0;

    if      (simd == 1) {
        // SSE
#if defined(__GNUC__)
        __attribute__((aligned(16))) float fsum[4];
#else // _WIN32
        __declspec(align(16)) float fsum[4];
#endif
        __m128 vsum;
        ne = (n / 4) * 4;
        vsum = _mm_setzero_ps();
        for (i = 0; i < ne; i += 4) {
            vsum = _mm_add_ps(vsum,
                   _mm_mul_ps(_mm_load_ps(a + i),
                              _mm_load_ps(b + i)));
        }
        _mm_store_ps(fsum, vsum);
        sum = fsum[0] + fsum[1] + fsum[2] + fsum[3];
    }
    else if (simd == 2) {
        // AVX
#if defined(__GNUC__)
        __attribute__((aligned(32))) float fsum[8];
#else // _WIN32
        __declspec(align(32)) float fsum[8];
#endif
        __m256 vsum;
        ne = (n / 8) * 8;
        vsum = _mm256_setzero_ps();
        for (i = 0; i < ne; i += 8) {
            vsum = _mm256_add_ps(vsum,
                   _mm256_mul_ps(_mm256_load_ps(a + i),
                                 _mm256_load_ps(b + i)));
        }
        _mm256_store_ps(fsum, vsum);
        sum = fsum[0] + fsum[1] + fsum[2] + fsum[3]
            + fsum[4] + fsum[5] + fsum[6] + fsum[7];
    }

    for (i = ne; i < n; i++) {
        sum += a[i] * b[i];
    }

    return sum;
}

int main(int argc, char **argv)
{
    int    simd = 0;
    int    n = 1000;
    int    loop = 1000;
    int    i;
    clock_t t0, t1;
    size_t size;
    float  *a, *b;
    double sum, exact;

    // arguments
    if (argc >= 3) {
        n = atoi(argv[1]);
        loop = atoi(argv[2]);
    }
    if (argc >= 4) {
        if      (!strcmp(argv[3], "sse")) {
            simd = 1;
        }
        else if (!strcmp(argv[3], "avx")) {
            simd = 2;
        }
    }

    // alloc
    if      (simd == 1) {
        size = ((n + 3) / 4) * 4 * sizeof(float);
        a = (float *)_mm_malloc(size, 16);
        b = (float *)_mm_malloc(size, 16);
    }
    else if (simd == 2) {
        size = ((n + 7) / 8) * 8 * sizeof(float);
        a = (float *)_mm_malloc(size, 32);
        b = (float *)_mm_malloc(size, 32);
    }
    else {
        size = n * sizeof(float);
        a = (float *)malloc(size);
        b = (float *)malloc(size);
    }

    // setup
    for (i = 0; i < n; i++) {
        a[i] = i + 1.0f;
        b[i] = i + 1.0f;
    }

    // timer
    t0 = clock();

    // calculation
    sum = 0;
    for (i = 0; i < loop; i++) {
        sum += sdot(simd, n, a, b);
    }

    // timer
    t1 = clock();

    // output
    exact = (double)loop * n * (n + 1) * (2 * n + 1) / 6.0;
    printf("n = %d, loop = %d, dot = %.6e(%.6e), cpu[sec] = %.3f\n",
        n, loop, sum, exact, (double)(t1 - t0) / CLOCKS_PER_SEC);

    // free
    if (simd) {
        _mm_free(a);
        _mm_free(b);
    }
    else {
        free(a);
        free(b);
    }

    return 0;
}
