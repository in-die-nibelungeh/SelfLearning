#include <math.h>

#include "types.h"
#include "Asp.h"

namespace Window {

void Hanning(f64 w[], size_t N)
{
    f64 dt = 2 * Asp::g_Pi / N;
    f64 offset = 0.5 * ( N%2 );
    for (int i = 0; i < N; ++i)
    {
        w[i] = 0.5 - 0.5 * cos( (i+offset) * dt);
    }
    return ;
}

} // namespace Window {
