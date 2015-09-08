#include <math.h>

#include "types.h"
#include "Buffer.h"

#define POW2(x) ((x)*(x))

namespace masp {
namespace window {

double g_Pi(M_PI);

/*
void GeneralizedHamming(Container::Vector<double>& w)
        0.5 =< a =< 1.0
        0.50: Hanning
        0.54: Hamming
        1.00:  Square
        w[i] = a - (1 - a) * cos( (i+o) * dt);

*/

double HammingFunction(int i, double dt, double arg)
{
    return 0.54 - 0.46 * cos( (2*i+1) * dt / 2.0);
}

double HanningFunction(int i, double dt, double arg)
{
    return 0.5 - 0.5 * cos( (2*i+1) * dt / 2.0);
}

double GeneralizedHammingFunction(int i, double dt, double arg)
{
    double& a = arg;
    return a - (1.0 - a) * cos( (2*i+1) * dt / 2.0);
}

double BlackmanFunction(int i, double dt, double arg)
{
    return 0.42 - 0.5 * cos( (2*i+1) * dt / 2.0) + 0.08 * cos( (2*i+1) * dt);
}

double BlackmanHarrisFunction(int i, double dt, double arg)
{
    return 0.35875 - 0.48829 * cos( (2*i+1) * dt / 2.0) + 0.14128 * cos( (2*i+1) * dt) - 0.01168 * cos((2*i+1) * dt * 1.5);
}

double I0(double x)
{
    return 1.0;
}

double Kaiser(int i, double dt, double arg)
{
    double& a = arg;
    return I0 ( g_Pi * a * sqrt(1 - POW2((2*i+1) * dt / g_Pi / 2.0 - 1) ) ) / I0 (g_Pi * a);
}

double Nuttall(int i, double dt, double arg)
{
    return 0.355768 - 0.487396 * cos( (2*i+1) * dt / 2.0) + 0.144232 * cos( (2*i+1) * dt) - 0.012604 * cos((2*i+1) * dt * 1.5);
}

void GeneralizedHamming(double a, double w[], size_t N)
{
    double dt = 2 * g_Pi / N;
    for (int i = 0; i < N; ++i)
    {
        w[i] = a - (1.0 - a) * cos( (2*i+1) * dt / 2.0);
    }
}

void GeneralizedHamming(double a, Container::Vector<double>& w)
{
    const size_t N = w.GetNumOfData();
    double dt = 2 * g_Pi / N;
    for (int i = 0; i < N; ++i)
    {
        w[i] = a - (1.0 - a) * cos( (2*i+1) * dt / 2.0);
    }
}

void Hanning(double w[], size_t N)         { GeneralizedHamming(0.50, w, N); }
void Hanning(Container::Vector<double>& w) { GeneralizedHamming(0.50, w); }
void Hamming(double w[], size_t N)         { GeneralizedHamming(0.54, w, N); }
void Hamming(Container::Vector<double>& w) { GeneralizedHamming(0.54, w); }

/*
void Hanning(double w[], size_t N)
{
    double dt = 2 * g_Pi / N;
    for (int i = 0; i < N; ++i)
    {
        w[i] = 0.5 - 0.5 * cos( (2*i+1) * dt / 2.0);
    }
    return ;
}

void Hanning(Container::Vector<double>& w)
{
    const size_t N = w.GetNumOfData();
    double dt = 2 * g_Pi / N;
    for (int i = 0; i < N; ++i)
    {
        w[i] = 0.5 - 0.5 * cos( (2*i+1) * dt / 2.0);
    }
}
*/

} // namespace window {
} // namespace masp {
