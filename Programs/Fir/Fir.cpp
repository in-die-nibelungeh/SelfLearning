
#include <math.h>
#include <sys/types.h>

#include "debug.h"

namespace masp {
namespace fir {

static const double g_Pi(M_PI);

typedef double (*FirLpfType)(double, double);

enum FirLpfFunctionId
{
    FLFI_SINC = 1,
    FLFI_LANCZOS
};

double Sinc(double x)
{
    return (0.0 == x) ? 1.0 : sin(x)/x;
}

double SincFilterFunction(double x, double arg)
{
    return Sinc(x);
}

double LanczosFilterFunction(double x, double n)
{
    return Sinc(x) * Sinc(x/n);
}

FirLpfType GetFirLpfFunction(int type)
{
    FirLpfType function = NULL;
    switch(type)
    {
    case FLFI_SINC   : function = SincFilterFunction   ; break;
    case FLFI_LANCZOS: function = LanczosFilterFunction; break;
    }
    return function;
}


void GenerateLpfFilter(double coef[], size_t _N, double fe, int type, double arg)
{
    const int N = _N;
    FirLpfType function = GetFirLpfFunction(type);
    ASSERT(NULL != function);
    for (int i = 0; i < N/2; ++i)
    {
        const double x = 2.0 * fe * (2 * i + 1 - N) / 2.0 * g_Pi;
        double v = 2.0 * fe * function(x, arg);
        coef[i] = v;
        coef[N-i-1] = v;
    }
    if (1 == (N & 1))
    {
        coef[N/2] = 2 * fe;
    }
}


void FilterSinc(double coef[], size_t _N, double fe)
{
    GenerateLpfFilter(coef, _N, fe, FLFI_SINC, 0.0);
#if 0
    for (int i = 0; i < N/2; ++i)
    {
        const double x = 2.0 * fe * (2 * i + 1 - N) / 2.0 * g_Pi;
        double v = 2.0 * fe * Sinc(x);
        coef[i] = v;
        coef[N-i-1] = v;
    }
    if (1 == (N & 1))
    {
        coef[N/2] = 2 * fe;
    }
#endif
}

void FilterLanczos(double coef[], size_t _N, double fe, double n)
{
    if (n < 1.0)
    {
        n = 1.0;
    }
    GenerateLpfFilter(coef, _N, fe, FLFI_LANCZOS, n);
#if 0
    for (int i = 0; i < N/2; ++i)
    {
        const double x = 2.0 * fe * (2 * i + 1 - N) / 2.0 * g_Pi;
        double v = 2.0 * fe * Sinc(x) * Sinc(x/n);
        coef[i] = v;
        coef[N-i-1] = v;
    }
    if (1 == (N & 1))
    {
        coef[N/2] = 2 * fe;
    }
#endif
}

} // namespace fir {
} // namespace masp {
