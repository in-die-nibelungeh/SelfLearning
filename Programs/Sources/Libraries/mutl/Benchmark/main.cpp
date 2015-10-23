#include <stdio.h>

#include "Stopwatch.h"

#if 0
#include <chrono>

static void tst(void)
{
    auto start = std::chrono::system_clock::now();
    auto end = std::chrono::system_clock::now();
}
#endif

static void test_stopwatch(void)
{
    mutl::Stopwatch sw;
    double d = 0.0;
    for ( int i = 0; i < 1000000000; ++i )
    {
        d += i;
    }
    printf("Time consumed: %g\n", sw.Tick());
    printf("ans=%f\n", d);
}

int main(void)
{
    test_stopwatch();
    return 0;
}
