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
#include "Vectord.h"

#define ALIGN(n) __attribute__((aligned(n)))

#ifdef PASS
#error PASS is already defined.
#endif

#define PASS (1LL<<63)

namespace {
    static const __m256i g_RemainMask[3] =
    {
        {PASS,    0,    0,   0},
        {PASS, PASS,    0,   0},
        {PASS, PASS, PASS,   0}
    };
    double* Align(const double* ptr, int align)
    {
        const unsigned char* aligned = reinterpret_cast<const unsigned char*>(ptr);
        while (reinterpret_cast<int>(aligned) % align) { ++aligned; }
        return const_cast<double*>(reinterpret_cast<const double*>(aligned));
    }
}

namespace mcon {

class Matrixd;

#define MCON_ITERATION(var, iter, statement)  \
    do {                                      \
        for (int var = 0; var < iter; ++var)  \
        {                                     \
            statement;                        \
        }                                     \
    } while(0)

Vectord::Vectord(int length)
    : m_AddressBase(NULL),
    m_AddressAligned(NULL),
    m_Length(length)
{
    Allocate();
}

Vectord::Vectord(const Vectord& v)
    : m_AddressBase(NULL),
    m_AddressAligned(NULL),
    m_Length(v.GetLength())
{
    Allocate();
    memcpy(m_AddressAligned, v, GetLength() * sizeof(double));
}

Vectord::~Vectord()
{
    if (NULL != m_AddressBase)
    {
        delete[] m_AddressBase;
        m_AddressBase = NULL;
        m_AddressAligned = NULL;
    }
    m_Length = 0;
}

Vectord Vectord::operator()(int offset, int length) const
{
    Vectord carveout;
    if (offset < 0 || GetLength() <= offset || length < 0)
    {
        // Null object.
        return carveout;
    }
    // Smaller value as length
    carveout.Resize( Smaller(GetLength() - offset, length) );
    for (int i = offset; i < Smaller(offset + length); ++i)
    {
        carveout[i-offset] = (*this)[i];
    }
    return carveout;
}

double Vectord::PushFromBack(double v)
{
    double ret = (*this)[0];
    for (int i = 0; i < GetLength() - 1; ++i)
    {
        (*this)[i] = (*this)[i+1];
    }
    (*this)[GetLength()-1] = v;
    return ret;
}

double Vectord::PushFromFront(double v)
{
    double ret = (*this)[GetLength()-1];
    for (int i = GetLength() - 1; i > 0; --i)
    {
        (*this)[i] = (*this)[i-1];
    }
    (*this)[0] = v;
    return ret;
}

Vectord& Vectord::operator=(double v)
{
#if 0
    MCON_ITERATION(i, m_Length, (*this)[i] = v);
#else
    if (IsNull())
    {
        return *this;
    }
    const int unit = 4;
    double* const ptr = (*this);
    double* pEr = ptr;
    ASSERT_ALIGNED(ptr, g_Alignment);
    *pEr = v;
    const __m256d vv = _mm256_broadcast_sd(pEr);
    const int length = GetLength();
    const int units = (length / unit) * unit;
    for ( ; pEr < ptr + units ; pEr += unit )
    {
        _mm256_store_pd(pEr, vv);
    }
    for ( ; pEr < ptr + length; )
    {
        *pEr = v;
    }
#endif
    return *this;
};

const Vectord Vectord::operator+(double v) const { Vectord vec(*this);  vec += v; return vec; }
const Vectord Vectord::operator-(double v) const { Vectord vec(*this);  vec -= v; return vec; }
const Vectord Vectord::operator*(double v) const { Vectord vec(*this);  vec *= v; return vec; }
const Vectord Vectord::operator/(double v) const { Vectord vec(*this);  vec /= v; return vec; }

const Vectord Vectord::operator+(const Vectord& v) const { Vectord vec(*this);  vec += v; return vec; }
const Vectord Vectord::operator-(const Vectord& v) const { Vectord vec(*this);  vec -= v; return vec; }
const Vectord Vectord::operator*(const Vectord& v) const { Vectord vec(*this);  vec *= v; return vec; }
const Vectord Vectord::operator/(const Vectord& v) const { Vectord vec(*this);  vec /= v; return vec; }

Vectord& Vectord::operator+=(double v)
{
#if 0
    MCON_ITERATION(i, m_Length, (*this)[i] += v);
#else
    double* ptr = (*this);
    ASSERT_ALIGNED(ptr, g_Alignment);
    const double ALIGN(32) _v[1] = {v};
    ASSERT_ALIGNED(_v, g_Alignment);
    const __m256d y = _mm256_broadcast_sd(_v);
    const int length = GetLength();
    int done = 0;
    for ( ; done < (length/4) * 4; done += 4, ptr += 4 )
    {
        __m256d x = _mm256_load_pd(ptr);
        x = _mm256_add_pd(x, y);
        _mm256_store_pd(ptr, x);
    }
    for ( int i = done; i < length; ++i )
    {
        (*this)[i] += v;
    }
#endif
    return *this;
}

Vectord& Vectord::operator-=(double v) { *this += (-v); return *this; }
Vectord& Vectord::operator*=(double v)
{
#if 0
    MCON_ITERATION(i, m_Length, (*this)[i] *= v);
#else
    double* ptr = m_AddressAligned;
    ASSERT_ALIGNED(ptr, g_Alignment);
    const double ALIGN(32) _v[1] = {v};
    ASSERT_ALIGNED(_v, g_Alignment);
    const __m256d y = _mm256_broadcast_sd(_v);
    const int length = GetLength();
    int done = 0;
    for ( ; done < (length/4) * 4; done += 4, ptr += 4 )
    {
        __m256d x = _mm256_load_pd(ptr);
        x = _mm256_mul_pd(x, y);
        _mm256_store_pd(ptr, x);
    }
    for ( int i = done; i < length; ++i )
    {
        (*this)[i] += v;
    }
#endif
    return *this;
}

Vectord& Vectord::operator/=(double v) { *this *= (1/v); return *this; }

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

Vectord& Vectord::operator+=(const Vectord& v)
{
#if 0
    MCON_ITERATION(i, Smaller(v.GetLength()), (*this)[i] += v[i]);
#else
    TEMPLATE_VECTOR_VECTOR_OPERATION((*this), v, add);
#endif
    return *this;
}
Vectord& Vectord::operator-=(const Vectord& v)
{
#if 0
    MCON_ITERATION(i, Smaller(v.GetLength()), (*this)[i] -= v[i]);
#else
    TEMPLATE_VECTOR_VECTOR_OPERATION((*this), v, sub);
#endif
    return *this;
}

Vectord& Vectord::operator*=(const Vectord& v)
{
#if 0
    MCON_ITERATION(i, Smaller(v.GetLength()), (*this)[i] *= v[i]);
#else
    TEMPLATE_VECTOR_VECTOR_OPERATION((*this), v, mul);
#endif
    return *this;
}
Vectord& Vectord::operator/=(const Vectord& v)
{
#if 0
    MCON_ITERATION(i, Smaller(v.GetLength()), (*this)[i] /= v[i]);
#else
    TEMPLATE_VECTOR_VECTOR_OPERATION((*this), v, div);
#endif
    return *this;
}

#undef TEMPLATE_VECTOR_VECTOR_OPERATION // Not used in the following

double Vectord::GetMaximum(void) const
{
    double __attribute((aligned(32))) max;
#if 0
    max = (*this)[0];
    for (int i = 1; i < GetLength(); ++i)
    {
        if (max < (*this)[i])
        {
            max = (*this)[i];
        }
    }
#else
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
#endif
    return max;
}

double Vectord::GetMaximumAbsolute(void) const
{
    double __attribute((aligned(32))) maxAbs;
#if 0
    double maxAbs = Absolute((*this)[0]);
    for (int i = 1; i < GetLength(); ++i)
    {
        const double v = Absolute((*this)[i]);
        if (maxAbs < v)
        {
            maxAbs = v;
        }
    }
#else
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
#endif
    return maxAbs;
}

double Vectord::GetMinimum(void) const
{
    double __attribute((aligned(32))) min;
#if 0
    min = (*this)[0];
    for (int i = 1; i < GetLength(); ++i)
    {
        if (min > (*this)[i])
        {
            min = (*this)[i];
        }
    }
#else
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
#endif
    return min;
}

double Vectord::GetMinimumAbsolute(void) const
{
    double __attribute((aligned(32))) minAbs;
#if 0
    minAbs = Absolute((*this)[0]);
    for (int i = 1; i < GetLength(); ++i)
    {
        const double v = Absolute((*this)[i]);
        if (min > v)
        {
            min = v;
        }
    }
#else
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
#endif
    return minAbs;
}


double Vectord::GetSum(void) const
{
    double sum = 0;
#if 0
    MCON_ITERATION( i, GetLength(), sum += (*this)[i] );
#else
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
        double __attribute((aligned(32))) accs[4] = {0};
        _mm256_store_pd(accs, accv);
        sum = accs[0] + accs[1] + accs[2] + accs[3];
    }
#endif
    return sum;
}

