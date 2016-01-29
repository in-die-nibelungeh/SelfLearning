
#include "mcon.h"

void DumpMatrix(const mcon::Matrix<double>&m, const char* fmt = NULL)
{
    for (uint i = 0; i < m.GetRowLength(); ++i)
    {
        printf("| ");
        for (uint j = 0; j < m.GetColumnLength(); ++j)
        {
            printf("%g", static_cast<double>(m[i][j]));
            printf("\t");
        }
        printf(" |\n");
    }
}

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
        const uint row = 5;
        const uint col = 6;
        mcon::Matrixd m(row, col);
        CHECK_VALUE(m.IsNull(), false);
        CHECK_VALUE(m.GetRowLength(), row);
        CHECK_VALUE(m.GetColumnLength(), col);

#define VALUE(i, k) ((i+1)*10 + (k+1))
        for (uint i = 0; i < row; ++i )
        {
            for (uint k = 0; k < col; ++k )
            {
                m[i][k] = VALUE(i, k);
            }
        }
        LOG("* [operator[]]\n");
        for (uint i = 0; i < row; ++i )
        {
            for (uint k = 0; k < col; ++k )
            {
                CHECK_VALUE(m[i][k], VALUE(i, k));
            }
        }
        LOG("* [operator+=(double)]\n");
        const double added = 2;
        m += added;
        for (uint i = 0; i < row; ++i )
        {
            for (uint k = 0; k < col; ++k )
            {
                CHECK_VALUE(m[i][k], VALUE(i, k) + added);
            }
        }
        LOG("* [operator*=(double)]\n");
        const double multiplicant = 3;
        m *= multiplicant;
        for (uint i = 0; i < row; ++i )
        {
            for (uint k = 0; k < col; ++k )
            {
                CHECK_VALUE(m[i][k], (VALUE(i, k) + added) * multiplicant);
            }
        }
        LOG("* [operator/=(double)]\n");
        m /= multiplicant;
        for (uint i = 0; i < row; ++i )
        {
            for (uint k = 0; k < col; ++k )
            {
                CHECK_VALUE(m[i][k], VALUE(i, k) + added);
            }
        }
        LOG("* [operator-=(double)]\n");
        m -= added;
        for (uint i = 0; i < row; ++i )
        {
            for (uint k = 0; k < col; ++k )
            {
                CHECK_VALUE(m[i][k], VALUE(i, k));
            }
        }
