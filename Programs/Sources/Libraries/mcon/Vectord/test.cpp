
#include "debug.h"

static double _fabs(double v)
{
    return (v < 0) ? -v : v;
}

void ShowVectord(const mcon::Vectord& v)
{
    const int n = v.GetLength();
    for (int i = 0; i < n; ++i )
    {
        printf("\t%g", v[i]);
        if ( (i % 8) == 7 )
        {
            printf("\n");
        }
    }
    printf("\n");
}

static void test_vectord_api(void)
{
    LOG("* [Empty Vector]\n");
    mcon::Vectord dvec;

    // Zero length
    CHECK_VALUE(dvec.GetLength(), 0);
    // IsNull() is true.
    CHECK_VALUE(dvec.IsNull(), true);

    LOG("* [Resize]\n");
    const int length = 6;
    dvec.Resize(length);
    CHECK_VALUE(dvec.GetLength(), length);
    // IsNull() is false.
    CHECK_VALUE(dvec.IsNull(), false);

    for (int i = 0; i < length; ++i)
    {
        dvec[i] = i;
    }
    for (int i = 0; i < length; ++i)
    {
        CHECK_VALUE(dvec[i], i);
    }

    LOG("* [operator+=(double)]\n");
    // operator+=(T)
    dvec += 1;
    for (int i = 0; i < length; ++i)
    {
        CHECK_VALUE(dvec[i], i+1);
    }
    // operator*=(T)
    LOG("* [operator*=(double)]\n");
    dvec *= 10;
    for (int i = 0; i < length; ++i)
    {
        CHECK_VALUE(dvec[i], (i+1)*10);
    }
    // operator/=(T)
    LOG("* [operator/=(double)]\n");
    dvec /= 5;
    for (int i = 0; i < length; ++i)
    {
        CHECK_VALUE(dvec[i], (i+1)*2);
    }
    // operator-=(T)
    LOG("* [operator-=(double)]\n");
    dvec -= 5;
    for (int i = 0; i < length; ++i)
    {
        CHECK_VALUE(dvec[i], (i+1)*2-5);
    }
    LOG("* [Copy]\n");
    mcon::Vectord dvec2(length*2);

    for (int i = 0; i < dvec2.GetLength(); ++i)
    {
        dvec2[i] = -(i+1);
    }
    // Copy
    dvec2.Copy(dvec);
    for (int i = 0; i < dvec2.GetLength(); ++i)
    {
        if (0 <= i && i < length)
        {
            CHECK_VALUE(dvec2[i], (i+1)*2-5);
        }
        else if (length <= i && i < length*2)
        {
            CHECK_VALUE(dvec2[i], -(i+1));
        }
    }
    LOG("* [operator=]\n");
    // Substitution
    dvec2 = dvec;
    for (int i = 0; i < length; ++i)
    {
        CHECK_VALUE(dvec2[i], (i+1)*2-5);
    }
    dvec = 10;
    for (int i = 0; i < dvec2.GetLength(); ++i)
    {
        dvec2[i] = i + 1;
    }

    LOG("* [operator?=]\n");
    dvec += dvec2;
    for (int i = 0; i < length; ++i)
    {
        CHECK_VALUE(dvec[i], (i+1) + 10);
    }
    for (int i = 0; i < dvec2.GetLength(); ++i)
    {
        dvec2[i] = (i & 1) ? 1.0 : 2.0;
    }

    dvec *= dvec2;
    for (int i = 0; i < length; ++i)
    {
        CHECK_VALUE(dvec[i], ((i+1) + 10) * ((i & 1) ? 1.0 : 2.0));
    }
    dvec /= dvec2;
    for (int i = 0; i < length; ++i)
    {
        CHECK_VALUE(dvec[i], (i+1) + 10);
    }
    for (int i = 0; i < dvec2.GetLength(); ++i)
    {
        dvec2[i] = i + 1;
    }
    dvec -= dvec2;
    for (int i = 0; i < length; ++i)
    {
        CHECK_VALUE(dvec[i], 10);
    }
    LOG("* [Cast]\n");
    LOG("* [Carve-out]\n");
    for (int i = 0; i < dvec.GetLength(); ++i)
    {
        dvec[i] = i + 1;
    }
    dvec2 = dvec(0, 0);
    CHECK_VALUE(dvec2.IsNull(), true);
    dvec2 = dvec(-1, 1);
    CHECK_VALUE(dvec2.IsNull(), true);
    dvec2 = dvec(dvec.GetLength(), 1);
    CHECK_VALUE(dvec2.IsNull(), true);
    dvec2 = dvec(dvec.GetLength()-1, 5);
    CHECK_VALUE(dvec2.IsNull(), false);
    for (int i = 0; i < dvec2.GetLength(); ++i)
    {
        CHECK_VALUE(dvec2[i], 6);
    }

    dvec2 = dvec(1, 3);
    for (int i = 0; i < dvec2.GetLength(); ++i)
    {
        CHECK_VALUE(dvec2[i], i+2);
    }
    // Fifo
    LOG("* [Fifo]\n");
    double v = dvec2.FifoIn(5);
    CHECK_VALUE(v, 2);
    for (int i = 0; i < dvec2.GetLength(); ++i)
    {
        CHECK_VALUE(dvec2[i], i+3);
    }
    // Unshift
    LOG("* [Unshift]\n");
    v = dvec2.Unshift(2);
    CHECK_VALUE(v, 5);
    for (int i = 0; i < dvec2.GetLength(); ++i)
    {
        CHECK_VALUE(dvec2[i], i+2);
    }

    LOG("* [Cast]\n");
    CHECK_VALUE( static_cast<double>(dvec), dvec[0] );
    // Maximum/Minimum
    LOG("* [GetMaximum]\n");
    {
        const int lens[] = {1, 2, 3, 4, 5, 7, 8, 9, 31, 32, 33, 63, 64, 65};
        for ( unsigned int i = 0; i < sizeof(lens)/sizeof(int); ++i )
        {
            double ans = - __DBL_MAX__;
            const int n = lens[i];
            LOG("    Lenght=%d:\n", n);
            mcon::Vectord v(n);
            // è∏èá
            for ( int k = 0; k < n; ++k )
            {
                v[k] = k - n/2;
                if (  ans < v[k] )
                {
                    ans = v[k];
                }
            }
            CHECK_VALUE(ans, v.GetMaximum());
            // ç~èá
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
            mcon::Vectord v(n);
            // è∏èá
            for ( int k = 0; k < n; ++k )
            {
                v[k] = k - n/2;
                if ( ans > v[k] )
                {
                    ans = v[k];
                }
            }
            CHECK_VALUE(ans, v.GetMinimum());
            // ç~èá
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
            mcon::Vectord v(n);
            // è∏èá
            for ( int k = 0; k < n; ++k )
            {
                v[k] = (k + 1) * (k & 1 ? -1 : 1);
                if (  ans < _fabs(v[k]) )
                {
                    ans = _fabs(v[k]);
                }
            }
            CHECK_VALUE(ans, v.GetMaximumAbsolute());
            // è∏èá (ïÑçÜÇ™ãt)
            // åãâ ÇÕïœÇÌÇÁÇ»Ç¢
            v *= -1;
            CHECK_VALUE(ans, v.GetMaximumAbsolute());

            // ç~èá
            for ( int k = 0; k < n; ++k )
            {
                v[k] = n - k;
                if ( ans < _fabs(v[k]) )
                {
                    ans = _fabs(v[k]);
                }
            }
            CHECK_VALUE(ans, v.GetMaximumAbsolute());
            // ç~èá (ïÑçÜÇ™ãt)
            // åãâ ÇÕïœÇÌÇÁÇ»Ç¢
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
            mcon::Vectord v(n);
            // è∏èá
            for ( int k = 0; k < n; ++k )
            {
                v[k] = (k + 1) * (k & 1 ? -1 : 1);
                if (  ans > _fabs(v[k]) )
                {
                    ans = _fabs(v[k]);
                }
            }
            CHECK_VALUE(ans, v.GetMinimumAbsolute());
            // è∏èá (ïÑçÜÇ™ãt)
            // åãâ ÇÕïœÇÌÇÁÇ»Ç¢
            v *= -1;
            CHECK_VALUE(ans, v.GetMinimumAbsolute());

            // ç~èá
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
            // ç~èá (ïÑçÜÇ™ãt)
            // åãâ ÇÕïœÇÌÇÁÇ»Ç¢
            v *= -1;
            CHECK_VALUE(ans, v.GetMinimumAbsolute());
        }
    }
    LOG("* [GetSum/Average/GetNorm]\n");
    {
        const int length = 10;
        mcon::Vectord v(length);
        for (int i = 0; i < length; ++i)
        {
            v[i] = i + 1;
        }
        const double sum = v.GetSum();
        const double ave = v.GetAverage();
        UNUSED(sum);
        UNUSED(ave);
        CHECK_VALUE(sum, 55);
        CHECK_VALUE(ave, 5.5);
        v[length - 1] = 2;
        const double norm = v.GetNorm();
        UNUSED(norm);
        CHECK_VALUE(norm, 17);
    }
    LOG("END\n");
}
