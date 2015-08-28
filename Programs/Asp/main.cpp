#include "types.h"
#include "Asp.h"
#include <math.h>
#include <stdio.h>

f64 g_Pi = M_PI;

void Sinc(f64 coef[], size_t N, f64 df)
{
    int offset = (N+1) & 1;
    for (int i = 0; i < N/2; ++i)
    {
        int idx = (i<<1) - N + offset;
        f64 x = idx * df / 2.0;
        f64 v = sin(x)/x;
        coef[i] = v;
        coef[N-i-1] = v;
    }
    if (0 == offset)
    {
        coef[N/2] = 1.0;
    }
    return ;
}

#if 0
static void test(void)
{
    // J + 1 is odd, so J is even;
    int SamplingRate = 8000;
    int PassBand = 1500;
    int StopBand = 2500;
    int TransferBandWidth = StopBand - PassBand;
    int CutOff = PassBand + TransferBandWidth/2;
    f64 delta = (f64)TransferBandWidth/SamplingRate;
    int N = (3.1f / delta + 0.5) - 1;

    if ( (N%2) == 1 )
    {
        N += 1;
    }
    return ;
}
#endif

static void test_sinc(void)
{
    int SamplingRate = 8000;
    int PassBand = 1500;
    int StopBand = 2500;
    int TransferBandWidth = StopBand - PassBand;
    int CutOff = PassBand + TransferBandWidth/2;
    f64 delta = (f64)TransferBandWidth/SamplingRate * 2 * g_Pi;

    f64 coef[256];
    size_t N = 24;
    Sinc(coef, N, delta);

    for (int i = 0; i < N; ++i)
    {
        printf("N=%d: %d,%f\n", N, i, coef[i]);
    }
    N = 25;
    Sinc(coef, N, delta);
    for (int i = 0; i < N; ++i)
    {
        printf("N=%d: %d,%f\n", N, i, coef[i]);
    }
    return ;
}

static void test_hanning(void)
{
    int SamplingRate = 8000;
    int PassBand = 1500;
    int StopBand = 2500;
    int TransferBandWidth = StopBand - PassBand;
    int CutOff = PassBand + TransferBandWidth/2;
    f64 delta = (f64)TransferBandWidth/SamplingRate * 2 * g_Pi;

    f64 coef[256];
    size_t N = 24;
    Window::Hanning(coef, N);

    for (int i = 0; i < N; ++i)
    {
        printf("N=%d: %d,%f\n", N, i, coef[i]);
    }
    N = 25;
    Window::Hanning(coef, N);
    for (int i = 0; i < N; ++i)
    {
        printf("N=%d: %d,%f\n", N, i, coef[i]);
    }
    return ;
}

int main(void)
{
    //test_sinc();
    test_hanning();
    return 0;
}
