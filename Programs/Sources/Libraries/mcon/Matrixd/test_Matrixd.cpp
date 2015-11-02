
#include "VectordBase.h"
#include "Vectord.h"
#include "Matrixd.h"

void test_Matrixd(void)
{
    LOG("* [Empty]\n");
    {
        mcon::Matrixd m;
        CHECK_VALUE(m.IsNull(), true);
        CHECK_VALUE(m.GetRowLength(), 0);
        CHECK_VALUE(m.GetColumnLength(), 0);
    }
    {
        const int row = 5;
        const int col = 6;
        mcon::Matrixd m(row, col);
        CHECK_VALUE(m.IsNull(), false);
        CHECK_VALUE(m.GetRowLength(), row);
        CHECK_VALUE(m.GetColumnLength(), col);
#define VALUE(i, k) ((i+1)*10 + (k+1))
        for ( int i = 0; i < row; ++i )
        {
            for ( int k = 0; k < col; ++k )
            {
                m[i][k] = VALUE(i, k);
            }
        }
        LOG("* [operator[]]\n");
        for ( int i = 0; i < row; ++i )
        {
            for ( int k = 0; k < col; ++k )
            {
                CHECK_VALUE(m[i][k], VALUE(i, k));
            }
        }
        LOG("* [operator+=(double)]\n");
        const double added = 2;
        m += added;
        for ( int i = 0; i < row; ++i )
        {
            for ( int k = 0; k < col; ++k )
            {
                CHECK_VALUE(m[i][k], VALUE(i, k) + added);
            }
        }
        LOG("* [operator*=(double)]\n");
        const double multiplicant = 3;
        m *= multiplicant;
        for ( int i = 0; i < row; ++i )
        {
            for ( int k = 0; k < col; ++k )
            {
                CHECK_VALUE(m[i][k], (VALUE(i, k) + added) * multiplicant);
            }
        }
        LOG("* [operator/=(double)]\n");
        m /= multiplicant;
        for ( int i = 0; i < row; ++i )
        {
            for ( int k = 0; k < col; ++k )
            {
                CHECK_VALUE(m[i][k], VALUE(i, k) + added);
            }
        }
        LOG("* [operator-=(double)]\n");
        m -= added;
        for ( int i = 0; i < row; ++i )
        {
            for ( int k = 0; k < col; ++k )
            {
                CHECK_VALUE(m[i][k], VALUE(i, k));
            }
        }
#undef VALUE
        LOG("* [Matrixd(const Matrixd&)]\n");
        mcon::Matrixd m1(m);
        for ( int i = 0; i < row; ++i )
        {
            for ( int k = 0; k < col; ++k )
            {
                CHECK_VALUE(m1[i][k], m[i][k]);
            }
        }
        LOG("* [operator+(double)]\n");
        m1 = m + added;
        for ( int i = 0; i < row; ++i )
        {
            for ( int k = 0; k < col; ++k )
            {
                CHECK_VALUE(m1[i][k], m[i][k] + added);
            }
        }
        LOG("* [operator*(double)]\n");
        m1 = m * multiplicant;
        for ( int i = 0; i < row; ++i )
        {
            for ( int k = 0; k < col; ++k )
            {
                CHECK_VALUE(m1[i][k], m[i][k]  * multiplicant);
            }
        }
        LOG("* [operator/(double)]\n");
        m1 = m / multiplicant;
        for ( int i = 0; i < row; ++i )
        {
            for ( int k = 0; k < col; ++k )
            {
                CHECK_VALUE(m1[i][k], m[i][k] / multiplicant);
            }
        }
        LOG("* [operator-(double)]\n");
        m1 = m - added;
        for ( int i = 0; i < row; ++i )
        {
            for ( int k = 0; k < col; ++k )
            {
                CHECK_VALUE(m1[i][k], m[i][k] - added);
            }
        }

        LOG("* [operator=(double)]\n");
        m = -added;
        for ( int i = 0; i < row; ++i )
        {
            for ( int k = 0; k < col; ++k )
            {
                CHECK_VALUE(m[i][k], -added);
            }
        }
    }
    return ;
}
