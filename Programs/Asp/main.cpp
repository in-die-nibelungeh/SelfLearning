#include <stdio.h>
#include <stdlib.h>

#include "types.h"
#include "Asp.h"
#include "WaveGen.h"
#include "FileIo.h"
#include "Fir.h"

#if 0
static void test(void)
{
    // J + 1 is odd, so J is even;
    int SamplingRate = 8000;
    int PassBand = 1500;
    int StopBand = 2500;
    int TransferBandWidth = StopBand - PassBand;
    int cutOff = PassBand + TransferBandWidth/2;
    double delta = (double)cutOff/SamplingRate;
    int N = (3.1f / delta + 0.5) - 1;

    if ( (N%2) == 1 )
    {
        N += 1;
    }
    return ;
}
#endif

static void test_filtering(void)
{
    int SamplingRate = 48000;
    int freq = 440;
    int multiple = 20;
    int PassBand = 1500;
    int StopBand = 2500;
    int TransferBandWidth = StopBand - PassBand;
    int cutOff = PassBand + TransferBandWidth/2;
    double df = (double)cutOff/SamplingRate * 2;

    double *coef = NULL;
    size_t N = Asp::GetNumTapps(cutOff, SamplingRate);
    printf("N=%d\n", N);
    Container::Vector<double> w(N);

    coef = reinterpret_cast<double*>(malloc(N * sizeof(double)));

    if (coef == NULL)
    {
        printf("Couldn't allocate memory of coef or window with the size of %d\n",  N * sizeof(double));
        if (coef != NULL) { free(coef); }
        return ;
    }

    masp::fir::FilterSinc(coef, N, df);
    masp::window::Hanning(w);
    for (int i = 0; i < N; ++i)
    {
        //printf("%f,%f,%f\n", coef[i], w[i], 2.0 * CutOff / SamplingRate);
        coef[i] *= w[i] * 2 * cutOff / SamplingRate;
        //printf("coef[%3d] = %f\n", i, coef[i]);
    }

    Container::Vector<int16_t> audioIn(1);
    FileIo sweep;
    sweep.Read("sweep_440-8800.wav", audioIn);
    size_t numData = audioIn.GetNumOfData();
    DEBUG_LOG("numData=%d\n", numData);
    Container::Vector<int16_t> audioOut(numData);
    printf("Filtering\n");
    double max = 0.0;
    for (int i = 0; i < numData; ++i)
    {
        double ans = 0.0f;
        for (int j = 0; j < N; ++j)
        {
            ans += audioIn[i+j] * coef[N-1-j];
        }
        audioOut[i] = static_cast<int16_t>(ans);
    }
    free(coef);

    FileIo wave;
    wave.SetMetaData(SamplingRate, 1, 16);

    char fname[256];
    sprintf(fname, "sweep_%d-%d_filtered.wav", freq, freq * multiple);
    int err = wave.Write(fname, audioOut);
    if (err)
    {
        printf("Write return err=%d\n", err);
    }
    return ;
}

int main(void)
{
    test_filtering();
    return 0;
}
