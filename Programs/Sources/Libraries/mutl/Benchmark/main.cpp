#include <stdio.h>

#include "Stopwatch.h"

double g_Value = 0.0;

static void test_stopwatch(void)
{
    mutl::Stopwatch sw;
    double d = 0.0;
    for ( int i = 0; i < 1000000000; ++i )
    {
        d += i;
    }
    printf("Time consumed: %g\n", sw.Tick());
    g_Value = d;
}

int main(void)
{
    test_stopwatch();
    return 0;
}
