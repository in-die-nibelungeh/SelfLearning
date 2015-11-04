/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2015 Ryosuke Kanata
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

#pragma once

#include "types.h"
#include "mcon.h"

namespace mtbx {

class WaveGen
{
public:

    enum WaveType
    {
        WT_SINE,
        WT_SAWTOOTH,
        WT_TRIANGLE,
        WT_SQUARE,
        WT_WATERSURFACE,
        WT_NOISE_WHITE,
        WT_NOISE_PINK,
        WT_RECTANGULAR = WT_SQUARE,
    };

    struct Variable {
        double phase;
        double value;
        double frequency;
    };

    WaveGen(int samplingRate, double frequency, WaveType type, double duty = 0.5f);
    WaveGen();
    ~WaveGen();

    void Reset(void);

    struct Variable GetVariable(void) const;
    double  GetValue(void) const ;
    bool SetPhase(double phase);
    int  GetSamplingRate(void) const ;
    void SetSamplingRate(int samplingRate);
    void SetWaveFrequency(double frequency);
    bool SetWaveType(WaveType type, double duty=0.5f);
    bool SetWaveParam(double frequency, WaveType type, double duty=0.5f);
    bool SetSweepParam(double targetFrequency, double duration, bool enable=false);
    bool EnableSweep(void);
    void DisableSweep(void);

    WaveGen& operator++(void);
    WaveGen  operator++(int);
    WaveGen  operator+(int) const;
    WaveGen& operator+=(int);
    WaveGen& operator--(void);
    WaveGen  operator--(int);
    WaveGen  operator-(int) const;
    WaveGen& operator-=(int);
    WaveGen  operator[](int);

    // Aliases
    void SetBaseFrequency(double frequency) { SetWaveFrequency(frequency); }

    void GenerateWaveform(double buffer[], size_t n, double amplitude = 1.0f);
    void GenerateWaveform(mcon::Vector<double>& buffer, double amplitude = 1.0f);

private:
    /*
     * Period Generator
    class PeriGen
    {
        // Parameters for quantizing
        int  m_SamplingRate;
        double  m_Tick;
        double  m_FrequencyBase;

        // Working variables.
        double  m_FrequencyCurrent;
        double  m_PhaseCurrent;
    };
     */
    // GenerateOners
    double  Sine(double);
    double  Sawtooth(double);
    double  Triangle(double);
    double  Square(double);
    double  WaterSurface(double);
    double  NoiseWhite(double);
    double  NoisePink(double);
    double  GenerateOne(double);
    double  (WaveGen::*m_Generator)(double);

    // Updates the internal states.
    static void Update(const double& tick, double& phase, double& freq, const double& factor);

    // Parameters for math
    static const double g_Pi;

    // Parameters for quantizing
    int  m_SamplingRate;
    double  m_Tick;

    // Parameters for generating waveform
    WaveType m_Type;
    double  m_Duty;
    double  m_FrequencyBase;

    bool m_SweepEnable;
    double  m_SweepFactor;

    // Working variables.
    double  m_FrequencyCurrent;
    double  m_PhaseCurrent;
    double  m_ValueCurrent;
};

} // namespace mtbx {
