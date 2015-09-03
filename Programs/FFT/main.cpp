#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#include "debug.h"
#include "Fft.h"
#include "WaveGen.h"

#define POW2(v) ((v)*(v))

#define FREQ_BASE 440.0
#define NUM_SAMPLES 4800

static void test_ft(void)
{
    double* buffer = NULL;
    double* fft = NULL, *ifft = NULL;
    int fs = 48000;
    int n = NUM_SAMPLES;
    WaveGen wg(fs, FREQ_BASE, WaveGen::WT_SINE, 1.0);
    wg.Reset();
    wg.SetSweepParam(FREQ_BASE*20, (double)n/fs, true);

    buffer = (double*)malloc(sizeof(double) * n);
    for (int i = 0; i < n; ++i, ++wg)
    {
        WaveGen::Variable var = wg.GetVariable();
        buffer[i] = var.value;
        //printf("%f,%f\n", (double)i/fs, var.value);
    }
    LOG("Ft\n");
    fft = (double*)malloc(sizeof(double) * n * 2);
    double *real = fft;
    double *imag = fft + n;
    Fft::Ft(real, imag, buffer, n);

    {
        double df = (double)fs/n;
        printf("freq,gain,phase\n");
        for (int i = 1; i < n/2; ++i)
        {
            double gain = 10 * log10(POW2(real[i]) + POW2(imag[i]));
            double phase = atan(real[i]/imag[i]);
            printf("%f,%f,%f\n", i*df, gain, phase);
        }
    }
    ifft = (double*)malloc(sizeof(double) * n * 2);
    double *td = ifft;
    double *td1 = ifft + n;
    printf("Ift\n");
    Fft::Ift(td, real, imag, n);
    {
        printf("time,orig,td,td1\n");
        for (int i = 1; i < n/10; ++i)
        {
            printf("%d,%f,%f,%f\n", i, buffer[i],td[i], 0.0);
        }
    }

    free(buffer);
    free(fft);
    free(ifft);
}

static void test_ft_buffer(void)
{
    int fs = 48000;
    int n = NUM_SAMPLES;
    WaveGen wg(fs, FREQ_BASE, WaveGen::WT_SINE);
    wg.Reset();
    wg.SetSweepParam(FREQ_BASE*20, (double)n/fs, true);

    Container::Vector<double> buffer(n);

    wg.GenerateValue(buffer, n);

    Container::Matrix<double> fft(2, n);

    Fft::Ft(fft, buffer);

    {
        double df = (double)fs/n;
        Container::Matrix<double> gp(2, n/2);

        Fft::ConvertToGainPhase(gp, fft);

        printf("freq,gain,phase\n");
        for (int i = 1; i < n/2; ++i)
        {
            printf("%f,%f,%f\n", i*df, gp[0][i], gp[1][i]);
        }
    }
    Container::Vector<double> ifft(n);
    Fft::Ift(ifft, fft);
    {
        printf("time,orig,td,td1\n");
        for (int i = 1; i < n/10; ++i)
        {
            printf("%d,%f,%f,%f\n", i, buffer[i],ifft[i], 0.0);
        }
    }
}

int main(void)
{
    //test_ft();
    test_ft_buffer();

    return 0;
}
