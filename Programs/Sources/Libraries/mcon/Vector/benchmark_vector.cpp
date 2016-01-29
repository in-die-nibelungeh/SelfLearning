#include "mutl.h"
#include "mcon.h"

void benchmark_vector_api(void)
{
    printf("[Vector] START Benchmark\n");
    static const int length = 4*1024*1024;

    mcon::Vector<TestType> vec(length);

    mutl::Stopwatch sw;
    for (int i = 0; i < 1024; ++i)
    {
        mcon::Vector<TestType> vec2(vec);
    }
    printf("Vector<T>(Vector<T>)=%f\n", sw.Tick());

    printf("[Vecotr] FINISH Benchmark\n");
}
