#include <stdio.h>

#include "debug.h"
#include "Window.h"

static void test_hanning(void)
{
    int SamplingRate = 8000;
    int PassBand = 500;
    int StopBand = 1500;
    int TransferBandWidth = StopBand - PassBand;
    int cutOff = PassBand + TransferBandWidth/2;

    double coef[256];
    size_t N = 24;
    masp::window::Hanning(coef, N);
    for (int i = 0; i < N; ++i)
    {
        printf("N=%d: %d,%f\n", N, i, coef[i]);
    }
    N = 25;
    masp::window::Hanning(coef, N);
    for (int i = 0; i < N; ++i)
    {
        printf("N=%d: %d,%f\n", N, i, coef[i]);
    }
    return ;
}

int main(void)
{
    test_hanning();
    return 0;
}
