#include <stdio.h>
#include <math.h>
#include <sys/types.h>

#include "Buffer.h"
#include "FileIo.h"
#include "Iir.h"
#include "Window.h"

static const double g_Pi(M_PI);

static void test_filtering(void)
{
    Container::Vector<int16_t> sweep(1);
    FileIo wave;
    wave.Read("sweep_440-8800.wav", sweep);
    struct FileIo::MetaData metaData = wave.GetMetaData();
    double Qf = 1 / sqrt(2);
    int fs = metaData.samplingRate;
    double fd = 1000;
    masp::iir::Biquad lpf(Qf, fd, 0.0, masp::iir::Biquad::FTI_LPF, fs);
    //lpf.CalculateCoefficients(Qf, fc, 0.0, masp::iir::Biquad::LPF, fs);

    Container::Vector<int16_t> sweep_filtered(sweep.GetNumOfData());

    printf("Fc=%f\n", masp::iir::Biquad::ConvertD2A(fd, fs));
    const struct masp::iir::Biquad::Coefficients coef = lpf.GetCoefficients();
    printf("b0=%f,b1=%f,b2=%f,a1=%f,a2=%f\n",
        coef.b[0], coef.b[1], coef.b[2], coef.a[0], coef.a[1]);

    for (int i = 0; i < sweep_filtered.GetNumOfData(); ++i)
    {
        double v = lpf.ApplyFilter(static_cast<double>(sweep[i]));
        if (i < 30)
            printf("%d,%d,%f\n", i, sweep[i], v);
        sweep_filtered[i] = static_cast<int16_t>(v);
    }
    wave.Write("sweep_440-8800_filtered.wav", sweep_filtered);
}

int main(void)
{
    test_filtering();
    return 0;
}
