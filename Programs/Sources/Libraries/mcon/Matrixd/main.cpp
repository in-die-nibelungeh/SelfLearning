
#include "mcon.h"

extern void test_Matrixd(void);
extern void benchmark_Matrixd(void);

int main(void)
{
    test_Matrixd();
    benchmark_Matrixd();

    return 0;
}
