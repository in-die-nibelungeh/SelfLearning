
//#ifdef __cplusplus
//#include <math>
//#else
#include <math.h>
//#endif /* __cplusplus */

#include <stdio.h> // for ASSERT

#include "WaveGen.h"


// TBD
#ifndef ASSERT
#define ASSERT(c) \
    if (!(c)) \
    {         \
        printf("ASSERT at %d in %s: " #c "\n", __LINE__, __FILE__); \
        *((int*)0) = 0; \
    }
#endif // #ifndef ASSERT

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif // #ifndef M_PI

#ifndef NULL
#define NULL ((void*)0)
#endif // NULL

#define DEFAULT_BASE_FREQUENCY 440
#define DEFAULT_SAMPLING_RATE  48000

const f64 WaveGen::g_Pi(M_PI);

WaveGen::WaveGen()
  : m_Type(WT_SINE), 
    m_Generator(&WaveGen::Sine),
	m_Duty(0.5), 
	m_FrequencyBase(DEFAULT_BASE_FREQUENCY), 
	m_FrequencyCurrent(DEFAULT_BASE_FREQUENCY),
	m_SweepEnable(false),
	m_SweepFactor(1.0f),
	m_PhaseCurrent(0),
	m_ValueCurrent(0),
	m_SamplingRate(DEFAULT_SAMPLING_RATE),
	m_Tick(1.0f/DEFAULT_SAMPLING_RATE)
{
	m_ValueCurrent = (this->*m_Generator)(m_PhaseCurrent);
}

WaveGen::~WaveGen()
{
}

struct WaveGen::Variable WaveGen::GetVariable(void) const
{
	struct Variable var = {
		m_PhaseCurrent, 
		m_ValueCurrent,
		m_FrequencyCurrent
	};
	
	return var;
}

bool WaveGen::SetWaveType(WaveGen::WaveType type, f64 duty)
{
    bool ret = true;
    ASSERT(
    	type == WT_SINE     || 
    	type == WT_SAWTOOTH || 
    	type == WT_TRIANGLE || 
    	type == WT_SQUARE   || 
    	type == WT_WATERSURFACE || 
    	type == WT_NOISE_WHITE  || 
    	type == WT_NOISE_PINK 
    );
    
    m_Type = type;
	m_Duty = duty;
	
	switch(type)
	{
	case WT_SINE        : m_Generator = &WaveGen::Sine         ; break;
	case WT_SAWTOOTH    : m_Generator = &WaveGen::Sawtooth     ; break;
	case WT_TRIANGLE    : m_Generator = &WaveGen::Triangle     ; break;
	case WT_SQUARE      : m_Generator = &WaveGen::Square       ; break;
	case WT_WATERSURFACE: m_Generator = &WaveGen::WaterSurface ; break;
	case WT_NOISE_WHITE : m_Generator = &WaveGen::NoiseWhite   ; break;
	case WT_NOISE_PINK  : m_Generator = &WaveGen::NoisePink    ; break;
	}
	
	// Updating the current value so that it can return the right value when called GetValue() right after called SetWaveType().
	m_ValueCurrent = Generate(m_PhaseCurrent);

    return true;
}

bool WaveGen::SetWaveParam(f64 base, WaveType type, f64 duty)
{
	SetWaveFrequency(base);
	return SetWaveType(type, duty);
}

void WaveGen::SetWaveFrequency(f64 base)
{
	ASSERT(base > 0.0);
	
	// Setting the current frequency, as well as the base frequency.
	m_FrequencyBase = base;
	m_FrequencyCurrent = base;
}

bool WaveGen::SetPhase(f64 p)
{
	if (p < 0.0f || p >= 1.0)
	{
		return false;
	}
	// Updating the current phase and value.
	m_PhaseCurrent = p;
	m_ValueCurrent = Generate(m_PhaseCurrent);

	return true;
}

bool WaveGen::EnableSweep(void)
{
	if (m_SweepFactor == 1.0f)
	{
    	m_SweepEnable = false;
	}
	else
	{
	    m_SweepEnable = true;
    }
    
    return m_SweepEnable;
}

void WaveGen::DisableSweep(void)
{
    m_SweepEnable = false;
}

bool WaveGen::SetSweepParam(f64 targetFrequency, f64 duration, bool flag)
{
	ASSERT(duration > 0.0f);
	ASSERT(targetFrequency > 0.0f);
	
	if (m_FrequencyBase == targetFrequency)
	{
		m_SweepFactor = 1.0f;
		return false;
	}
	
	m_SweepFactor = pow(targetFrequency/m_FrequencyBase, 
		1.0f/ (m_SamplingRate * duration) );
	
	m_SweepEnable = flag;
	
	return true;
}

void WaveGen::Reset(void)
{
	m_PhaseCurrent = 0.0f;
	m_FrequencyCurrent = m_FrequencyBase;
	m_SweepEnable = false;
	m_SweepFactor = 1.0f;
	m_ValueCurrent = Generate(m_PhaseCurrent);
}

void WaveGen::SetSamplingRate(s32 fs)
{
	ASSERT(fs > 0);
	m_SamplingRate = fs;
	m_Tick = 1.0f / fs;
}

f64 WaveGen::Sine(f64 nt)
{
	return sin(2 * g_Pi * nt);
}

f64 WaveGen::Sawtooth(f64 nt)
{
	//return nt;
	return (nt >= 0.5f) ? 2.0f * nt - 2.0f : 2.0f * nt;
}

f64 WaveGen::Triangle(f64 nt)
{
	f64 v = 4.0 * nt;
	if ( nt >= 0.75)
	{
		v = 4.0 * nt - 4.0;
	}
	else if ( nt >= 0.25)
	{
		v = -4.0 * nt + 2.0;
	}

	return v;
}

f64 WaveGen::Square(f64 nt)
{
	ASSERT(0.0 < m_Duty && m_Duty < 1.0);
	
	return (nt >= m_Duty) ? -1.0f : 1.0f;
}

f64 WaveGen::WaterSurface(f64 t)
{
	return 0.0;
}

f64 WaveGen::NoiseWhite(f64 t)
{
	return 0.0;
}

f64 WaveGen::NoisePink(f64 t)
{
	return 0.0;
}

f64 WaveGen::Generate(f64 p)
{
	return (this->*m_Generator)(p);
}

WaveGen  WaveGen::operator++(int)
{
	WaveGen old = *this;
	*this += 1;
	return old;
}

WaveGen& WaveGen::operator++(void)
{
	*this += 1;
	return *this;
}

WaveGen  WaveGen::operator+(int c) const
{
	f64 tick = c * m_Tick;
	f64 phase = m_PhaseCurrent;
	f64 freq = m_FrequencyCurrent;
	f64 factor = m_SweepEnable ? pow(m_SweepFactor, c) : 1.0f;
	
	Update(tick, phase, freq, factor);
	
	WaveGen ret = *this;
	ret.m_FrequencyCurrent = freq;
	ret.SetPhase(phase); // m_ValueCurrent is updated in SetPhase
	
	return ret;
}

WaveGen& WaveGen::operator+=(int c)
{
	f64 tick = c * m_Tick;
	f64 factor = m_SweepEnable ? pow(m_SweepFactor, c) : 1.0f;
	
	Update(tick, m_PhaseCurrent, m_FrequencyCurrent, factor);
	
	m_ValueCurrent = Generate(m_PhaseCurrent);
	
	return *this;
}

WaveGen  WaveGen::operator--(int)
{
	WaveGen old = *this;
	*this -= 1;
	return old;
}

WaveGen& WaveGen::operator--(void)
{
	*this -= 1;
	return *this;
}

WaveGen  WaveGen::operator-(int c) const
{
	return *this + (-c);
}

WaveGen& WaveGen::operator-=(int c)
{
	*this += (-c);
	return *this;
}

WaveGen  WaveGen::operator[](int c)
{
	return *this + c;
}

void WaveGen::Update(const f64& tick, f64& phase, f64& freq, const f64& factor)
{
	phase += (tick * freq);
	phase -= static_cast<int>(phase);
	freq  *= factor;
}

f64 WaveGen::GetValue(void) const
{
	return m_ValueCurrent;
}

s32 WaveGen::GetSamplingRate(void) const
{
	return m_SamplingRate;
}
