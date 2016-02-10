/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2015-2016 Ryosuke Kanata
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include <stdio.h>
#include <math.h>
#include "mtbx.h"
#include "mfio.h"

#define NUM_SAMPLES 120
#define NUM_TYPES 4

using namespace mtbx;

int test_wavetype(void)
{
    int i, j;
    mcon::Matrixd buffer(NUM_TYPES, NUM_SAMPLES);

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
    mfio::Csv::Write("test_wavetype.csv", buffer);
    return 0;
}

#undef NUM_SAMPLES
#undef NUM_TYPES
#define NUM_TYPES 3
#define NUM_SAMPLES 480

#define FREQ_BASE 440

int test_sweep(void)
{
    int i, j;
    mcon::Matrixd buffer(NUM_TYPES, NUM_SAMPLES);

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
    mfio::Csv::Write("test_sweep.csv", buffer);
    return 0;
}

// Inverse and Reverse options are removed.
#undef NUM_SAMPLES
#undef NUM_TYPES
#define NUM_TYPES 4
#define NUM_SAMPLES 480

#define FREQ_BASE 440

int test_invrev(void)
{
    int i, j;
    mcon::Matrixd buffer(NUM_TYPES, NUM_SAMPLES);

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

    {
        mfio::Csv csv("test_invrev.csv");
        csv.Write("Time,Normal,Decrement,Inverser,Dec&Inv\n");
        csv.Write(buffer);
        csv.Close();
    }
    return 0;
}

#undef NUM_SAMPLES
#undef NUM_TYPES
#define NUM_TYPES 2
#define NUM_SAMPLES 480

#define FREQ_BASE 440

int test_opedec(void)
{
    int i, j;
    mcon::Matrixd buffer(NUM_TYPES, NUM_SAMPLES);

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

    {
        mfio::Csv csv("test_opedec.csv");
        csv.Write("Time,Normal,Decrement\n");
        csv.Write(buffer);
        csv.Close();
    }
    return 0;
}

#undef NUM_SAMPLES
#undef NUM_TYPES
#define NUM_TYPES 2
#define NUM_SAMPLES 4800

#define FREQ_BASE 440

int test_2incs(void)
{
    int i, j;
    mcon::Matrixd buffer(NUM_TYPES, NUM_SAMPLES);

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

    {
        mfio::Csv csv("test_2incs.csv");
        csv.Write("\"Time\",=\"++wg\",\"wg++\"\n");
        csv.Write(buffer);
        csv.Close();
    }
    printf("Result: %s\n", result ? "OK" : "NG");

    return 0;
}

#undef NUM_SAMPLES
#undef NUM_TYPES
#define NUM_TYPES 3
#define NUM_SAMPLES 480

#define FREQ_BASE 440

int test_indexing(void)
{
    int i, j;
    mcon::Matrixd buffer(NUM_TYPES, NUM_SAMPLES);

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
    {
        mfio::Csv csv("test_indexing.csv");
        csv.Write("\"Time\",=\"++wg\",=\"(wg + i)\",\"wg += 1\"\n");
        csv.Write(buffer);
        csv.Close();
    }

    return 0;
}

int test_(void)
{
    WaveGen wg;
    wg = wg + 3;
    wg = wg - 4;
    wg += 2;
    return 0;
}

int test_buffer(void)
{
    float duration = 0.01;
    int samplingRate = 48000;
    double freq = 440;
    WaveGen wg(samplingRate, freq, WaveGen::WT_SINE);
    const size_t numData = static_cast<int>(duration * samplingRate);
    wg.Reset();

    mcon::Vector<double> buffer(numData);

    wg.GenerateWaveform(buffer);
    mfio::Csv::Write("result_buffer.csv", buffer);
    return 0;
}

int main(void)
{
    test_wavetype();
    test_sweep();
    test_invrev();
    test_opedec();
    test_2incs();
    test_indexing();
    test_buffer();

    return 0;
}