double Vectord::GetNorm(void) const
{
    double squaredSum = 0;
#if 0
    MCON_ITERATION( i, GetLength(), squaredSum += (*this)[i] * (*this)[i]);
#else
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
        double __attribute((aligned(32))) norms[unit];
        _mm256_store_pd(norms, normv);
        squaredSum = norms[0] + norms[1] + norms[2] + norms[3];
    }
#endif
    return sqrt(squaredSum);
}

double Vectord::GetDotProduct(Vectord& v) const
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
    double __attribute((aligned(32))) accs[4];
    _mm256_store_pd(accs, accv);
    return accs[0] + accs[1] + accs[2] + accs[3];
}

void Vectord::Allocate(void)
{
    m_AddressBase = NULL;
    if (m_Length > 0)
    {
        m_AddressBase = new double[m_Length + g_Alignment/sizeof(double)];
        m_AddressAligned = Align(m_AddressBase, g_Alignment);
        DEBUG_LOG("m_Address=%p\n", m_AddressBase);
        DEBUG_LOG("m_AddressAligned=%p\n", m_AddressAligned);
        ASSERT(NULL != m_AddressBase);
    }
}

const Vectord& Vectord::Copy(const Vectord& v)
{
    const int n = Smaller(v.GetLength());
    memcpy(*this, v, n * sizeof(double));
    return *this;
}


Vectord& Vectord::operator=(const Vectord& v)
{
    // m_Length is updated in Resize().
    const int n = v.GetLength();
    Resize(n);
    memcpy(*this, v, n * sizeof(double));
    return *this;
}

bool Vectord::Resize(int length)
{
    if (length < 0)
    {
        return false;
    }
    if (length == m_Length)
    {
        return true;
    }
    if (NULL != m_AddressBase)
    {
        delete[] m_AddressBase;
        m_AddressBase = NULL;
    }
    m_Length = length;
    m_AddressBase = new double[length + g_Alignment/sizeof(double)];
    m_AddressAligned = Align(m_AddressBase, g_Alignment);
    DEBUG_LOG("m_Address=%p\n", m_AddressBase);
    DEBUG_LOG("m_AddressAligned=%p\n", m_AddressAligned);
    ASSERT (NULL != m_AddressBase);
    return true;
}

} // namespace mcon {
