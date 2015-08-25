#include "types.h"
#include <math.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif // #ifndef M_PI

namespace Fft
{

static f64 g_Pi(M_PI);

bool Fft(f64 real[], f64 imag[], f64 td[], s32 n)
{
	return true;
}

bool Ft(f64 real[], f64 imag[], f64 td[], s32 n)
{
	for (int i = 0; i < n; ++i)
	{
		f64 df = (f64)i * g_Pi * 2/ n;
		real[i] = 0.0;
		imag[i] = 0.0;
		for (int j = 0; j < n; ++j)
		{
			real[i] += (td[j] * cos(df * j));
			imag[i] -= (td[j] * sin(df * j));
		}
		//real[i] /= n;
		//imag[i] /= n;
	}
	return true;
}

bool Ift(f64 td[], f64 real[], f64 imag[], s32 n)
{
	for (int i = 0; i < n; ++i)
	{
		f64 df = (f64)i * g_Pi * 2/ n;
		td[i] = 0.0;
		for (int j = 0; j < n; ++j)
		{
			td[i]  += (real[j] * cos(df * j) - imag[j] * sin(df * j));
		}
		td[i] /= n;
	}
	return true;
}

}; // namespace Fft
