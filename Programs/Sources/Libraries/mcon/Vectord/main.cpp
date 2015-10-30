#include <stdio.h>
#include <math.h>

#include <x86intrin.h>

#include "Vectord.h"

extern void test_VectordBase(void);
extern void test_Vectord(void);
extern void benchmark_Vectord();

//#include "tune.cpp"


#define EFLAG (1LL<<63)

static const __m256i g_RemainMask[3] =
{
    {EFLAG, 0, 0, 0},
    {EFLAG, EFLAG, 0, 0},
    {EFLAG, EFLAG, EFLAG, 0}
};

float dot_avx_2(const double *vec1, const double *vec2, unsigned n)
{
    __m256d u1 = {0};
    __m256d u2 = {0};
    for(unsigned int i = 0; i < n; i += 8)
    {
        __m256d w1 = _mm256_load_pd(&vec1[i]);
        __m256d w2 = _mm256_load_pd(&vec1[i + 4]);
        __m256d x1 = _mm256_load_pd(&vec2[i]);
        __m256d x2 = _mm256_load_pd(&vec2[i + 4]);

        x1 = _mm256_mul_pd(w1, x1);
        x2 = _mm256_mul_pd(w2, x2);
        u1 = _mm256_add_pd(u1, x1);
        u2 = _mm256_add_pd(u2, x2);
    }
    u1 = _mm256_add_pd(u1, u2);

    double __attribute__((aligned(32))) t[4] = {0};
    _mm256_store_pd(t, u1);
    return t[0] + t[1] + t[2] + t[3];
}

double GetMinimum(const double* ptr, int length)
{
    const int unit = 4;
    const double* pEr = ptr;
    double __attribute((aligned(32))) min = 0;

    if (length < unit*2)
    {
        __m128d _min = _mm_load_sd(pEr);
        for ( ++pEr ; pEr < ptr + length; ++pEr )
        {
            __m128d v = _mm_load_sd(pEr);
            _min = _mm_min_sd(_min, v);
        }
        _mm_store_pd(&min, _min);
    }
    else
    {
        const int units = (length / unit) * unit;
        __m256d minv = _mm256_loadu_pd(ptr + length - unit);
        for ( ; pEr < ptr + units; pEr += unit )
        {
            __m256d xv = _mm256_load_pd(pEr);
            minv = _mm256_min_pd(minv, xv);
        }
        double __attribute((aligned(32))) mins[unit];
        _mm256_store_pd(mins, minv);
        __m128d _min = _mm_load_sd(mins);
        for ( int i = 1; i < unit; ++i )
        {
            __m128d v = _mm_load_sd(mins+i);
            _min = _mm_min_sd(_min, v);
        }
        _mm_store_pd(&min, _min);
    }
    return min;
}

double GetMaximum(const double* ptr, int length)
{
    const int unit = 4;
    const double* pEr = ptr;
    double __attribute((aligned(32))) max = 0;

    if (length < unit*2)
    {
        __m128d _max = _mm_load_sd(pEr);
        for ( ++pEr ; pEr < ptr + length; ++pEr )
        {
            __m128d v = _mm_load_sd(pEr);
            _max = _mm_max_sd(_max, v);
        }
        _mm_store_pd(&max, _max);
    }
    else
    {
        const int units = (length / unit) * unit;
        __m256d maxv = _mm256_loadu_pd(ptr + length - unit);
        for ( ; pEr < ptr + units; pEr += unit )
        {
            __m256d xv = _mm256_load_pd(pEr);
            maxv = _mm256_max_pd(maxv, xv);
        }
        double __attribute((aligned(32))) maxs[unit] = {0};
        _mm256_store_pd(maxs, maxv);
        __m128d _max = _mm_load_sd(maxs);
        for ( int i = 1; i < unit; ++i )
        {
            __m128d v = _mm_load_sd(maxs+i);
            _max = _mm_max_sd(_max, v);
        }
        _mm_store_pd(&max, _max);
    }
    return max;
}

