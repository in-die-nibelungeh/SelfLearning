#pragma once

#include "types.h"

namespace Fft
{
	status_t Fft (f64 real[], f64 imag[], f64 td[], s32 n);
	status_t Ifft(f64 td[], f64 real[], f64 imag[], s32 n);
	status_t Ft  (f64 real[], f64 imag[], f64 td[], s32 n);
	status_t Ift (f64 td[], f64 real[], f64 imag[], s32 n);

}; // namespace Fft
