#include <math.h>
#include "types.h"

namespace Asp {

void Sinc(f64 coef[], size_t N, f64 df)
{
    int offset = (N+1) & 1;
    for (int i = 0; i < N/2; ++i)
    {
        int idx = (i<<1) - N + offset;
        f64 x = idx * df / 2.0;
        f64 v = sin(x)/x;
        coef[i] = v;
        coef[N-i-1] = v;
    }
    if (0 == offset)
    {
        coef[N/2] = 1.0;
    }
    return ;
}


} // namespace Asp {
