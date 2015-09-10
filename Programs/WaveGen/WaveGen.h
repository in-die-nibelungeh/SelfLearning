
#ifndef _WAVE_GEN_H_
#define _WAVE_GEN_H_

#include "types.h"
#include "Buffer.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#ifdef __cplusplus
}
#endif /* __cplusplus */

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
    bool SetBaseFrequency(double frequency) { SetWaveFrequency(frequency); }

    void GenerateWaveform(double buffer[], size_t n, double amplitude = 1.0f);
    void GenerateWaveform(Container::Vector<double>& buffer, double amplitude = 1.0f);

private:

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

#endif // #ifndef _WAVE_GEN_H_
