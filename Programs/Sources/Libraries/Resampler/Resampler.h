#pragma once

#include "types.h"
#include "Window.h"

class Resampler
{
public:
    enum WindowType
    {
        HANNING,
        HAMMING,
        BLACKMAN,
        KAISER,
        NUM_WINDOW_TYPE
    };

    Resampler() {};
    ~Resampler() {};
    status_t Initialize(int targetFs, int baseFs, int windowType);
    status_t SetSamplingRates(int targetFs, int baseFs);
    status_t SetWindowType(int windowType, double alpha = 2.0);
    status_t Convert(mcon::Vector<double>& output) const;
    double Get(int i) const;

private:
    status_t UpdateCoefficients(void);
    int m_TargetFs;
    int m_BaseFs;
    double m_Ratio;
    int m_L;
    int m_M;
    int m_Length;
    mcon::Vector<double> m_Data;
    mcon::Vector<double> m_Coefficients;
    int m_WindowType;
    double m_WindowArgument;
};

//Resampler::

