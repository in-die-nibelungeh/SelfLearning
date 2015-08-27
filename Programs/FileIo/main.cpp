#include <stdlib.h>
#include <stdio.h>

#include "FileIo.h"
#include "WaveGen.h"
#include "types.h"


static void test_test(void)
{
	FileIo wave;
	
	wave.Test();
}

static void test_read(void)
{
	FileIo wave;
	wave.Read("ding.wav");
	s32 fs;
	s32 ch, bit;
	
	wave.GetMetaData(&fs, &ch, &bit);
	
	printf("fs =%5d: %s\n",  fs,  fs == 44100 ? "OK" : "NG");
	printf("ch =%5d: %s\n",  ch,  ch ==     2 ? "OK" : "NG");
	printf("bit=%5d: %s\n", bit, bit ==    16 ? "OK" : "NG");
}

static void test_write(void)
{
	WaveGen wg;
	s32 freq = 440;
	s32 fs = 48000;
	s32 duration = 10;
	s32 ch = 1;
	s32 depth = sizeof(s16) * 8;
	s32 amp = 32767;
	s32 multi = 20;
	size_t size = duration * fs * ch * depth / 8;
	s16* buffer = (s16*)malloc(size);

	wg.SetWaveType(WaveGen::WT_SINE);
	wg.SetSamplingRate(fs);
	wg.SetWaveFrequency(freq);
	wg.Reset();
	wg.SetSweepParam(freq*multi, (f64)duration, true);
	
	for (int i = 0; i < duration * fs; ++i, ++wg)
	{
		buffer[i] = static_cast<s16>(amp * wg.GetValue());
	}
	
	{
		char fname[256];
		FileIo wave;
		wave.SetMetaData(fs, ch, depth);
		wave.SetAudioData(buffer, size);
		sprintf(fname, "sweep_%d-%d.wav", freq, freq*multi);
		wave.Write(fname);
	}
	free(buffer);
}

int main(void)
{
	test_test();
	//test_read();
	test_write();
	
	return 0;
}
