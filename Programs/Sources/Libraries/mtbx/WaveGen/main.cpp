#include <stdio.h>
#include <math.h>
#include "mtbx.h"

#define NUM_SAMPLES 120
#define NUM_TYPES 4

using namespace mtbx;

static int test_wavetype(void)
{
    int i, j;
    double buffer[NUM_TYPES][NUM_SAMPLES];

    WaveGen wg;
    WaveGen::WaveType types[] =
    {
        WaveGen::WT_SINE,
        WaveGen::WT_SQUARE,
        WaveGen::WT_TRIANGLE,
        WaveGen::WT_SAWTOOTH
    };

    wg.SetWaveFrequency(440);

    for (j = 0; j < NUM_TYPES; ++j)
    {
        wg.SetWaveType(types[j]);
        wg.Reset();

        for (i = 0; i < NUM_SAMPLES; ++i, ++wg)
        {
            buffer[j][i] = wg.GetValue();
        }
    }
    double dt = 1.0f / wg.GetSamplingRate();
    for (i = 0; i < NUM_SAMPLES; ++i)
    {
        printf("%d,%f,%f,%f,%f,%f\n",
            i, dt * i, buffer[0][i], buffer[1][i], buffer[2][i], buffer[3][i]);
    }
    return 0;
}

#undef NUM_SAMPLES
#undef NUM_TYPES
#define NUM_TYPES 3
#define NUM_SAMPLES 480

#define FREQ_BASE 440

static int test_sweep(void)
{
    int i, j;
    double buffer[NUM_TYPES][NUM_SAMPLES];

    WaveGen wg;
    wg.SetWaveType(WaveGen::WT_SINE);
    wg.SetWaveFrequency(FREQ_BASE);

    for (j = 0; j < NUM_TYPES-1; ++j)
    {
        wg.Reset();
        if (j == 1)
        {
            wg.SetSweepParam(FREQ_BASE*2, 0.01, true);
        }

        for (i = 0; i < NUM_SAMPLES; ++i, ++wg)
        {
            WaveGen::Variable var = wg.GetVariable();
            buffer[j][i] = var.value;
            buffer[j+1][i] = var.frequency;
        }
    }
    double dt = 1.0f / wg.GetSamplingRate();

    for (i = 0; i < NUM_SAMPLES; ++i)
    {
        printf("%d,%f,%f,%f,%f\n",
            i, dt * i, buffer[0][i], buffer[1][i], buffer[2][i]);
    }
    return 0;
}

// Inverse and Reverse options are removed.
#undef NUM_SAMPLES
#undef NUM_TYPES
#define NUM_TYPES 4
#define NUM_SAMPLES 480

#define FREQ_BASE 440

static int test_invrev(void)
{
    int i, j;
    double buffer[NUM_TYPES][NUM_SAMPLES];

    WaveGen wg;
    wg.SetWaveType(WaveGen::WT_SAWTOOTH);
    wg.SetWaveFrequency(FREQ_BASE);

    j = 0;
    wg.Reset();
    for (i = 0; i < NUM_SAMPLES; ++i, ++wg)
    {
        buffer[j][i] = wg.GetValue();
    }
    j = 1;
    wg.Reset();
    for (i = 0; i < NUM_SAMPLES; ++i, --wg)
    {
        buffer[j][i] = wg.GetValue();
    }
    j = 2;
    wg.Reset();
    for (i = 0; i < NUM_SAMPLES; ++i, ++wg)
    {
        buffer[j][i] = -wg.GetValue();
    }
    j = 3;
    wg.Reset();
    for (i = 0; i < NUM_SAMPLES; ++i, --wg)
    {
        buffer[j][i] = -wg.GetValue();
    }

    double dt = 1.0f / wg.GetSamplingRate();

    printf("TI,Time,Normal,Decrement,Inverser,Dec&Inv\n");
    for (i = 0; i < NUM_SAMPLES; ++i)
    {
        printf("%d,%f,%f,%f,%f,%f\n",
            i, dt * i, buffer[0][i], buffer[1][i], buffer[2][i], buffer[3][i]);
    }
    return 0;
}

#undef NUM_SAMPLES
#undef NUM_TYPES
#define NUM_TYPES 2
#define NUM_SAMPLES 480

#define FREQ_BASE 440

