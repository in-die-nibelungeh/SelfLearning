#include <stdio.h>

#include "Window.h"
#include "FileIo.h"
#include "Fft.h"
#include "Fir.h"

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
    masp::window::Rectangular(s);
    masp::window::Hanning(han);
    masp::window::Hamming(ham);
    masp::window::Blackman(b);
    masp::window::BlackmanHarris(bh);
    masp::window::Nuttall(n);
    masp::window::Kaiser(k, 3.0);
    masp::window::Flattop(f);
    printf(",Hanning,Hamming,Blackman,BlackmanHarris,Nuttall,Kaiser,Flattop,Rectangular\n");
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
    masp::window::Rectangular(s);
    masp::window::Hanning(han);
    masp::window::Hamming(ham);
    masp::window::Blackman(b);
    masp::window::BlackmanHarris(bh);
    masp::window::Nuttall(n);
    masp::window::Kaiser(k, 3.0);
    masp::window::Flattop(f);
    printf("\n");
    printf(",Hanning,Hamming,Blackman,BlackmanHarris,Nuttall,Kaiser,Flattop,Rectangular\n");
    for (int i = 0; i < N; ++i)
    {
        printf("%d,%f,%f,%f,%f,%f,%f,%f,%f\n", i, han[i], ham[i], b[i], bh[i], n[i], k[i], f[i], s[i]);
    }
    return ;
}

static void test_tapps(void)
{
    const int Ms[] = {2, 4, 8, 16, 32, 64, 128};
    const int baseM = 1024;
    //mfio::Csv csv("hanning.csv");
    //mfio::Csv csv("rectangular.csv");
    mfio::Csv csv("sinc.csv");

    for ( int i = 0; i < sizeof(Ms)/sizeof(int); ++i )
    {
        const double fe = 0.25;
        const int N = Ms[i] + 1;
        mcon::Vector<double> sinc(N);
        mcon::Vector<double> window(N);
        mcon::Vector<double> response(baseM);
        masp::fir::GetCoefficientsLpfSinc(sinc, fe);
        mcon::Matrix<double> complex;
        mcon::Matrix<double> gp;
        //masp::window::Hanning(window);
        masp::window::Rectangular(window);
        response = 0;
        window *= sinc;
        response.Copy(window);
        for ( int i = 0; i < (baseM - N)/2; ++i )
        {
            response.Unshift(0);
        }

        Fft::Ft(complex, response);
        Fft::ConvertToPolarCoords(gp, complex);
        gp[0] /= gp[0].GetMaximumAbsolute();
        {
            const int n = response.GetLength();
            mcon::Matrix<double> matrix(3, n);
            matrix[1] = gp[0];
            for ( int k = 0; k < n; ++k )
            {
                matrix[0][k] = k * 1.0 / n;
                //matrix[1][k] = 20 * log10(matrix[1][k]);
            }
            matrix[2] = response;
            csv.Write(matrix);
            csv.Crlf();
        }
        //mfio::Csv::Write("fft.csv", gp[0]);
    }
}

int main(void)
{
    test_tapps();
    test_window();
    return 0;
}
