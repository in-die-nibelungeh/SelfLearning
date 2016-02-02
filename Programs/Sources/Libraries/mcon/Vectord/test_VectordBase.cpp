
#include <math.h>

#include "debug.h"
#include "mcon.h"

static double _fabs(double v)
{
    return (v < 0) ? -v : v;
}

void ShowVectordBase(const mcon::VectordBase& v)
{
    const uint n = v.GetLength();
    for (uint i = 0; i < n; ++i )
    {
        printf("\t%g", v[i]);
        if ( (i % 8) == 7 )
        {
            printf("\n");
        }
    }
    printf("\n");
}

void test_VectordBase(void)
{
    const int length = 6;
    double __attribute((aligned(mcon::VectordBase::g_Alignment))) area[128];
    mcon::VectordBase dvec(area, length);

    LOG("* [GetLength]\n");
    CHECK_VALUE(dvec.GetLength(), length);

    LOG("* [operator[]]\n");
    for (uint i = 0; i < length; ++i)
    {
        dvec[i] = i;
    }
    for (uint i = 0; i < length; ++i)
    {
        CHECK_VALUE(dvec[i], i);
    }

    LOG("* [operator+=(double)]\n");
    // operator+=(T)
    dvec += 1;
    for (uint i = 0; i < length; ++i)
    {
        CHECK_VALUE(dvec[i], i+1);
    }
    // operator*=(T)
    LOG("* [operator*=(double)]\n");
    dvec *= 10;
    for (uint i = 0; i < length; ++i)
    {
        CHECK_VALUE(dvec[i], (i+1)*10);
    }
    // operator/=(T)
    LOG("* [operator/=(double)]\n");
    dvec /= 5;
    for (uint i = 0; i < length; ++i)
    {
        CHECK_VALUE(dvec[i], (i+1)*2);
    }
    // operator-=(T)
    LOG("* [operator-=(double)]\n");
    dvec -= 5;
    for (uint i = 0; i < length; ++i)
    {
        CHECK_VALUE(dvec[i], (static_cast<int>(i)+1)*2-5);
    }
    LOG("* [Copy]\n");
    mcon::VectordBase dvec2(area+(length/4+1)*4, length*2);

    for (uint i = 0; i < dvec2.GetLength(); ++i)
    {
        dvec2[i] = -(i+1);
    }
    // Copy
    dvec2.Copy(dvec);
    for (uint i = 0; i < dvec2.GetLength(); ++i)
    {
        if (0 <= i && i < length)
        {
            CHECK_VALUE(dvec2[i], (static_cast<int>(i)+1)*2-5);
        }
    }
    dvec = 10;
    for (uint i = 0; i < dvec2.GetLength(); ++i)
    {
        dvec2[i] = i + 1;
    }

    LOG("* [operator+=]\n");
    dvec += dvec2;
    for (uint i = 0; i < length; ++i)
    {
        CHECK_VALUE(dvec[i], (i+1) + 10);
    }
    for (uint i = 0; i < dvec2.GetLength(); ++i)
    {
        dvec2[i] = (i & 1) ? 1.0 : 2.0;
    }

    LOG("* [operator*=]\n");
    dvec *= dvec2;
    for (uint i = 0; i < length; ++i)
    {
        CHECK_VALUE(dvec[i], ((i+1) + 10) * ((i & 1) ? 1.0 : 2.0));
    }
    LOG("* [operator/=]\n");
    dvec /= dvec2;
    for (uint i = 0; i < length; ++i)
    {
        CHECK_VALUE(dvec[i], (i+1) + 10);
    }
    for (uint i = 0; i < dvec2.GetLength(); ++i)
    {
        dvec2[i] = i + 1;
    }
    LOG("* [operator-=]\n");
    dvec -= dvec2;
    for (uint i = 0; i < length; ++i)
    {
        CHECK_VALUE(dvec[i], 10);
    }
    // Fifo
    for (uint i = 0; i < dvec2.GetLength(); ++i)
    {
        dvec2[i] = i + 2;
    }
    LOG("* [Fifo]\n");
    double v = dvec2.Fifo(5);
    CHECK_VALUE(v, 2);
    for (uint i = 0; i < dvec2.GetLength()-1; ++i)
    {
        CHECK_VALUE(dvec2[i], i+3);
    }
    CHECK_VALUE(dvec2[dvec2.GetLength()-1], 5);

    // Unshift
    LOG("* [Unshift]\n");
    v = dvec2.Unshift(2);
    CHECK_VALUE(v, 5);
    for (uint i = 0; i < dvec2.GetLength(); ++i)
    {
        CHECK_VALUE(dvec2[i], i+2);
    }

    // Maximum/Minimum
    LOG("* [GetMaximum]\n");
    {
        const int lens[] = {1, 2, 3, 4, 5, 7, 8, 9, 31, 32, 33, 63, 64, 65};
        for ( unsigned int i = 0; i < sizeof(lens)/sizeof(int); ++i )
        {
            double ans = - __DBL_MAX__;
            const int n = lens[i];
            LOG("    Lenght=%d:\n", n);
            double __attribute((aligned(mcon::VectordBase::g_Alignment))) _area[128];
            mcon::VectordBase v(_area, n);
            // 昇順
            for ( int k = 0; k < n; ++k )
            {
                v[k] = k - n/2;
                if (  ans < v[k] )
                {
                    ans = v[k];
                }
            }
            CHECK_VALUE(ans, v.GetMaximum());
            // 降順
            ans = - __DBL_MAX__;
            for ( int k = 0; k < n; ++k )
            {
                v[k] = n/2 - k;
                if (  ans < v[k] )
                {
                    ans = v[k];
                }
            }
            CHECK_VALUE(ans, v.GetMaximum());
        }
    }

    LOG("* [GetMinimum]\n");
    {
        const int lens[] = {1, 2, 3, 4, 5, 7, 8, 9, 31, 32, 33, 63, 64, 65};
        for ( unsigned int i = 0; i < sizeof(lens)/sizeof(int); ++i )
        {
            double ans = __DBL_MAX__;
            const int n = lens[i];
            LOG("    Lenght=%d:\n", n);
            double __attribute((aligned(mcon::VectordBase::g_Alignment))) _area[128];
            mcon::VectordBase v(_area, n);
            // 昇順
            for ( int k = 0; k < n; ++k )
            {
                v[k] = k - n/2;
                if ( ans > v[k] )
                {
                    ans = v[k];
                }
            }
            CHECK_VALUE(ans, v.GetMinimum());
            // 降順
            ans = __DBL_MAX__;
            for ( int k = 0; k < n; ++k )
            {
                v[k] = n/2 - k;
                if (  ans > v[k] )
                {
                    ans = v[k];
                }
            }
            CHECK_VALUE(ans, v.GetMinimum());
        }
    }
    LOG("* [GetMaximumAbsolute]\n");
    {
        const int lens[] = {1, 2, 3, 4, 5, 7, 8, 9, 31, 32, 33, 63, 64, 65};
        for ( unsigned int i = 0; i < sizeof(lens)/sizeof(int); ++i )
        {
            double ans = 0;
            const int n = lens[i];
            LOG("    Lenght=%d:\n", n);
            double __attribute((aligned(mcon::VectordBase::g_Alignment))) _area[128];
            mcon::VectordBase v(_area, n);
            // 昇順
            for ( int k = 0; k < n; ++k )
            {
                v[k] = (k + 1) * (k & 1 ? -1 : 1);
                if (  ans < _fabs(v[k]) )
                {
                    ans = _fabs(v[k]);
                }
            }
            CHECK_VALUE(ans, v.GetMaximumAbsolute());
            // 昇順 (符号が逆)
            // 結果は変わらない
            v *= -1;
            CHECK_VALUE(ans, v.GetMaximumAbsolute());

            // 降順
            for ( int k = 0; k < n; ++k )
            {
                v[k] = n - k;
                if ( ans < _fabs(v[k]) )
                {
                    ans = _fabs(v[k]);
                }
            }
            CHECK_VALUE(ans, v.GetMaximumAbsolute());
            // 降順 (符号が逆)
            // 結果は変わらない
            v *= -1;
            CHECK_VALUE(ans, v.GetMaximumAbsolute());
        }
    }
    LOG("* [GetMinimumAbsolute]\n");
    {
        const int lens[] = {1, 2, 3, 4, 5, 7, 8, 9, 31, 32, 33, 63, 64, 65};
        for ( unsigned int i = 0; i < sizeof(lens)/sizeof(int); ++i )
        {
            double ans = __DBL_MAX__;
            const int n = lens[i];
            LOG("    Lenght=%d:\n", n);
            double __attribute((aligned(mcon::VectordBase::g_Alignment))) _area[128];
            mcon::VectordBase v(_area, n);
            // 昇順
            for ( int k = 0; k < n; ++k )
            {
                v[k] = (k + 1) * (k & 1 ? -1 : 1);
                if (  ans > _fabs(v[k]) )
                {
                    ans = _fabs(v[k]);
                }
            }
            CHECK_VALUE(ans, v.GetMinimumAbsolute());
            // 昇順 (符号が逆)
            // 結果は変わらない
            v *= -1;
            CHECK_VALUE(ans, v.GetMinimumAbsolute());

            // 降順
            ans = __DBL_MAX__;
            for ( int k = 0; k < n; ++k )
            {
                v[k] = n - k;
                if ( ans > _fabs(v[k]) )
                {
                    ans = _fabs(v[k]);
                }
            }
            CHECK_VALUE(ans, v.GetMinimumAbsolute());
            // 降順 (符号が逆)
            // 結果は変わらない
            v *= -1;
            CHECK_VALUE(ans, v.GetMinimumAbsolute());
        }
    }
    LOG("* [GetSum/Average/GetNorm]\n");
    {
        const int lens[] = {1, 2, 3, 4, 5, 7, 8, 9, 31, 32, 33, 63, 64, 65};
        for ( unsigned int i = 0; i < sizeof(lens)/sizeof(int); ++i )
        {
            const int n = lens[i];
            LOG("    Lenght=%d:\n", n);
            double __attribute((aligned(mcon::VectordBase::g_Alignment))) _area[128];
            mcon::VectordBase v(_area, n);
            double sum = 0;
            double norm = 0;
            for ( int k = 0; k < n; ++k )
            {
                v[k] = k + 1 - n/4;
                sum += v[k];
                norm += v[k]*v[k];
            }
            double ave = sum / n;
            norm = sqrt(norm);
            CHECK_VALUE(sum , v.GetSum());
            CHECK_VALUE(ave , v.GetAverage());
            CHECK_VALUE(norm, v.GetNorm());
        }
    }
    LOG("* [Dot]\n");
    {
        const int lens[] = {1, 2, 3, 4, 5, 7, 8, 9, 31, 32, 33, 63, 64, 65};
        for ( unsigned int i = 0; i < sizeof(lens)/sizeof(int); ++i )
        {
            const int n = lens[i];
            LOG("    Lenght=%d:\n", n);
            double __attribute((aligned(mcon::VectordBase::g_Alignment))) _area[256];
            mcon::VectordBase v(_area, n);
            mcon::VectordBase w(_area+128, n);
            double dot = 0;
            for ( int k = 0; k < n; ++k )
            {
                v[k] = k + 1 - n/4;
                w[k] = 3*n/4 - k - 1;
                dot += v[k]*w[k];
            }
            CHECK_VALUE(dot, v.GetDotProduct(w));
        }
    }
    LOG("* [GetLocalMinimumIndex/GetLocalMaximumIndex]\n");
    {
        const int lens[] = {1, -1, -2, 4, 5, 6, 5, -2, -6, 7, 10, 11, 0};
        const uint length = sizeof(lens)/sizeof(int);
        double __attribute((aligned(mcon::VectordBase::g_Alignment))) _area[length];
        mcon::VectordBase v(_area, length);
        for (uint k = 0; k < v.GetLength(); ++k)
        {
            v[k] = static_cast<double>(lens[k]);
        }
        // LocalMinimum
        int index = v.GetLocalMinimumIndex();
        CHECK_VALUE(index, 2);
        CHECK_VALUE(v[index], -2);
        index = v.GetLocalMinimumIndex(index + 1);
        CHECK_VALUE(index, 8);
        CHECK_VALUE(v[index], -6);
        index = v.GetLocalMinimumIndex(index + 1);
        CHECK_VALUE(index, -1);

        // LocalMaximum
        index = v.GetLocalMaximumIndex();
        CHECK_VALUE(index, 5);
        CHECK_VALUE(v[index], 6);
        index = v.GetLocalMaximumIndex(index + 1);
        CHECK_VALUE(index, 11);
        CHECK_VALUE(v[index], 11);
        index = v.GetLocalMaximumIndex(index + 1);
        CHECK_VALUE(index, -1);
    }
    LOG("END\n");
}