double GetMaximumAbsolute(const double* ptr, int length)
{
    const int unit = 4;
    const double* pEr = ptr;
    double __attribute((aligned(32))) maxAbs;

    if (length < unit * 2)
    {
        const __m128d mask = {-0.0f, -0.0f};
        __m128d _maxabs = _mm_load_sd(pEr);
        _maxabs = _mm_andnot_pd(mask, _maxabs);
        for ( ++pEr ; pEr < ptr + length ; ++pEr )
        {
            __m128d xv = _mm_load_sd(pEr);
            xv = _mm_andnot_pd(mask, xv);
            _maxabs = _mm_max_sd(_maxabs, xv);
        }
        _mm_store_sd(&maxAbs, _maxabs);
    }
    else
    {
        const __m256d mask = {-0.0f, -0.0f, -0.0f, -0.0f};
        const int units = (length / unit) * unit;
        __m256d maxabsv = _mm256_loadu_pd(ptr + length - unit);
        maxabsv = _mm256_andnot_pd(mask, maxabsv);

        for ( ; pEr < ptr + units ; pEr += unit )
        {
            __m256d xv = _mm256_load_pd(pEr);
            xv = _mm256_andnot_pd(mask, xv);
            maxabsv = _mm256_max_pd(maxabsv, xv);
        }
        double __attribute((aligned(32))) maxabss[4];
        _mm256_store_pd(maxabss, maxabsv);
        __m128d _maxabs = _mm_load_sd(maxabss);
        for ( int i = 1; i < unit; ++i )
        {
            __m128d v = _mm_load_sd(maxabss+i);
            _maxabs = _mm_max_sd(_maxabs, v);
        }
        _mm_store_pd(&maxAbs, _maxabs);
    }
    return maxAbs;
}

double GetMinimumAbsolute(const double* ptr, int length)
{
    const int unit = 4;
    const double* pEr = ptr;
    double __attribute((aligned(32))) minAbs;

    if (length < unit * 2)
    {
        const __m128d mask = {-0.0f, -0.0f};
        __m128d _minabs = _mm_load_sd(pEr);
        _minabs = _mm_andnot_pd(mask, _minabs);
        for ( ++pEr ; pEr < ptr + length ; ++pEr )
        {
            __m128d xv = _mm_load_sd(pEr);
            xv = _mm_andnot_pd(mask, xv);
            _minabs = _mm_min_sd(_minabs, xv);
        }
        _mm_store_sd(&minAbs, _minabs);
    }
    else
    {
        const __m256d mask = {-0.0f, -0.0f, -0.0f, -0.0f};
        const int units = (length / unit) * unit;
        __m256d minabsv = _mm256_loadu_pd(ptr + length - unit);
        minabsv = _mm256_andnot_pd(mask, minabsv);

        for ( ; pEr < ptr + units ; pEr += unit )
        {
            __m256d xv = _mm256_load_pd(pEr);
            xv = _mm256_andnot_pd(mask, xv);
            minabsv = _mm256_min_pd(minabsv, xv);
        }
        double __attribute((aligned(32))) minabss[4];
        _mm256_store_pd(minabss, minabsv);
        __m128d _minabs = _mm_load_sd(minabss);
        for ( int i = 1; i < unit; ++i )
        {
            __m128d v = _mm_load_sd(minabss+i);
            _minabs = _mm_min_sd(_minabs, v);
        }
        _mm_store_pd(&minAbs, _minabs);
    }
    return minAbs;
}

