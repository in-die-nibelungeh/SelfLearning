#pragma once

#include "Vector.h"

namespace masp {
namespace window {

void Square(double w[], size_t N);
void Square(mcon::Vector<double>& w);
void Hanning(double w[], size_t N);
void Hanning(mcon::Vector<double>& w);
void Hamming(double w[], size_t N);
void Hamming(mcon::Vector<double>& w);
void GeneralizedHamming(double w[], size_t N, double a);
void GeneralizedHamming(mcon::Vector<double>& w, double a);
void Blackman(double w[], size_t N);
void Blackman(mcon::Vector<double>& w);
void BlackmanHarris(double w[], size_t N);
void BlackmanHarris(mcon::Vector<double>& w);
void Nuttall(double w[], size_t N);
void Nuttall(mcon::Vector<double>& w);
void Kaiser(double w[], size_t N, double a);
void Kaiser(mcon::Vector<double>& w, double a);
void Flattop(double w[], size_t N);
void Flattop(mcon::Vector<double>& w);

} // namespace window {
} // namespace masp {
