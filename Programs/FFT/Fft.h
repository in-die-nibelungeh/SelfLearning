#pragma once

#include "types.h"
#include "Buffer.h"

namespace Fft {

status_t Fft (double realPart[], double imaginaryPart[], double timeSeries[], int numData);
status_t Ifft(double timeSeries[], double realPart[], double imaginaryPart[], int numData);
status_t Ft  (double realPart[], double imaginaryPart[], const double timeSeries[], int numData);
status_t Ift (double timeSeries[], const double realPart[], const double imaginaryPart[], int numData);

status_t Ft  (Container::Matrix<double>& complex, const Container::Vector<double>& timeSeries);
status_t Ift (Container::Vector<double>& timeSeries, const Container::Matrix<double>& complex);

status_t ConvertToGainPhase(Container::Matrix<double>& gainPhase, const Container::Matrix<double>& complex);

} // namespace Fft
