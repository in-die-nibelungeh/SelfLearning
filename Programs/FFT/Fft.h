#pragma once

#include "types.h"
#include "Matrix.h"

namespace Fft {

status_t Fft (double realPart[], double imaginaryPart[], double timeSeries[], int numData);
status_t Ifft(double timeSeries[], double realPart[], double imaginaryPart[], int numData);
status_t Ft  (double realPart[], double imaginaryPart[], const double timeSeries[], int numData);
status_t Ift (double timeSeries[], const double realPart[], const double imaginaryPart[], int numData);

status_t Ft  (mcon::Matrix<double>& complex, const mcon::Vector<double>& timeSeries);
status_t Ift (mcon::Vector<double>& timeSeries, const mcon::Matrix<double>& complex);

status_t ConvertToGainPhase(mcon::Matrix<double>& gainPhase, const mcon::Matrix<double>& complex);

} // namespace Fft
