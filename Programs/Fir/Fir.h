
#pragma once

#include <sys/types.h>

#include "Vector.h"

namespace masp {
namespace fir {

enum FilterTypeId
{
    FTI_LPF = 1,
    FTI_HPF,
    FTI_BPF,
    FTI_BEF
};

enum FilterBaseId
{
    FBI_SINC = 1,
    FBI_LANCZOS
};

//void GetCoefficients(double coef[], size_t N, double fe, int filterType, int functionId);
//void HpfSinc(double coef[], size_t N, double fe);
void FilterSinc(double coef[], size_t N, double fe);
void FilterLanczos(double coef[], size_t N, double fe, double n);

void GetCoefficientsLpfSinc(mcon::Vector<double>& coef, double fe);
void GetCoefficientsHpfSinc(mcon::Vector<double>& coef, double fe);
void GetCoefficientsBpfSinc(mcon::Vector<double>& coef, double fe1, double fe2);
void GetCoefficientsBefSinc(mcon::Vector<double>& coef, double fe1, double fe2);
void GetCoefficientsLpfLanczos(mcon::Vector<double>& coef, double fe, double n);
void GetCoefficientsHpfLanczos(mcon::Vector<double>& coef, double fe, double n);
void GetCoefficientsBpfLanczos(mcon::Vector<double>& coef, double fe1, double fe2, double n);
void GetCoefficientsBefLanczos(mcon::Vector<double>& coef, double fe1, double fe2, double n);

// This is for Hanning, not the others.
size_t GetNumOfTapps(double cutOff, int samplingRate);


} // namespace fir {
} // namespace masp {
