#include <stdio.h>

#include "Window.h"

static void test_window(void)
{
    int SamplingRate = 8000;
    int PassBand = 500;
    int StopBand = 1500;
    int TransferBandWidth = StopBand - PassBand;
    int cutOff = PassBand + TransferBandWidth/2;
    double coef[256];
    size_t N = 24;

    mcon::Vector<double> han(N), ham(N), b(N), bh(N), n(N), k(N), f(N);
    masp::window::Hanning(coef, N);
    masp::window::Hanning(han);
    masp::window::Hamming(ham);
    masp::window::Blackman(b);
    masp::window::BlackmanHarris(bh);
    masp::window::Nuttall(n);
    masp::window::Kaiser(k, 3.0);
    masp::window::Flattop(f);
    printf(",Hanning,Hamming,Blackman,BlackmanHarris,Nuttall,Kaiser,Flattop\n");
    for (int i = 0; i < N; ++i)
    {
        printf("%d,%f,%f,%f,%f,%f,%f,%f\n", i, han[i], ham[i], b[i], bh[i], n[i], k[i], f[i]);
    }
    N = 25;
    han.Resize(N);
    ham.Resize(N);
    b.Resize(N);
    bh.Resize(N);
    n.Resize(N);
    k.Resize(N);
    f.Resize(N);
    masp::window::Hanning(coef, N);
    masp::window::Hanning(han);
    masp::window::Hamming(ham);
    masp::window::Blackman(b);
    masp::window::BlackmanHarris(bh);
    masp::window::Nuttall(n);
    masp::window::Kaiser(k, 3.0);
    masp::window::Flattop(f);
    printf("\n");
    printf(",Hanning,Hamming,Blackman,BlackmanHarris,Nuttall,Kaiser,Flattop\n");
    for (int i = 0; i < N; ++i)
    {
        printf("%d,%f,%f,%f,%f,%f,%f,%f\n", i, han[i], ham[i], b[i], bh[i], n[i], k[i], f[i]);
    }
    return ;
}

int main(void)
{
    test_window();
    return 0;
}
