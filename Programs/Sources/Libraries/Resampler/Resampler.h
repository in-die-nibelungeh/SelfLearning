#pragma once

#include "types.h"
#include "debug.h"
#include "Vector.h"

namespace masp {

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

    Resampler(int targetFs, int baseFs, double nFpass, double nFstop)
        : m_TargetFs(targetFs),
        m_BaseFs(baseFs),
        m_PassBandFrequency(nFpass),
        m_StopBandFrequency(nFstop)
        //m_PassBandRipple(0.01),
        //m_StopBandDecay(40)
    {
        ASSERT(targetFs > 0);
        ASSERT(baseFs > 0);
        ASSERT(0.0 < nFpass && nFpass < 1.0);
        ASSERT(0.0 < nFstop && nFstop < 1.0);
        ASSERT(nFpass < nFstop);

        Initialize(targetFs, baseFs, nFpass, nFstop);
    }

    ~Resampler() {};
    status_t Initialize(int targetFs, int baseFs, double nFpass, double nFstop);
    status_t SetSamplingRates(int targetFs, int baseFs);
    status_t SetFilterParams(double nFpass, double nFstop);

    status_t GetCoefficients(mcon::Vector<double>& coefficients) const;

    status_t MakeFilterByWindowType(int windowType, double alpha = 2.0);
    status_t MakeFilterBySpec(double pbRipple, double sbDecay);

    status_t Convert(mcon::Vector<double>& output, const mcon::Vector<double>& input) const;

private:
    status_t UpdateCoefficients(int windowType, double alpha);
    int m_TargetFs;
    int m_BaseFs;
    int m_L;
    int m_M;
    double m_PassBandFrequency;
    double m_StopBandFrequency;
    //double m_PassBandRipple;
    //double m_StopBandDecay; // [dB]
    mcon::Vector<double> m_Coefficients;
};

} // namespace masp {
