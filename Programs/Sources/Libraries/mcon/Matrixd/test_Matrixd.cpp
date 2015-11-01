
#include "VectordBase.h"
#include "Vectord.h"
#include "Matrixd.h"

void test_Matrixd(void)
{
    int length = 6;
    mcon::Vectord dvec(length);
    for (int i = 0; i < length; ++i)
    {
        dvec[i] = i + 1;
    }
    mcon::Matrixd m;

    CHECK_VALUE(m.IsNull(), true);
}
