#include <stdio.h>

#include "BenchUtil.h"

static void test_time_measure(void)
{
    BenchUtil bu;
    double d = 0.0;
    for ( int i = 0; i < 1000000000; ++i )
    {
        d += i;
    }
    printf("ans=%f (%f) \n", d, bu.Finish());
}

int main(void)
{
    test_time_measure();
    return 0;
}
