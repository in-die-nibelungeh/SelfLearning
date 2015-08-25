#pragma once

#include "types.h"

namespace Fft
{
	bool Fft(f64 real[], f64 imag[], f64 td[], s32 n);
	bool Ft (f64 real[], f64 imag[], f64 td[], s32 n);
	bool Ift(f64 td[], f64 real[], f64 imag[], s32 n);

}; // namespace Fft
