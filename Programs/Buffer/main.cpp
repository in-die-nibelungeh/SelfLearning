#include <stdio.h>

#include "Buffer.h"


int main(void)
{
    int numData = 32;
    size_t size = 32;
    TBuffer<f64> iof64(numData * sizeof(f64));
    TBuffer<s16> ios16(numData * sizeof(s16));
    Buffer io(size);

    for (int i = 0; i < size/sizeof(f64); ++i)
    {
        printf("%d: %f\n", i, io[static_cast<f64>(i)]);
        io[static_cast<f64>(i)] = 1.0 * (i+1);
    }
    for (int i = 0; i < size/sizeof(int) + 1; ++i)
    {
        printf("%d: %08x\n", i, io[i]);
    }

    for (int i = 0; i < numData; ++i)
    {
        iof64[i] = (i+1);
        printf("%d: %f\n", i, iof64[i]);
    }

    ios16 = iof64;
    return 0;
}
