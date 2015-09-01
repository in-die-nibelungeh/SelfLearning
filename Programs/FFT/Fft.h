#pragma once

#include "types.h"
#include "Buffer.h"

namespace Fft
{
    status_t Fft (f64 real[], f64 imag[], f64 td[], s32 n);
    status_t Ifft(f64 td[], f64 real[], f64 imag[], s32 n);
    status_t Ft  (f64 real[], f64 imag[], const f64 td[], s32 n);
    status_t Ft  (Container::Matrix<f64>& fd, const Container::Vector<f64>& td);
    status_t Ift (f64 td[], const f64 real[], const f64 imag[], s32 n);
    status_t Ift (Container::Vector<f64>& td, const Container::Matrix<f64>& fd);

}; // namespace Fft
