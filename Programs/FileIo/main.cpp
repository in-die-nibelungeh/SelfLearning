#include <stdlib.h>
#include <stdio.h>

#include "FileIo.h"
#include "WaveGen.h"
#include "types.h"
#include "debug.h"

static void test_test(void)
{
    mfio::Wave wave;

    wave.Check();
}

static void test_read(void)
{
    size_t size;
    int16_t* buffer;
    mfio::Wave wave;
    wave.Read("ding.wav", &buffer, &size);

    //printf("Sizeof(PcmFormat)=%d\n", sizeof(FileIo::PcmFormat));
    //printf("Sizeof(MetaData)=%d\n", sizeof(FileIo::MetaData));
    {
        int32_t fs, ch, bit;
        wave.GetMetaData(&fs, &ch, &bit);
        CHECK_VALUE_INT( fs, 44100);
        CHECK_VALUE_INT( ch,     2);
        CHECK_VALUE_INT(bit,    16);
    }

    {
        struct mfio::Wave::MetaData metaData = wave.GetMetaData();

        CHECK_VALUE_INT(metaData.samplingRate, 44100);
        CHECK_VALUE_INT(metaData.numChannels ,     2);
        CHECK_VALUE_INT(metaData.bitDepth    ,    16);
        CHECK_VALUE_INT(size, 70016);
    }

    // îÒÉ[ÉçÇ≈Ç†ÇÍÇŒâΩÇ≈Ç‡ó«Ç¢ÅB
    mcon::Vector<int16_t> bufferObj(1);
    mfio::Wave waveBuf;
    waveBuf.Read("ding.wav", bufferObj);
    {
        int32_t fs, ch, bit;
        waveBuf.GetMetaData(&fs, &ch, &bit);
        CHECK_VALUE_INT( fs, 44100);
        CHECK_VALUE_INT( ch,     2);
        CHECK_VALUE_INT(bit,    16);
        CHECK_VALUE_INT(bufferObj.GetLength(), 70016/2);
    }

    DEBUG_LOG("Comparing data buffer:\n");
    int numUnequal = 0;
    for ( int i = 0; i < bufferObj.GetLength(); ++i )
    {
        if ( buffer[i] != bufferObj[i] )
        {
            ++numUnequal;
            printf("Not equal at %5d: buffer=%d, bufferObj=%d\n",
                i, buffer[i], bufferObj[i]);
        }
    }
    if ( 0 == numUnequal )
    {
        DEBUG_LOG("Matched completely\n");
    }
}

static void test_write(void)
{
    WaveGen wg;
    int32_t freq = 440;
    int32_t fs = 48000;
    int32_t duration = 10;
    int32_t ch = 1;
    int32_t depth = sizeof(int16_t) * 8;
    int32_t amp = 32767;
    int32_t multi = 20;
    size_t size = duration * fs * ch * depth / 8;
    int16_t* buffer = (int16_t*)malloc(size);
    mcon::Vector<int16_t> bufferObj(size/sizeof(int16_t));

    wg.SetWaveType(WaveGen::WT_SINE);
    wg.SetSamplingRate(fs);
    wg.SetWaveFrequency(freq);
    wg.Reset();
    wg.SetSweepParam(freq*multi, (double)duration, true);

    for (int i = 0; i < duration * fs; ++i, ++wg)
    {
        buffer[i] = static_cast<int16_t>(amp * wg.GetValue());
        bufferObj[i] = buffer[i];
    }

    {
        char fname[256];
        mfio::Wave wave(fs, ch, depth);
        sprintf(fname, "sweep_%d-%d.wav", freq, freq*multi);
        wave.Write(reinterpret_cast<const char*>(fname), buffer, size);
        sprintf(fname, "sweep_%d-%d_buffer.wav", freq, freq*multi);
        wave.Write(reinterpret_cast<const char*>(fname), bufferObj);
    }
    free(buffer);
}

int main(void)
{
    test_test();
    test_read();
    test_write();

    return 0;
}
