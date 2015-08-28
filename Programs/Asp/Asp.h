#pragma once

#include "Window.h"
#include <math.h>

namespace Asp {
    static const f64 g_Pi(M_PI);
    void Sinc(f64 coef[], size_t N, f64 df);
} // namespace Asp {
