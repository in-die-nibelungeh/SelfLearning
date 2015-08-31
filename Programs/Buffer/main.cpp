#include <stdio.h>

#include "Buffer.h"

int main(void)
{
    int numData = 8;
    Buffer<f64> iof64(numData/2);
    Buffer<s16> ios16(numData);

    for (int i = 0; i < iof64.GetNumOfData(); ++i)
    {
        iof64[i] = (i+1);
        printf("%d: %f\n", i, iof64[i]);
    }

    ios16 = iof64;

    for (int i = 0; i < ios16.GetNumOfData(); ++i)
    {
        printf("%d: %d\n", i, ios16[i]);
    }

    return 0;
}
