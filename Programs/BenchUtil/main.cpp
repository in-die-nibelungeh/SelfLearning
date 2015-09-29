#include <stdio.h>

#include "BenchUtil.h"

static void test_stopwatch(void)
{
    mbut::Stopwatch sw;
    double d = 0.0;
    for ( int i = 0; i < 1000000000; ++i )
    {
        d += i;
    }
    printf("Time consumed: %g\n", sw.Push());
    printf("ans=%f\n", d);
}

int main(void)
{
    test_stopwatch();
    return 0;
}
