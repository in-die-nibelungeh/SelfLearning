#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#include "Fft.h"
#include "WaveGen.h"

#define POW2(v) ((v)*(v))

#define FREQ_BASE 440
#define NUM_SAMPLES 4800

int main(void)
{
	int i;
	WaveGen wg;
	f64* buffer = NULL;
	f64* fft = NULL, *ifft = NULL;
	s32 fs = wg.GetSamplingRate();
	wg.SetWaveType(WaveGen::WT_SINE);
	wg.SetWaveFrequency(FREQ_BASE);
	wg.Reset();
	wg.SetSweepParam(FREQ_BASE*20, (f64)NUM_SAMPLES/fs, true);
	
	buffer = (f64*)malloc(sizeof(f64) * NUM_SAMPLES);
	
	for (i = 0; i < NUM_SAMPLES; ++i, ++wg)
	{
		WaveGen::Variable var = wg.GetVariable();
		buffer[i] = var.value;
		//printf("%f,%f\n", (f64)i/fs, var.value);
	}
	
	s32 n = NUM_SAMPLES;
	//printf("Ft\n");
	fft = (f64*)malloc(sizeof(f64) * n * 2);
	f64 *real = fft;
	f64 *imag = fft + n;
	Fft::Ft(real, imag, buffer, n);
	
	{
		f64 df = (f64)fs/NUM_SAMPLES;
		printf("freq,gain,phase\n");
		for (int i = 1; i < n/2; ++i)
		{
			f64 gain = 10 * log10(POW2(real[i]) + POW2(imag[i]));
			f64 phase = atan(real[i]/imag[i]);
			printf("%f,%f,%f\n", i*df, gain, phase);
		}
	}
	ifft = (f64*)malloc(sizeof(f64) * n * 2);
	f64 *td = ifft;
	f64 *td1 = ifft + n;
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
	
	return 0;
}
