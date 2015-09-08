
#pragma once

#include <sys/types.h>

#include "Buffer.h"

namespace masp {
namespace fir {

void FilterSinc(double coef[], size_t N, double fe);
void FilterLanczos(double coef[], size_t N, double fe, double n);

//void Sinc   (double coef[], size_t N, double dt);
//void Lanczos(double coef[], size_t N, double dt);

//void Sinc   (Container::Vector<double>& coef, double dt);
//void Lanczos(Container::Vector<double>& coef, double dt);

} // namespace fir {
} // namespace masp {
