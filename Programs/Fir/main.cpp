#include <stdio.h>
#include <string>

#include "Fft.h"
#include "Fir.h"
#include "Window.h"
#include "FileIo.h"
#include "Vector.h"

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
    mcon::Vector<double> slpf(N), shpf(N), sbpf(N), sbef(N), han(N);
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

static void test_src(void)
{
    const std::string ewav(".wav");
    const std::string ecsv(".csv");
    const std::string fbody("sweep_440-3520_3s");

    mcon::Vector<double> sweep;
    mfio::Wave wave;
    wave.Read(fbody + ewav, sweep);

    {
        const int baseFs = wave.GetSamplingRate();
        const int targetFs = baseFs / 2;
        const double ratio = targetFs / static_cast<double>(baseFs); // 0.5

        const int baseLength = sweep.GetLength();
        const int targetLength = static_cast<int>(baseLength * ratio + 0.5);
        // カットオフ周波数は伸縮に合わせる
        const double cutoffRatio = 0.45 * ratio;
        const double passbandRatio = 0.1 * ratio;

        const int M = masp::fir::GetNumOfTapps(passbandRatio);
        printf("cutoff=%f\n", cutoffRatio);
        printf("passband=%f\n", passbandRatio);
        printf("M=%d\n", M);
        mcon::Vector<double> w(M);
        mcon::Vector<double> sinc(M);
        mcon::Vector<double> sweepDown(targetLength);

        masp::window::Hanning(w);
        masp::fir::GetCoefficientsLpfSinc(sinc, cutoffRatio);

        w *= sinc;
        {
            mcon::Matrix<double> complex(2, M);
            mcon::Matrix<double> polar(2, M);
            Fft::Ft(complex, w);
            Fft::ConvertToPolarCoords(polar, complex);
            mcon::Matrix<double> matrix(3, M);
            matrix[0] = w;
            matrix[1] = polar[0];
            matrix[2] = polar[1];

            mfio::Csv::Write("hanning_sinc.csv", matrix);
        }

        for (int i = 0; i < sweepDown.GetLength(); ++i)
        {
            double v = 0.0;
            for (int k = 0; k < M; ++k)
            {
                v += w[k] * sweep[static_cast<int>(i/ratio)+(M+1)/2-k];
            }
            sweepDown[i] = v;
        }
        wave.SetNumChannels(1);
        wave.SetSamplingRate(targetFs);
        wave.Write(fbody + std::string("_down") + ewav, sweepDown);
    }
}

void test_upsample(void)
{
    const std::string ewav(".wav");
    const std::string ecsv(".csv");
    const std::string fbody("sweep_440-3520_3s");

    mcon::Vector<double> sweep;
    mfio::Wave wave;
    wave.Read(fbody + ewav, sweep);

    {
        const int baseFs = wave.GetSamplingRate();
        const int targetFs = baseFs * 2;
        const int ratio = targetFs / static_cast<int>(baseFs); // 2

        const int baseLength = sweep.GetLength();
        const int targetLength = static_cast<int>(baseLength * ratio + 0.5);
        // カットオフ周波数は、ベース周波数で考えれば良いのだが、
        // 補間計算で係数に ratio 刻みでアクセスする場合サンプリング周波数が
        // 変わってしまうので、予め ratio で割っておく。
        // 例えば、48kHz にてカットオフ周波数 20kHz で設計したフィルタ係数に
        // 2 刻みでアクセスすると、カットオフ 40 kHz のフィルタに相当する性能になる。
        const double cutoffRatio = 0.45 / ratio;
        // パスバンドもベース周波数で考えれば良い。
        const double passbandRatio = 0.1 / ratio;

        const int M = masp::fir::GetNumOfTapps(passbandRatio) * ratio;
        printf("cutoff=%f\n", cutoffRatio);
        printf("passband=%f\n", passbandRatio);
        printf("M=%d\n", M);
        mcon::Vector<double> w(M);
        mcon::Vector<double> sinc(M);
        mcon::Vector<double> sweepUp(targetLength);

        masp::window::Hanning(w);
        masp::fir::GetCoefficientsLpfSinc(sinc, cutoffRatio);

        w *= sinc;
        {
            mcon::Matrix<double> complex(2, M);
            mcon::Matrix<double> polar(2, M);
            Fft::Ft(complex, w);
            Fft::ConvertToPolarCoords(polar, complex);
            mcon::Matrix<double> matrix(3, M);
            matrix[0] = w;
            matrix[1] = polar[0];
            matrix[2] = polar[1];

            mfio::Csv::Write("hanning_sinc_up.csv", matrix);
        }

        for (int i = 0; i < sweepUp.GetLength(); ++i)
        {
            double v = 0.0;
            for (int k = i % ratio; k < M; k += ratio)
            {
                v += w[k] * sweep[i/ratio+(M+1)/2-k/ratio];
            }
            sweepUp[i] = ratio * v;
        }
        wave.SetNumChannels(1);
        wave.SetSamplingRate(targetFs);
        wave.Write(fbody + std::string("_up") + ewav, sweepUp);
    }
}

static void test_minimumXXX(void)
{
    int X = 8000;//32728;
    int Y = 32000;//32000

    int x = X;
    int y = Y;
    int smaller = x > y ? y : x;
    int e[32] = {0,};
    int n = 0;

    for (int i = 2; i < smaller/2; ++i)
    {
        if ( (x % i) == 0 && (y % i) == 0 )
        {
            x /= i;
            y /= i;
            e[n] = i;
            ++n;
            i = 1; // reset
        }
    }
    printf("x=%d, y=%d\n", x, y);
    for (int i = 0; i < n; ++i)
    {
        printf("e[%d]=%d\n", i, e[i]);
    }
}

int main(void)
{
    test_minimumXXX();
    return 0;
    // Primary
    if (0)
    {
        const int value = 738414;
        int tmp = value;

        while ( tmp != 1 )
        {
            printf("searching for %d ...\n", tmp);
            int n;
            for (n = 2; n < tmp/n; ++n)
            {
                if ( (tmp % n) == 0 )
                {
                    printf("%d is \n", n);
                    tmp /= n;
                    break;
                }
            }
            if (n == tmp/2)
            {
                tmp /= n;
            }
        }
        return 0;
    }
    //test_sinc();
    //test_src();
    test_upsample();
    return 0;
}