#undef VALUE
        LOG("* [Matrixd(const Matrixd&)]\n");
        mcon::Matrixd m1(m);
        for (uint i = 0; i < row; ++i )
        {
            for (uint k = 0; k < col; ++k )
            {
                CHECK_VALUE(m1[i][k], m[i][k]);
            }
        }
        LOG("* [Matrixd(const VectordBase&)]\n");
        {
            mcon::Vectord v(row);
#define VALUE(i) ((i+1)*10+i+1)
            for (uint i = 0; i < row; ++i )
            {
                v[i] = VALUE(i);
            }
#undef VALUE
            mcon::Matrixd m1(v);
            CHECK_VALUE(m1.GetRowLength(), 1);
            CHECK_VALUE(m1.GetColumnLength(), v.GetLength());
            for (uint i = 0; i < v.GetLength(); ++i )
            {
                CHECK_VALUE(m1[0][i], v[i]);
            }
            mcon::Matrixd m2(v, true);
            CHECK_VALUE(m2.GetRowLength(), v.GetLength());
            CHECK_VALUE(m2.GetColumnLength(), 1);
            for (uint i = 0; i < v.GetLength(); ++i )
            {
                CHECK_VALUE(m2[i][0], v[i]);
            }
        }
        LOG("* [operator+(double)]\n");
        m1 = m + added;
        for (uint i = 0; i < row; ++i )
        {
            for (uint k = 0; k < col; ++k )
            {
                CHECK_VALUE(m1[i][k], m[i][k] + added);
            }
        }
        LOG("* [operator*(double)]\n");
        m1 = m * multiplicant;
        for (uint i = 0; i < row; ++i )
        {
            for (uint k = 0; k < col; ++k )
            {
                CHECK_VALUE(m1[i][k], m[i][k]  * multiplicant);
            }
        }
        LOG("* [operator/(double)]\n");
        m1 = m / multiplicant;
        for (uint i = 0; i < row; ++i )
        {
            for (uint k = 0; k < col; ++k )
            {
                CHECK_VALUE(m1[i][k], m[i][k] / multiplicant);
            }
        }
        LOG("* [operator-(double)]\n");
        m1 = m - added;
        for (uint i = 0; i < row; ++i )
        {
            for (uint k = 0; k < col; ++k )
            {
                CHECK_VALUE(m1[i][k], m[i][k] - added);
            }
        }

        LOG("* [operator=(double)]\n");
        m = -added;
        for (uint i = 0; i < row; ++i )
        {
            for (uint k = 0; k < col; ++k )
            {
                CHECK_VALUE(m[i][k], -added);
            }
        }
    }
    {
#define VALUE1(i, k) ((i+1)*10 + (k+1))
#define VALUE2(i, k) ((i+1) + (k+1)*10)

        const uint row = 5;
        const uint col = 6;
        mcon::Matrixd m1(row, col);
        mcon::Matrixd m2(row, col);
        for (uint i = 0; i < row; ++i )
        {
            for (uint k = 0; k < col; ++k )
            {
                m1[i][k] = VALUE1(i, k);
                m2[i][k] = VALUE2(i, k);
            }
        }
        LOG("* [operator+=(Matrixd&)]\n");
        m1 += m2;
        for (uint i = 0; i < row; ++i )
        {
            for (uint k = 0; k < col; ++k )
            {
                CHECK_VALUE(m1[i][k], VALUE1(i, k) + VALUE2(i, k));
            }
        }
        LOG("* [operator-=(Matrixd&)]\n");
        m1 -= m2;
        for (uint i = 0; i < row; ++i )
        {
            for (uint k = 0; k < col; ++k )
            {
                CHECK_VALUE(m1[i][k], VALUE1(i, k));
            }
        }
        LOG("* [operator*=(Matrixd&)]\n");
        m1 *= m2;
        for (uint i = 0; i < row; ++i )
        {
            for (uint k = 0; k < col; ++k )
            {
                CHECK_VALUE(m1[i][k], VALUE1(i, k) * VALUE2(i, k));
            }
        }
        LOG("* [operator/=(Matrixd&)]\n");
        m1 /= m2;
        for (uint i = 0; i < row; ++i )
        {
            for (uint k = 0; k < col; ++k )
            {
                CHECK_VALUE(m1[i][k], VALUE1(i, k));
            }
        }
#undef VALUE1
#undef VALUE2
        mcon::Matrixd m3(row, col);
        LOG("* [operator+(Matrixd&)]\n");
        m3 = m1 + m2;
        for (uint i = 0; i < row; ++i )
        {
            for (uint k = 0; k < col; ++k )
            {
                CHECK_VALUE(m3[i][k], m1[i][k] + m2[i][k]);
            }
        }
        LOG("* [operator-(Matrixd&)]\n");
        m3 = m1 - m2;
        for (uint i = 0; i < row; ++i )
        {
            for (uint k = 0; k < col; ++k )
            {
                CHECK_VALUE(m3[i][k], m1[i][k] - m2[i][k]);
            }
        }
        LOG("* [operator*(Matrixd&)]\n");
        m3 = m1 * m2;
        for (uint i = 0; i < row; ++i )
        {
            for (uint k = 0; k < col; ++k )
            {
                CHECK_VALUE(m3[i][k], m1[i][k] * m2[i][k]);
            }
        }
        LOG("* [operator/(Matrixd&)]\n");
        m3 = m1 / m2;
        for (uint i = 0; i < row; ++i )
        {
            for (uint k = 0; k < col; ++k )
            {
                CHECK_VALUE(m3[i][k], m1[i][k] / m2[i][k]);
            }
        }
    }
    LOG("* [Resize]\n");
    {
        const int row = 5;
        const int col = 6;
        const int inc = 2;
        mcon::Matrixd m1(row, col);
        for (uint i = 0; i < row; ++i )
        {
            for (uint k = 0; k < col; ++k )
            {
                m1[i][k] = 1.0;
            }
        }
        m1.Resize(row+inc, col+inc);
        CHECK_VALUE(row+inc, m1.GetRowLength());
        CHECK_VALUE(col+inc, m1.GetColumnLength());
        for (uint i = 0; i < row+inc; ++i )
        {
            for (uint k = 0; k < col+inc; ++k )
            {
                m1[i][k] = 1.0;
            }
        }
    }
    LOG("* [Transpose]\n");
    {
        const int numArray = 3, numData= 5;
        mcon::Matrixd m(numArray, numData);
        for (uint i = 0; i < numArray; ++i)
        {
            for (uint j = 0; j < numData; ++j)
            {
                m[i][j] = (i+1)*10+(j+1);
            }
        }

        mcon::Matrixd mt(m.Transpose());
        for (uint i = 0; i < mt.GetRowLength(); ++i)
        {
            for (uint k = 0; k < mt.GetColumnLength(); ++k)
            {
                CHECK_VALUE(mt[i][k], m[k][i]);
            }
        }
    }

    LOG("* [Multiply]\n");
    {
        const int numArray = 3;
        const int numData= 4;
        mcon::Matrixd m1(numArray, numData);
        for (uint c = 1, i = 0; i < numArray; ++i)
        {
            for (uint j = 0; j < numData; ++j, ++c)
            {
                m1[i][j] = c;
            }
        }
        mcon::Matrixd m2(m1.T());
        mcon::Matrixd m3(m1.Multiply(m2));

        const double m4 [numArray][numArray] = {
            { 30,  70, 110},
            { 70, 174, 278},
            {110, 278, 446},
        };
        for (uint i = 0; i < numArray; ++i )
        {
            for (uint k = 0; k < numArray; ++k )
            {
                CHECK_VALUE(m3[i][k], m4[i][k]);
            }
        }
    }

    {
        LOG("* [Determinant]\n");
        const int numArray = 4;
        const int numData= 4;
        mcon::Matrixd m1(numArray, numData);
        m1[0][0] = 2;
        m1[0][1] = -2;
        m1[0][2] = 4;
        m1[0][3] = 2;
        m1[1][0] = 2;
        m1[1][1] = -1;
        m1[1][2] = 6;
        m1[1][3] = 3;
        m1[2][0] = 3;
        m1[2][1] = -2;
        m1[2][2] = 12;
        m1[2][3] = 12;
        m1[3][0] = -1;
        m1[3][1] = 3;
        m1[3][2] = -4;
        m1[3][3] = 4;
        CHECK_VALUE(m1.D(), 120);
    }

    {
        LOG("* [Inverse]\n");
        const int numArray = 4;
        const int numData= 4;
        mcon::Matrixd m1(numArray, numData);
        m1[0][0] = 3;
        m1[0][1] = 2;
        m1[0][2] = 1;
        m1[0][3] = 0;
        m1[1][0] = 1;
        m1[1][1] = 2;
        m1[1][2] = 3;
        m1[1][3] = 4;
        m1[2][0] = 2;
        m1[2][1] = 1;
        m1[2][2] = 0;
        m1[2][3] = 1;
        m1[3][0] = 2;
        m1[3][1] = 0;
        m1[3][2] = 2;
        m1[3][3] = 1;
        const mcon::Matrixd m2(m1.Inverse());
        const mcon::Matrixd m3(m1.Multiply(m2));
        for (uint i = 0; i < numArray; ++i )
        {
            for (uint k = 0; k < numArray; ++k )
            {
                double correct = ( k == i ) ? 1.0 : 0.0;
                CHECK_VALUE(m3[i][k], correct);
            }
        }
    }
    {
        LOG("* [SubMatrix]\n");
        const uint numArray = 4;
        const uint numData= 5;
        mcon::Matrixd m(numArray, numData);

        for (uint i = 0; i < numArray; ++i)
        {
            for (uint k = 0; k < numArray; ++k)
            {
                m[i][k] = (i + 1) * 10 + k + 1;
            }
        }
        // Normal Case
        {
            const uint rowBegin = 1;
            const uint rowEnd = 2;
            const uint columnBegin = 2;
            const uint columnEnd = 3;

            const mcon::Matrixd ms = m(rowBegin, rowEnd, columnBegin, columnEnd);

            CHECK_VALUE(ms.IsNull(), false);
            CHECK_VALUE(ms.GetRowLength(), rowEnd - rowBegin + 1);
            CHECK_VALUE(ms.GetColumnLength(), columnEnd - columnBegin + 1);

            for (uint i = 0; i < ms.GetRowLength(); ++i)
            {
                for (uint k = 0; k < ms.GetColumnLength(); ++k)
                {
                    CHECK_VALUE(ms[i][k], m[i + rowBegin][k + columnBegin]);
                }
            }
            const mcon::Matrixd ms1 = m(0, 0, 0, 0);
            CHECK_VALUE(ms1.IsNull(), false);
            CHECK_VALUE(ms1[0][0], 11);
        }
        // Normal Case (All)
        {
            const uint rowBegin = 0;
            const uint rowEnd = numArray - 1;
            const uint columnBegin = 0;
            const uint columnEnd = numData - 1;

            const mcon::Matrixd ma = m(rowBegin, rowEnd, columnBegin, columnEnd);

            CHECK_VALUE(ma.IsNull(), false);
            CHECK_VALUE(ma.GetRowLength(), rowEnd - rowBegin + 1);
            CHECK_VALUE(ma.GetColumnLength(), columnEnd - columnBegin + 1);

            for (uint i = 0; i < ma.GetRowLength(); ++i)
            {
                for (uint k = 0; k < ma.GetColumnLength(); ++k)
                {
                    CHECK_VALUE(ma[i][k], m[i + rowBegin][k + columnBegin]);
                }
            }
        }
        // Error Case
        {
            const uint rowBegin = 2;
            const uint rowEnd = 6;
            const uint columnBegin = 3;
            const uint columnEnd = 7;

            const mcon::Matrixd ms = m(rowBegin, rowEnd, columnBegin, columnEnd);

            CHECK_VALUE(ms.IsNull(), false);
            CHECK_VALUE(ms.GetRowLength(),  m.GetRowLength() - rowBegin);
            CHECK_VALUE(ms.GetColumnLength(),  m.GetColumnLength() - columnBegin);

            for (uint i = 0; i < ms.GetRowLength(); ++i)
            {
                for (uint k = 0; k < ms.GetColumnLength(); ++k)
                {
                    CHECK_VALUE(ms[i][k], m[i + rowBegin][k + columnBegin]);
                }
            }
            const mcon::Matrixd ms1 = m(1, 0, 0, 1);
            CHECK_VALUE(ms1.IsNull(), true);
            const mcon::Matrixd ms2 = m(0, 1, 1, 0);
            CHECK_VALUE(ms2.IsNull(), true);
        }
    }

    return ;
}
