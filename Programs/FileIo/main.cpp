#include <stdlib.h>
#include <stdio.h>

#include "FileIo.h"
#include "WaveGen.h"
#include "types.h"
#include "debug.h"

static void test_check(void)
{
    mfio::Wave wave;

    wave.Check();
}

static void test_read(void)
{
    int length;
    double* buffer;
    mfio::Wave wave;
    wave.Read("ding.wav", &buffer, &length);

    //printf("Sizeof(PcmFormat)=%d\n", sizeof(FileIo::PcmFormat));
    //printf("Sizeof(MetaData)=%d\n", sizeof(FileIo::MetaData));
    {
        int fs, ch, bit, fmt;
        wave.GetMetaData(&fs, &ch, &bit, &fmt);
        CHECK_VALUE( fs, 44100);
        CHECK_VALUE( ch,     2);
        CHECK_VALUE(bit,    16);
        CHECK_VALUE(fmt,   mfio::Wave::LPCM);
    }

    {
        struct mfio::Wave::MetaData metaData = wave.GetMetaData();

        CHECK_VALUE(metaData.samplingRate, 44100);
        CHECK_VALUE(metaData.numChannels ,     2);
        CHECK_VALUE(metaData.bitDepth    ,    16);
        CHECK_VALUE(metaData.format      , mfio::Wave::LPCM);
        CHECK_VALUE(length, 70016/(metaData.bitDepth/8));
    }
    /*----------------------------------------------------------------
     * Vector Interface
     *----------------------------------------------------------------*/
    mcon::Vector<double> bufferVector;
    mfio::Wave waveVector;
    waveVector.Read("ding.wav", bufferVector);
    {
        int32_t fs, ch, bit, fmt;
        waveVector.GetMetaData(&fs, &ch, &bit, &fmt);
        CHECK_VALUE( fs, 44100);
        CHECK_VALUE( ch,     2);
        CHECK_VALUE(bit,    16);
        CHECK_VALUE(bufferVector.GetLength(), 70016/(bit/8));
        CHECK_VALUE(fmt, mfio::Wave::LPCM);
    }
    LOG("Comparing data buffer (Vector):\n");
    int numUnequal = 0;
    for ( int i = 0; i < bufferVector.GetLength(); ++i )
    {
        if ( buffer[i] != bufferVector[i] )
        {
            ++numUnequal;
            LOG("Not equal at %5d: buffer=%g, bufferVector=%g\n",
                i, buffer[i], bufferVector[i]);
        }
    }
    CHECK_VALUE(numUnequal, 0);
    if ( 0 == numUnequal )
    {
        LOG("Matched completely (Vector)\n");
    }

    /*----------------------------------------------------------------
     * Matrix Interface
     *----------------------------------------------------------------*/
    mcon::Matrix<double> bufferObj;
    mfio::Wave waveMatrix;
    waveMatrix.Read("ding.wav", bufferObj);
    {
        int32_t fs, ch, bit, fmt;
        waveMatrix.GetMetaData(&fs, &ch, &bit, &fmt);
        CHECK_VALUE( fs, 44100);
        CHECK_VALUE( ch,     2);
        CHECK_VALUE(bit,    16);
        CHECK_VALUE(bufferObj.GetColumnLength(), 70016/ch/(bit/8));
        CHECK_VALUE(fmt, mfio::Wave::LPCM);
    }

    LOG("Comparing data buffer (Matrix):\n");
    numUnequal = 0;
    const int ch = waveMatrix.GetNumChannels();
    for ( int i = 0; i < bufferObj.GetColumnLength(); ++i )
    {
        for (int c = 0; c < ch; ++c)
        {
            const int idx = ch*i + c;
            if ( buffer[idx] != bufferObj[c][i] )
            {
                ++numUnequal;
                LOG("Not equal at %5d: buffer=%g, bufferObj=%g\n",
                    i, buffer[i], bufferObj[c][i]);
            }
        }
    }
    CHECK_VALUE(numUnequal, 0);
    if ( 0 == numUnequal )
    {
        LOG("Matched completely (Matrix)\n");
    }
    free(buffer);
}

static void test_write(void)
{
    WaveGen wg;
    int32_t freq = 440;
    int32_t fs = 48000;
    int32_t duration = 10;
    int32_t ch = 2;
    int32_t amp = 32767;
    int32_t multi = 20;
    const int length = duration * fs * ch;
    const size_t size = length * sizeof(double);
    double* buffer = (double*)malloc(size);
    mcon::Vector<double> bufferVector(length);
    mcon::Matrix<double> bufferMatrix(ch, length/ch);

    wg.SetWaveType(WaveGen::WT_SINE);
    wg.SetSamplingRate(fs);
    wg.SetWaveFrequency(freq);
    wg.Reset();
    wg.SetSweepParam(freq*multi, (double)duration, true);

    for (int i = 0; i < duration * fs; ++i, ++wg)
    {
        const double v = amp * wg.GetValue();
        for (int c = 0; c < ch; ++c)
        {
            buffer[ch*i+c] = v;
            bufferVector[ch*i+c] = v;
            bufferMatrix[c][i] = v;
        }
    }
    {
        char fname[256];
        int depth = sizeof(int16_t) * 8;
        mfio::Wave wave(fs, ch, depth);
        sprintf(fname, "sweep_%d-%d.wav", freq, freq*multi);
        wave.Write(reinterpret_cast<const char*>(fname), buffer, size);
        sprintf(fname, "sweep_%d-%d_vector.wav", freq, freq*multi);
        wave.Write(reinterpret_cast<const char*>(fname), bufferVector);
        sprintf(fname, "sweep_%d-%d_matrix.wav", freq, freq*multi);
        wave.Write(reinterpret_cast<const char*>(fname), bufferMatrix);
    }
    free(buffer);
}

static void test_read_float(void)
{
    const char* name = "101-st.wav";
    mfio::Wave wave;
    mcon::Matrix<double> buffer;
    status_t status = wave.Read(name, buffer);
    CHECK_VALUE(status, NO_ERROR);

    struct mfio::Wave::MetaData metaData = wave.GetMetaData();
    const int ch = metaData.numChannels;
    const int length = buffer.GetColumnLength();
    CHECK_VALUE(metaData.samplingRate, 44100);
    CHECK_VALUE(metaData.numChannels ,     2);
    CHECK_VALUE(metaData.bitDepth    ,    32);
    CHECK_VALUE(metaData.format      ,    mfio::Wave::IEEE_FLOAT);
    CHECK_VALUE(length, 143328/(metaData.bitDepth/8)/ch);

    for (int i = 0; i < length/1000; ++i)
    {
        LOG("buffer[%d][%d]=%g\n", 0, i, buffer[0][i]);
    }
}

static void test_write_float(void)
{
    const int ch = 2;
    const int length = 10;
    const char* name = "float.wav";
    mfio::Wave wave(48000, ch, 32, mfio::Wave::IEEE_FLOAT);
    mcon::Matrix<double> buffer(2, length);
    for (int i = 0; i < length; ++i)
    {
        for (int c = 0; c < ch; ++c)
        {
            buffer[c][i] = (c+1)*10+i;
        }
    }
    status_t status = wave.Write(name, buffer);
    CHECK_VALUE(status, NO_ERROR);
}


int main(void)
{
    test_check();
    test_read();
    test_write();
    test_read_float();
    test_write_float();

    return 0;
}