double GetSum(const double* ptr, int length)
{
    ASSERT_ALIGNED(ptr, 32);
    const int unit = 4;
    const double* pEr = ptr;
    double sum = 0;
    if ( length < unit*2 )
    {
        for ( ; pEr < ptr + length; ++pEr )
        {
            sum += *pEr;
        }
    }
    else
    {
        const int units = (length / unit) * unit;
        __m256d accv = _mm256_load_pd(pEr);
        for ( pEr += unit ; pEr < ptr + units; pEr += unit )
        {
            const __m256d xv = _mm256_load_pd(pEr);
            accv = _mm256_add_pd(xv, accv);
        }
        const int remain = length - units;

        if (remain > 0)
        {
            const __m256d xv = _mm256_maskload_pd(pEr, g_RemainMask[remain-1]);
            accv = _mm256_add_pd(xv, accv);
        }
        double __attribute((aligned(32))) accs[4] = {0};
        _mm256_store_pd(accs, accv);
        sum = accs[0] + accs[1] + accs[2] + accs[3];
    }
    return sum;
}

double GetNorm(const double* ptr, int length)
{
    ASSERT_ALIGNED(ptr, 32);
    const int unit = 4;
    const double* pEr = ptr;
    double norm = 0;
    if ( length < unit * 2 )
    {
        for ( ; pEr < ptr + length; ++pEr )
        {
            norm += *pEr * *pEr;
        }
        norm = sqrt(norm);
    }
    else
    {
        const int units = (length / unit) * unit;
        __m256d normv = _mm256_load_pd(pEr);
        normv = _mm256_mul_pd(normv, normv);
        for ( pEr += unit ; pEr < ptr + units; pEr += unit )
        {
            __m256d xv = _mm256_load_pd(pEr);
            xv = _mm256_mul_pd(xv, xv);
            normv = _mm256_add_pd(normv, xv);
        }
        const int remain = length - units;
        if (remain > 0)
        {
            __m256d xv = _mm256_maskload_pd(pEr, g_RemainMask[remain-1]);
            xv = _mm256_mul_pd(xv, xv);
            normv = _mm256_add_pd(normv, xv);
        }
        double __attribute((aligned(32))) norms[unit];
        _mm256_store_pd(norms, normv);
        norm = sqrt(norms[0] + norms[1] + norms[2] + norms[3]);
    }
    return norm;
}

double GetDotProduct(const double* _this, const double* ptr, int length)
{
    const int unit = 4;
    const double* pEr = ptr;
    const double* pEe = _this;
    const int units = (length / unit) * unit;

    __m256d accv = {0, 0, 0, 0};
    for ( ; pEr < ptr + units; pEr += unit, pEe += unit )
    {
        __m256d xv = _mm256_load_pd(pEr);
        __m256d yv = _mm256_load_pd(pEe);
        xv = _mm256_mul_pd(xv, yv);
        accv = _mm256_add_pd(xv, accv);
    }
    const int remain = length - units;
    if (remain > 0)
    {
        __m256d xv = _mm256_maskload_pd(pEr, g_RemainMask[remain-1]);
        __m256d yv = _mm256_maskload_pd(pEe, g_RemainMask[remain-1]);
        xv = _mm256_mul_pd(xv, yv);
        accv = _mm256_add_pd(xv, accv);
    }
    double __attribute((aligned(32))) accs[4];
    _mm256_store_pd(accs, accv);
    return accs[0] + accs[1] + accs[2] + accs[3];
}


