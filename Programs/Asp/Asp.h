#pragma once

#include "types.h"

#include "Window.h"

class Asp
{
public:
    static const double g_Pi;
    static void Sinc(double coef[], size_t N, double df);
    static inline int GetNumTapps(double cutOff, double samplingRate)
    {
        int tapps = static_cast<int>((3.1f * samplingRate / cutOff) + 0.5 - 1);
        if ( (tapps & 1) == 0)
        {
            ++tapps;
        }
        return tapps;
    }
};
