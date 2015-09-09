#include <stdio.h>

#include "Buffer.h"
#include "Fir.h"
#include "Window.h"

static const double g_Pi(M_PI);

static void test_sinc(void)
{
    int SamplingRate = 8000;
    int PassBand =  500;
    int StopBand = 1500;
    int UpperBand = 2000;
    int TransferBandWidth = StopBand - PassBand;
    int cutOff = PassBand + TransferBandWidth/2;
    double delta = (double)cutOff/SamplingRate * 2 * g_Pi;
    double fe = (double)cutOff/SamplingRate;
    double fe2 = (double)UpperBand/SamplingRate;
    double scoef[256];
    double lcoef[256];
    size_t N = 24;
    Container::Vector<double> slpf(N), shpf(N), sbpf(N), sbef(N), han(N);
    masp::window::Hanning(han);
    masp::fir::FilterSinc(scoef, N, fe);
    masp::fir::GetCoefficientsLpfSinc(slpf, fe);
    masp::fir::GetCoefficientsHpfSinc(shpf, fe);
    masp::fir::GetCoefficientsBpfSinc(sbpf, fe, fe2);
    masp::fir::GetCoefficientsBefSinc(sbef, fe, fe2);
    masp::fir::FilterLanczos(lcoef, N, fe, 2.0);

    slpf *= han;
    printf("N,i,sinc,sinc_n,lanczos\n");
    for (int i = 0; i < N; ++i)
    {
        printf("%d,%d,%f,%f,%f\n", N, i, scoef[i], slpf[i], lcoef[i]);
    }
    printf("\n");
    printf("N,i,slpf,shpf,sbpf,sbef\n");
    for (int i = 0; i < N; ++i)
    {
        printf("%d,%d,%f,%f,%f,%f\n", N, i, slpf[i], shpf[i], sbpf[i], sbef[i]);
    }
    N = 25;
    han.Resize(N);
    slpf.Resize(N);
    shpf.Resize(N);
    sbpf.Resize(N);
    sbef.Resize(N);
    masp::window::Hanning(han);
    masp::fir::FilterSinc(scoef, N, fe);
    masp::fir::GetCoefficientsLpfSinc(slpf, fe);
    masp::fir::GetCoefficientsHpfSinc(shpf, fe);
    masp::fir::GetCoefficientsBpfSinc(sbpf, fe, fe2);
    masp::fir::GetCoefficientsBefSinc(sbef, fe, fe2);
    masp::fir::FilterLanczos(lcoef, N, fe, 2.0);
    slpf *= han;
    printf("\n");
    printf("N,i,sinc,sinc_n,lanczos\n");
    for (int i = 0; i < N; ++i)
    {
        printf("%d,%d,%f,%f,%f\n", N, i, scoef[i], slpf[i], lcoef[i]);
    }
    printf("\n");
    printf("N,i,slpf,shpf,sbpf,sbef\n");
    for (int i = 0; i < N; ++i)
    {
        printf("%d,%d,%f,%f,%f,%f\n", N, i, slpf[i], shpf[i], sbpf[i], sbef[i]);
    }
    return ;
}

int main(void)
{
    test_sinc();
    return 0;
}
