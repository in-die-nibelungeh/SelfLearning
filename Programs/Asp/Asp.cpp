#include <math.h>
#include "types.h"
#include "Asp.h"

const double Asp::g_Pi(M_PI);

void Asp::Sinc(double coef[], size_t N, double df)
{
    int offset = (N+1) & 1;
    for (int i = 0; i < N/2; ++i)
    {
        int idx = (i<<1) - N + offset;
        double x = idx * df / 2.0;
        double v = sin(x)/x;
        coef[i] = v;
        coef[N-i-1] = v;
    }
    if (0 == offset)
    {
        coef[N/2] = 1.0;
    }
    return ;
}

//void Asp::Lanczos(double coef[], size_t N, double df)
//   Sinc(x) * Sinc(x/N)
