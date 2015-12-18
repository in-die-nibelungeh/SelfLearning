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

#include <x86intrin.h>
#include <string.h>
#include <math.h>

#include "debug.h"
#include "mcon.h"

#ifdef VDB_ALIGN
#error VDB_ALIGN is already defined.
#endif
#define VDB_ALIGN(n) __attribute__((aligned(n)))

#ifdef VDB_PASS
#error VDB_PASS is already defined.
#endif
#define VDB_PASS (1LL<<63)

namespace {

    static const int g_NumSimdLanes = 4;
    static const __m256i g_RemainMask[g_NumSimdLanes-1] =
    {
        {VDB_PASS,        0,        0,        0},
        {VDB_PASS, VDB_PASS,        0,        0},
        {VDB_PASS, VDB_PASS, VDB_PASS,        0}
    };
} // anonymous

namespace mcon {

double VectordBase::PushFromBack(double v)
{
    double ret = (*this)[0];
    for (int i = 0; i < GetLength() - 1; ++i)
    {
        (*this)[i] = (*this)[i+1];
    }
    (*this)[GetLength()-1] = v;
    return ret;
}

double VectordBase::PushFromFront(double v)
{
    double ret = (*this)[GetLength()-1];
    for (int i = GetLength() - 1; i > 0; --i)
    {
        (*this)[i] = (*this)[i-1];
    }
    (*this)[0] = v;
    return ret;
}

VectordBase& VectordBase::operator=(double v)
{
    const int unit = 4;
    double* const ptr = (*this);
    double* pEr = ptr;
    ASSERT_ALIGNED(ptr, g_Alignment);
    const __m256d vv = _mm256_set_pd(v, v, v, v);
    const int length = GetLength();
    const int units = (length / unit) * unit;
    for ( ; pEr < ptr + units ; pEr += unit )
    {
        _mm256_store_pd(pEr, vv);
    }
    for ( ; pEr < ptr + length; ++pEr )
    {
        *pEr = v;
    }
    return *this;
};

VectordBase& VectordBase::operator+=(double v)
{
    double* const pBase = *this;
    ASSERT_ALIGNED(pBase, g_Alignment);
    double* pEr = pBase;
    const int unit = 4;
    const int length = GetLength();
    const int units = (length / unit) * unit;
    const __m256d vv = _mm256_set_pd(v, v, v, v);
    for ( ; pEr < pBase + units; pEr += unit )
    {
        __m256d x = _mm256_load_pd(pEr);
        x = _mm256_add_pd(x, vv);
        _mm256_store_pd(pEr, x);
    }
    for ( ; pEr < pBase + length; ++pEr )
    {
        *pEr += v;
    }
    return *this;
}

VectordBase& VectordBase::operator*=(double v)
{
    double* const pBase = *this;
    ASSERT_ALIGNED(pBase, g_Alignment);
    double* pEr = pBase;
    const int unit = 4;
    const int length = GetLength();
    const int units = (length / unit) * unit;
    const __m256d vv = _mm256_set_pd(v, v, v, v);
    for ( ; pEr < pBase + units; pEr += unit )
    {
        __m256d x = _mm256_load_pd(pEr);
        x = _mm256_mul_pd(x, vv);
        _mm256_store_pd(pEr, x);
    }
    for ( ; pEr < pBase + length; ++pEr )
    {
        *pEr *= v;
    }
    return *this;
}

VectordBase& VectordBase::operator-=(double v) { *this += (-v) ; return *this; }
VectordBase& VectordBase::operator/=(double v) { *this *= (1/v); return *this; }

#define TEMPLATE_VECTOR_VECTOR_OPERATION(er, ee, ope) \
    const int unit = 4;                               \
    const double* const ptr = er;                     \
    double* pEr = er;                                 \
    const double* pEe = ee;                           \
    ASSERT_ALIGNED(pEr, g_Alignment);                 \
    ASSERT_ALIGNED(pEe, g_Alignment);                 \
    const int length = Smaller(ee.GetLength());       \
    const int units = (length / unit) * unit;         \
    for ( ; pEr < ptr + units ; pEr += unit, pEe += unit ) \
    {                                                 \
        __m256d x = _mm256_load_pd(pEr);              \
        __m256d y = _mm256_load_pd(pEe);              \
        x = _mm256_ ## ope ## _pd(x, y);              \
        _mm256_store_pd(pEr, x);                      \
    }                                                 \
    const int remain = length - units;                \
    if (remain > 0)                                   \
    {                                                 \
        const __m256i m = g_RemainMask[remain-1];     \
        __m256d x = _mm256_maskload_pd(pEr, m);       \
        __m256d y = _mm256_maskload_pd(pEe, m);       \
        x = _mm256_ ## ope ## _pd(x, y);              \
        _mm256_maskstore_pd(pEr, m, x);               \
    }

VectordBase& VectordBase::operator+=(const VectordBase& v)
{
    TEMPLATE_VECTOR_VECTOR_OPERATION((*this), v, add);
    return *this;
}
VectordBase& VectordBase::operator-=(const VectordBase& v)
{
    TEMPLATE_VECTOR_VECTOR_OPERATION((*this), v, sub);
    return *this;
}

VectordBase& VectordBase::operator*=(const VectordBase& v)
{
    TEMPLATE_VECTOR_VECTOR_OPERATION((*this), v, mul);
    return *this;
}
VectordBase& VectordBase::operator/=(const VectordBase& v)
{
    TEMPLATE_VECTOR_VECTOR_OPERATION((*this), v, div);
    return *this;
}

#undef TEMPLATE_VECTOR_VECTOR_OPERATION // Not used in the following

double VectordBase::GetMaximum(void) const
{
    double VDB_ALIGN(32) max;
    const double* const ptr = *this;
    const int length = GetLength();
    const int unit = 4;
    const double* pEr = ptr;

    if (length <= unit*2)
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
        double VDB_ALIGN(32) maxs[unit] = {0};
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

double VectordBase::GetMaximumAbsolute(void) const
{
    double VDB_ALIGN(32) maxAbs;
    const int length = GetLength();
    const double* const ptr = *this;
    const int unit = 4;
    const double* pEr = ptr;
    if (length <= unit*2)
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
        double VDB_ALIGN(32) maxabss[4];
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

double VectordBase::GetMinimum(void) const
{
    double VDB_ALIGN(32) min;
    const double* const ptr = *this;
    const int length = GetLength();
    const int unit = 4;
    const double* pEr = ptr;
    if (length <= unit*2)
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
        double VDB_ALIGN(32) mins[unit];
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

double VectordBase::GetMinimumAbsolute(void) const
{
    double VDB_ALIGN(32) minAbs;
    const double* const ptr = *this;
    const int length = GetLength();
    const int unit = 4;
    const double* pEr = ptr;

    if (length <= unit * 2)
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
        double VDB_ALIGN(32) minabss[4];
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


double VectordBase::GetSum(void) const
{
    double sum = 0;
    const int unit = 4;
    const int length = GetLength();
    const double* const ptr = *this;
    const double* pEr = ptr;
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
        double VDB_ALIGN(32) accs[4] = {0};
        _mm256_store_pd(accs, accv);
        sum = accs[0] + accs[1] + accs[2] + accs[3];
    }
    return sum;
}

double VectordBase::GetNorm(void) const
{
    double squaredSum = 0;
    const int length = GetLength();
    const int unit = 4;
    const double* const ptr = *this;
    const double* pEr = ptr;
    if ( length < unit * 2 )
    {
        for ( ; pEr < ptr + length; ++pEr )
        {
            squaredSum += *pEr * *pEr;
        }
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
        double VDB_ALIGN(32) norms[unit];
        _mm256_store_pd(norms, normv);
        squaredSum = norms[0] + norms[1] + norms[2] + norms[3];
    }
    return sqrt(squaredSum);
}

double VectordBase::GetDotProduct(const VectordBase& v) const
{
    const int length = GetLength();
    const int unit = 4;
    const double* const ptr = *this;
    const double* pEr = ptr;
    const double* pEe = v;
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
    double VDB_ALIGN(32) accs[4];
    _mm256_store_pd(accs, accv);
    return accs[0] + accs[1] + accs[2] + accs[3];
}

const VectordBase& VectordBase::Copy(const VectordBase& v)
{
    const int n = Smaller(v.GetLength());
    memcpy(*this, v, n * sizeof(double));
    return *this;
}

} // namespace mcon {
