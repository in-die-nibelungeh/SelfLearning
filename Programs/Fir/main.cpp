#include <stdio.h>
#include <math.h>

#include "Buffer.h"
#include "Fir.h"
#include "Window.h"

static const double g_Pi(M_PI);

static void test_sinc(void)
{
    int SamplingRate = 8000;
    int PassBand =  500;
    int StopBand = 1500;
    int TransferBandWidth = StopBand - PassBand;
    int cutOff = PassBand + TransferBandWidth/2;
    double delta = (double)cutOff/SamplingRate * 2 * g_Pi;
    double fe = (double)cutOff/SamplingRate;
    double scoef[256];
    double lcoef[256];
    size_t N = 24;
    masp::fir::FilterSinc(scoef, N, fe);
    masp::fir::FilterLanczos(lcoef, N, fe, 2.0);
    printf("N,i,sinc,lanczos\n");
    for (int i = 0; i < N; ++i)
    {
        printf("%d,%d,%f,%f\n", N, i, scoef[i], lcoef[i]);
    }
    N = 25;
    masp::fir::FilterSinc(scoef, N, fe);
    masp::fir::FilterLanczos(lcoef, N, fe, 2.0);
    printf("\n");
    printf("N,i,sinc,lanczos\n");
    for (int i = 0; i < N; ++i)
    {
        printf("%d,%d,%f,%f\n", N, i, scoef[i], lcoef[i]);
    }
    return ;
}

int main(void)
{
    test_sinc();
    return 0;
}
