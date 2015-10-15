#include <stdio.h>

#include "Window.h"
#include "FileIo.h"
#include "Fft.h"

static void test_window(void)
{
    int SamplingRate = 8000;
    int PassBand = 500;
    int StopBand = 1500;
    int TransferBandWidth = StopBand - PassBand;
    int cutOff = PassBand + TransferBandWidth/2;
    double coef[256];
    size_t N = 24;

    mcon::Vector<double> s(N), han(N), ham(N), b(N), bh(N), n(N), k(N), f(N);
    masp::window::Hanning(coef, N);
    masp::window::Square(s);
    masp::window::Hanning(han);
    masp::window::Hamming(ham);
    masp::window::Blackman(b);
    masp::window::BlackmanHarris(bh);
    masp::window::Nuttall(n);
    masp::window::Kaiser(k, 3.0);
    masp::window::Flattop(f);
    printf(",Hanning,Hamming,Blackman,BlackmanHarris,Nuttall,Kaiser,Flattop,Square\n");
    for (int i = 0; i < N; ++i)
    {
        printf("%d,%f,%f,%f,%f,%f,%f,%f,%f\n", i, han[i], ham[i], b[i], bh[i], n[i], k[i], f[i], s[i]);
    }
    N = 25;
    s.Resize(N);
    han.Resize(N);
    ham.Resize(N);
    b.Resize(N);
    bh.Resize(N);
    n.Resize(N);
    k.Resize(N);
    f.Resize(N);
    masp::window::Hanning(coef, N);
    masp::window::Square(s);
    masp::window::Hanning(han);
    masp::window::Hamming(ham);
    masp::window::Blackman(b);
    masp::window::BlackmanHarris(bh);
    masp::window::Nuttall(n);
    masp::window::Kaiser(k, 3.0);
    masp::window::Flattop(f);
    printf("\n");
    printf(",Hanning,Hamming,Blackman,BlackmanHarris,Nuttall,Kaiser,Flattop,Square\n");
    for (int i = 0; i < N; ++i)
    {
        printf("%d,%f,%f,%f,%f,%f,%f,%f,%f\n", i, han[i], ham[i], b[i], bh[i], n[i], k[i], f[i], s[i]);
    }
    return ;
}

static void test_tapps(void)
{
    const int Ms[] = {48000};//16, 32, 64, 128, 256, 512, 1024};
    //mfio::Csv csv("hanning.csv");
    mfio::Csv csv("square.csv");

    for ( int i = 0; i < sizeof(Ms)/sizeof(int); ++i )
    {
        mcon::Vector<double> window(Ms[i]);
        mcon::Matrix<double> complex;
        mcon::Matrix<double> gp;
        //masp::window::Hanning(window);
        masp::window::Square(window);
        Fft::Ft(complex, window);
        Fft::ConvertToPolarCoords(gp, complex);
        csv.Write(gp[0]);
        csv.Write("\n");
    }
}

int main(void)
{
    //test_tapps();
    test_window();
    return 0;
}
