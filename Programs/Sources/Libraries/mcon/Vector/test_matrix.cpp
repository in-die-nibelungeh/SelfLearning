#include "mcon.h"

#define Type double

void test_matrix_api(void)
{
    const uint length = 6;
    mcon::Vector<Type> dvec(length);
    for (uint i = 0; i < length; ++i)
    {
        dvec[i] = i + 1;
    }
    mcon::Matrix<Type> m;

    CHECK_VALUE(m.IsNull(), true);

    m = dvec.Transpose();

    CHECK_VALUE(m.GetRowLength(), length);
    CHECK_VALUE(m.GetColumnLength(), 1);
    for (uint i = 0; i < length; ++i)
    {
        CHECK_VALUE(m[i][0], i+1);
    }
    mcon::Matrix<Type> m1(dvec.ToMatrix());

    CHECK_VALUE(m1.GetRowLength(), 1);
    CHECK_VALUE(m1.GetColumnLength(), length);
    for (uint i = 0; i < length; ++i)
    {
        CHECK_VALUE(m1[0][i], i+1);
    }
}
