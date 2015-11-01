
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
    LOG("* [operator[]]\n");
    {
        const int row = 5;
        const int col = 6;
        mcon::Matrixd m(row, col);
        CHECK_VALUE(m.IsNull(), true);
        CHECK_VALUE(m.GetRowLength(), row);
        CHECK_VALUE(m.GetColumnLength(), col);
        for ( int i = 0; i < row; ++i )
        {
            for ( int k = 0; k < col; ++k )
            {
                m[i][k] = (i+1)*10 + (k+1);
            }
        }
        for ( int i = 0; i < row; ++i )
        {
            for ( int k = 0; k < col; ++k )
            {
                CHECK_VALUE(m[i][k], (i+1)*10 + (k+1));
            }
        }
    }
    LOG("* [operator=(double)]\n");
    {
        mcon::Matrixd m;
        CHECK_VALUE(m.IsNull(), true);
    }

    LOG("* [operator+=(double)]\n");
    {
        mcon::Matrixd m;
        CHECK_VALUE(m.IsNull(), true);
    }
}
