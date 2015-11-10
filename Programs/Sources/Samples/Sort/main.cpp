#include <stdio.h>
#include <stdint.h>

void sort(double d[], size_t n)
{
    for (uint i = 0; i < n; ++i)
    {
        for (uint k = 0; k < n - i - 1; ++k)
        {
            if (d[k] > d[k+1])
            {
                const double tmp = d[k];
                d[k] = d[k+1];
                d[k+1] = tmp;
            }
        }
    }
    return ;
}

int main(void)
{
    double darray[] =
    {
        1.0, -4, 3, 2, 5, 9, 11, -3, -2, -5, 6, 1.2, -10, 10, 10
    };
    const size_t size = sizeof(darray)/sizeof(double);

    sort(darray, size);

    for (uint k = 0; k < size; ++k)
    {
        printf("d[%d]=%g\n", k, darray[k]);
    }

    return 0;
}