int main(void)
{
#if 0
    // dot
    {
        const int len = 16;
        double *p1 = new __attribute__((aligned(32))) double[len + 16];
        double *p2 = new __attribute__((aligned(32))) double[len + 16];
        double *vec1 = p1;
        double *vec2 = p2;
        while(reinterpret_cast<long>(vec1) % 32) ++vec1;
        while(reinterpret_cast<long>(vec2) % 32) ++vec2;

        double sum = 0;
        for ( int i = 0; i < len; ++i )
        {
            p1[i] = i + 1;
            p2[i] = i + 1;
            sum += p1[i] * p2[i];
            printf("p[%d]=%g\n", i, p1[i]);
        }
        printf("[sum]=%g\n", sum);
        CHECK_VALUE(dot_avx_2(p1, p2, len), sum);
    }
    // operator=
    {
        printf("operator=\n");
        const double __attribute((aligned(32))) v = 1.0;
        double __attribute((aligned(32))) r[4] = {0};
        const __m256d y = _mm256_broadcast_sd(&v);
        _mm256_store_pd(r, y);
        for ( int i = 0; i < 4; ++i )
        {
            CHECK_VALUE(r[i], 1.0);
        }
    }
    // operator+=
    {
        printf("operator+=\n");
        const int unit = 4;
        const int length = 17;
        double __attribute((aligned(32))) x[length];
        double __attribute((aligned(32))) y[length];
        for ( int i = 0 ; i < length; ++i )
        {
            x[i] = i + 1;
            y[i] = (i + 1) * 10;
        }
        int done = 0;
        double* pEr = x;
        double* pEe = y;
        for ( ; done < (length/unit) * unit; done += unit, pEr += unit, pEe += unit )
        {
            __m256d xv = _mm256_load_pd(pEr);
            __m256d yv = _mm256_load_pd(pEe);
            xv = _mm256_add_pd(xv, yv);
            _mm256_store_pd(pEr, xv);
        }
        for ( int i = 0; i < length - done ; ++i )
        {
            pEr[i] += pEe[i];
        }

        for ( int i = 0; i < length; ++i )
        {
            CHECK_VALUE(x[i], i + 1 + (i+1)*10);
        }
    }
    // max
    {
        printf("\n[max]\n");
        const int n = 17;
        double __attribute((aligned(32))) x[n];
        double max;
        const int limits[] = {n, 1, 2, 3, 4, 5, 6, 7, 8, 9};

        for ( unsigned int k = 0 ; k < sizeof(limits)/sizeof(int); ++k )
        {
            const int limit = limits[k];
            max = __DBL_MIN__;
            for ( int i = 0 ; i < limit ; ++i )
            {
                x[i] = n - limit + i + 1;
                if ( max < x[i] )
                {
                    max = x[i];
                }
            }
            printf("limit=%d\n", limit);
            CHECK_VALUE(max, GetMaximum(x, limit));
        }
    }
    // min
    {
        printf("\n[min]\n");
        const int n = 17;
        double __attribute((aligned(32))) x[n];
        double min;
        const int limits[] = {n, 1, 2, 3, 4, 5, 6, 7, 8, 9};

        for ( unsigned int k = 0 ; k < sizeof(limits)/sizeof(int); ++k )
        {
            const int limit = limits[k];
            min = __DBL_MAX__;
            for ( int i = 0 ; i < limit ; ++i )
            {
                x[i] = n - limit + i + 1;
                if ( min > x[i] )
                {
                    min = x[i];
                }
            }
            printf("limit=%d\n", limit);
            CHECK_VALUE(min, GetMinimum(x, limit));
        }
    }

    // max absolute
    {
        printf("\n[max absolute]\n");
        const int n = 17;
        double __attribute((aligned(32))) x[n];
        double ans;
        const int limits[] = {n, 1, 2, 3, 4, 5, 6, 7, 8, 9};

        for ( unsigned int k = 0 ; k < sizeof(limits)/sizeof(int); ++k )
        {
            const int limit = limits[k];
            ans = 0;
            for ( int i = 0 ; i < limit ; ++i )
            {
                x[i] = -(n - limit + i + 1);
                double temp = x[i];
                if (temp < 0) { temp = -temp; }
                if ( ans < temp )
                {
                    ans = temp;
                }
            }
            printf("limit=%d\n", limit);
            CHECK_VALUE(ans, GetMaximumAbsolute(x, limit));
        }
    }

    // min absolute
    {
        printf("\n[min absolute]\n");
        const int n = 17;
        double __attribute((aligned(32))) x[n];
        double ans;
        const int limits[] = {n, 1, 2, 3, 4, 5, 6, 7, 8, 9};

        for ( unsigned int k = 0 ; k < sizeof(limits)/sizeof(int); ++k )
        {
            const int limit = limits[k];
            ans = __DBL_MAX__;
            for ( int i = 0 ; i < limit ; ++i )
            {
                x[i] = -(n - limit + i + 1);
                double temp = x[i];
                if (temp < 0) { temp = -temp; }
                if ( ans > temp )
                {
                    ans = temp;
                }
            }
            printf("limit=%d\n", limit);
            CHECK_VALUE(ans, GetMinimumAbsolute(x, limit));
        }
    }

    // mask load
    if(0)
    {
        const __m256i mask[4] =
        {
            {EFLAG, 0, 0, 0},
            {EFLAG, EFLAG, 0, 0},
            {EFLAG, EFLAG, EFLAG, 0},
            {EFLAG, EFLAG, EFLAG, EFLAG}
        };
        const unsigned char* ptr = reinterpret_cast<const unsigned char*>(mask);
        for (unsigned int i = 0; i < sizeof(mask); ++i)
        {
            printf("%02x ", ptr[i]);
            if ( (i % 32) == 31 )
            {
                printf("\n");
            }
        }
        printf("mask load\n");
        const int length = 4;
        double __attribute((aligned(32))) x[length];
        for ( int i = 0 ; i < length; ++i )
        {
            x[i] = i + 1;
        }
        for (int i = 0; i < length; ++i)
        {
            __m256d xv = _mm256_maskload_pd(x, mask[i]);
            double __attribute((aligned(32))) y[length] = {0};
            _mm256_store_pd(y, xv);
            printf("i=%d: ", i);
            for (int k = 0; k < length; ++k)
            {
                printf("%g\t", y[k]);
            }
            printf("\n");
        }
    }
    // dot
    {
        printf("\n[dot]\n");
        const int n = 17;
        double __attribute((aligned(32))) x[n];
        double __attribute((aligned(32))) y[n];
        const int limits[] = {n, 1, 2, 3, 4, 5, 6, 7, 8, 9};

        for ( unsigned int k = 0 ; k < sizeof(limits)/sizeof(int); ++k )
        {
            const int limit = limits[k];
            double ans = 0;
            for ( int i = 0 ; i < limit ; ++i )
            {
                x[i] = n - limit + i + 1;
                y[i] = i + 1;
                ans += x[i]*y[i];
            }
            printf("limit=%d\n", limit);
            CHECK_VALUE(ans, GetDotProduct(x, y, limit));
        }
    }
    // sum
    {
        printf("\n[sum]\n");
        const int n = 17;
        double __attribute((aligned(32))) x[n];
        const int limits[] = {n, 1, 2, 3, 4, 5, 6, 7, 8, 9};

        for ( unsigned int k = 0 ; k < sizeof(limits)/sizeof(int); ++k )
        {
            const int limit = limits[k];
            double ans = 0;
            for ( int i = 0 ; i < limit ; ++i )
            {
                x[i] = n - limit + i + 1;
                ans += x[i];
            }
            printf("limit=%d\n", limit);
            CHECK_VALUE(ans, GetSum(x, limit));
        }
    }
    // norm
    {
        printf("\n[norm]\n");
        const int n = 17;
        double __attribute((aligned(32))) x[n];
        const int limits[] = {n, 1, 2, 3, 4, 5, 6, 7, 8, 9};

        for ( unsigned int k = 0 ; k < sizeof(limits)/sizeof(int); ++k )
        {
            const int limit = limits[k];
            double ans = 0;
            for ( int i = 0 ; i < limit ; ++i )
            {
                x[i] = n - limit + i + 1;
                ans += x[i]*x[i];
            }
            ans = sqrt(ans);
            printf("limit=%d\n", limit);
            CHECK_VALUE(ans, GetNorm(x, limit));
        }
    }
    return 0;
#endif
    test_VectordBase();
    test_Vectord();
    benchmark_Vectord();

    return 0;
}