static int test_opedec(void)
{
    int i, j;
    double buffer[NUM_TYPES][NUM_SAMPLES];

    WaveGen wg;
    wg.SetWaveType(WaveGen::WT_SAWTOOTH);
    wg.SetWaveFrequency(FREQ_BASE);

    for (j = 0; j < NUM_TYPES; ++j)
    {
        wg.Reset();

        if (j == 0)
        {
            for (i = 0; i < NUM_SAMPLES; ++i, ++wg)
            {
                buffer[j][i] = wg.GetValue();
            }
        }
        else // Time goes to the negative direction.
        {
            for (i = 0; i < NUM_SAMPLES; ++i, --wg)
            {
                buffer[j][i] = wg.GetValue();
            }
        }
    }
    double dt = 1.0f / wg.GetSamplingRate();

    printf("TI,Time,Normal,Decrement\n");
    for (i = 0; i < NUM_SAMPLES; ++i)
    {
        printf("%d,%f,%f,%f\n",
            i, dt * i, buffer[0][i], buffer[1][i]);
    }
    return 0;
}

#undef NUM_SAMPLES
#undef NUM_TYPES
#define NUM_TYPES 2
#define NUM_SAMPLES 4800

#define FREQ_BASE 440

static int test_2incs(void)
{
    int i, j;
    double buffer[NUM_TYPES][NUM_SAMPLES];

    WaveGen wg;
    wg.SetWaveType(WaveGen::WT_SAWTOOTH);
    wg.SetWaveFrequency(FREQ_BASE);

    for (j = 0; j < NUM_TYPES; ++j)
    {
        wg.Reset();

        if (j == 0)
        {
            for (i = 0; i < NUM_SAMPLES; ++i)
            {
                buffer[j][i] = (++wg).GetValue();
            }
        }
        else // Time goes to the negative direction.
        {
            for (i = 0; i < NUM_SAMPLES; ++i)
            {
                buffer[j][i] = (wg++).GetValue();
            }
        }
    }
    bool result = true;

    for (i = 1; i < NUM_SAMPLES-1; ++i)
    {
        if (buffer[0][i] != buffer[1][i+1])
        {
            result = false;
        }
    }
    double dt = 1.0f / wg.GetSamplingRate();

    printf("\"TI\",\"Time\",=\"++wg\",\"wg++\"\n");
    for (i = 0; i < NUM_SAMPLES; ++i)
    {
        printf("%d,%f,%f,%f\n",
            i, dt * i, buffer[0][i], buffer[1][i]);
    }

    printf("Result: %s\n", result ? "OK" : "NG");

    return 0;
}

#undef NUM_SAMPLES
#undef NUM_TYPES
#define NUM_TYPES 3
#define NUM_SAMPLES 480

#define FREQ_BASE 440

static int test_indexing(void)
{
    int i, j;
    double buffer[NUM_TYPES][NUM_SAMPLES];

    WaveGen wg;
    wg.SetWaveType(WaveGen::WT_SAWTOOTH);
    wg.SetWaveFrequency(FREQ_BASE);

    for (j = 0; j < NUM_TYPES; ++j)
    {
        wg.Reset();

        if (j == 0)
        {
            for (i = 0; i < NUM_SAMPLES; ++i, ++wg)
            {
                buffer[j][i] = wg.GetValue();
            }
        }
        else if (j == 1) // Time goes to the negative direction.
        {
            for (i = 0; i < NUM_SAMPLES; ++i)
            {
                buffer[j][i] = (wg + i).GetValue();
            }
        }
        else // Time goes to the negative direction.
        {
            for (i = 0; i < NUM_SAMPLES; ++i, wg += 1)
            {
                buffer[j][i] = wg.GetValue();
            }
        }
    }
#if 0
    bool result = true;

    for (i = 1; i < NUM_SAMPLES-1; ++i)
    {
        if (buffer[0][i] != buffer[1][i+1])
        {
            result = false;
        }
    }
#endif
    double dt = 1.0f / wg.GetSamplingRate();

    printf("\"TI\",\"Time\",=\"++wg\",=\"(wg + i)\",\"wg += 1\"\n");
    for (i = 0; i < NUM_SAMPLES; ++i)
    {
        printf("%d,%f,%f,%f,%f\n",
            i, dt * i, buffer[0][i], buffer[1][i], buffer[2][i]);
    }

    return 0;
}

static int test_(void)
{
    WaveGen wg;
    wg = wg + 3;
    wg = wg - 4;
    wg += 2;
    return 0;
}

static void test_buffer(void)
{
    float duration = 0.01;
    int samplingRate = 48000;
    double freq = 440;
    WaveGen wg(samplingRate, freq, WaveGen::WT_SINE);
    const uint numData = static_cast<int>(duration * samplingRate);
    wg.Reset();

    mcon::Vector<double> buffer(numData);

    wg.GenerateWaveform(buffer);

    for (uint i = 0; i < numData; ++i)
    {
        printf("%d,%f\n", i, buffer[i]);
    }
    return ;
}

int main(void)
{
    // test_wavetype();
    // test_sweep();
    // test_invrev();
    // test_opedec();
    // test_2incs();
    //test_indexing();
    // test_();
    test_buffer();

    return 0;
}
