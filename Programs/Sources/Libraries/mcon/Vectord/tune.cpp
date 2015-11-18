#include "BenchUtil.h"

static void benchmark_vectord_api(void)
{
    printf("[Vector] START Benchmark\n");
    static const int length = 4*1024*1024;

    mcon::Vector<double> vec(length);

    mbut::Stopwatch sw;
    for (int i = 0; i < 1024; ++i)
    {
        mcon::Vector<double> vec2(vec);
    }
    printf("Vector<T>(Vector<T>)=%f\n", sw.Tick());

    printf("[Vecotr] FINISH Benchmark\n");
}
