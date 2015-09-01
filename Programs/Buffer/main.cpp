#include <stdio.h>

#include "Buffer.h"

using namespace Container;

int main(void)
{
    int numData = 8;
    Vector<f64> iof64(numData/2);
    Vector<s16> ios16(numData);

    for (int i = 0; i < iof64.GetNumOfData(); ++i)
    {
        iof64[i] = (i+1);
        printf("%d: %f\n", i, iof64[i]);
    }

    // Substituting
    ios16 = iof64;

    for (int i = 0; i < ios16.GetNumOfData(); ++i)
    {
        printf("%d: %d\n", i, ios16[i]);
    }

    // Copy-constructor
    Vector<f32> iof32(ios16);

    for (int i = 0; i < iof32.GetNumOfData(); ++i)
    {
        printf("%d: %f\n", i, iof32[i]);
    }

    int numArray = 2;
    Matrix<f64> m1(numArray, numData);

    for (int i = 0; i < numArray; ++i)
    {
        for (int j = 0; j < numData; ++j)
        {
            m1[i][j] = (i+1) * 8 + (j+1);
        }
    }

    Matrix<s16> m2(m1);

    return 0;
}
