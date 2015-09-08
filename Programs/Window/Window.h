#pragma once

#include "Buffer.h"

namespace masp {
namespace window {
    void Hanning(double w[], size_t N);
    void Hanning(Container::Vector<double>& w);
    void Hamming(double w[], size_t N);
    void Hamming(Container::Vector<double>& w);
    void GeneralizedHamming(double w[], size_t N, double a);
    void GeneralizedHamming(Container::Vector<double>& w, double a);
    void Blackman(double w[], size_t N);
    void Blackman(Container::Vector<double>& w);
    void BlackmanHarris(double w[], size_t N);
    void BlackmanHarris(Container::Vector<double>& w);
    void Nuttall(double w[], size_t N);
    void Nuttall(Container::Vector<double>& w);
    void Kaiser(double w[], size_t N, double a);
    void Kaiser(Container::Vector<double>& w, double a);

} // namespace window {
} // namespace masp {
