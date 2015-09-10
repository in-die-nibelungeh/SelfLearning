
#pragma once

#include <sys/types.h>

#include "Buffer.h"

namespace masp {
namespace iir {

class Biquad
{
public:
    enum FilterTypeId
    {
        LPF = 1,
        HPF,
        BPF,
        BEF
    };

    struct Coefficients
    {
        double a[2];
        double b[3];
    };

    struct Context
    {
        double x[2];
        double y[2];
    };

    Biquad();
    Biquad(double Q, double fc, int type, int samplingRate);
    ~Biquad() {};

    void Initialize(void);
    void CalculateCoefficients(double Q, double fc, int type, int samplingRate);

    const struct Coefficients& GetCoefficients(void) { return m_Coefficients; }
    const struct Context& GetContext(void) { return m_Context; }

    double ApplyFilter(double x0);

    static double ConvertD2A(double fd, int fs);

private:

    struct Coefficients m_Coefficients;
    struct Context m_Context;
};

} // namespace iir {
} // namespace masp {
