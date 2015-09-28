#include "BenchUtil.h"

static void benchmark_vector_api(void)
{
    static const int length = 4*1024*1024;

    mcon::Vector<double> vec(length);

    BenchUtil bu;
    for (int i = 0; i < 1024; ++i)
    {
        mcon::Vector<double> vec2(vec);
    }
    printf("Vector<T>(Vector<T>)=%f\n", bu.Finish());

    printf("Benchmark END\n");
}
