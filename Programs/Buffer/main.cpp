#include <stdio.h>

#include "Buffer.h"

using namespace Container;

int main(void)
{
    int CheckPoint = 0;
    int numData = 8;
    Vector<f64> iof64(numData/2);
    Vector<s16> ios16(numData);

    printf("CheckPoint: %d\n", CheckPoint++);
    for (int i = 0; i < iof64.GetNumOfData(); ++i)
    {
        iof64[i] = (i+1);
        printf("%d: %f\n", i, iof64[i]);
    }

    // Substituting
    ios16 = iof64;

    printf("CheckPoint: %d\n", CheckPoint++);
    printf("ios16.GetNumOfData()=%d\n", ios16.GetNumOfData());
    for (int i = 0; i < ios16.GetNumOfData(); ++i)
    {
        printf("%d: %d\n", i, ios16[i]);
    }

    printf("CheckPoint: %d\n", CheckPoint++);
    // Copy-constructor
    Vector<f32> iof32(ios16);

    printf("CheckPoint: %d\n", CheckPoint++);
    for (int i = 0; i < iof32.GetNumOfData(); ++i)
    {
        printf("%d: %f\n", i, iof32[i]);
    }

    printf("CheckPoint: %d\n", CheckPoint++);
    int numArray = 2;
    Matrix<f64> m1(numArray, numData);

    for (int i = 0; i < m1.GetNumOfArray(); ++i)
    {
        for (int j = 0; j < m1.GetNumOfData(); ++j)
        {
            m1[i][j] = i * m1.GetNumOfData() + (j+1);
        }
    }

#define DUMP_MATRIX(m, t) \
    for (int i = 0; i < m.GetNumOfArray(); ++i) \
    {\
        for (int j = 0; j < m.GetNumOfData(); ++j)\
        {\
            printf(#m"[%d, %d] = "#t"\n", i, j, m[i][j]);\
        }\
    }

    DUMP_MATRIX(m1, %f);

    printf("CheckPoint: %d\n", CheckPoint++);
    Matrix<s16> m2(m1);

    printf("CheckPoint: %d\n", CheckPoint++);
    DUMP_MATRIX(m2, %d);

    Matrix<f32> m3(numArray/2, numData*2);

    printf("CheckPoint: %d\n", CheckPoint++);
    for (int i = 0; i < m3.GetNumOfArray(); ++i)
    {
        for (int j = 0; j < m3.GetNumOfData(); ++j)
        {
            m3[i][j] = (i+1) * 4 + (j+1) * 2;
            printf("m3[%d][%d]=%f\n", i, j, m3[i][j]);
        }
    }

    printf("CheckPoint: %d\n", __LINE__);
    m2 = m3;

    printf("CheckPoint: %d\n", __LINE__);
    DUMP_MATRIX(m2, %d);

    printf("CheckPoint: %d\n", __LINE__);
    Vector<f64> vec(m2[0]);

    printf("CheckPoint: %d\n", __LINE__);
    for (int i = 0; i < vec.GetNumOfData(); ++i)
    {
        printf("vec[%d]=%f\n", i, vec[i]);
    }
    return 0;
}
