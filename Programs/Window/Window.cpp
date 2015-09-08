#include <math.h>

#include "Buffer.h"
#include "Window.h"

#define POW2(x) ((x)*(x))


namespace masp {
namespace window {

enum WindowFunctionId
{
    WFI_HANNING = 1,
    WFI_HAMMING,
    WFI_GENERALIZED_HAMMING,
    WFI_BLACKMAN,
    WFI_BLACKMAN_HARRIS,
    WFI_NUTTALL,
    WFI_KAISER
};

static const double g_Pi(M_PI);

typedef double (*WindowFunctionType)(int i, double dt, double arg) ;

double GeneralizedHammingFunction(int i, double dt, double arg)
{
    double& a = arg;
    return a - (1.0 - a) * cos( (2*i+1) * dt / 2.0);
}

double HammingFunction(int i, double dt, double arg)
{
    //return 0.54 - 0.46 * cos( (2*i+1) * dt / 2.0);
    return GeneralizedHammingFunction(i, dt, 0.54);
}

double HanningFunction(int i, double dt, double arg)
{
    //return 0.5 - 0.5 * cos( (2*i+1) * dt / 2.0);
    return GeneralizedHammingFunction(i, dt, 0.50);
}

double BlackmanFunction(int i, double dt, double arg)
{
    return 0.42 - 0.5 * cos( (2*i+1) * dt / 2.0) + 0.08 * cos( (2*i+1) * dt);
}

double BlackmanHarrisFunction(int i, double dt, double arg)
{
    return 0.35875 - 0.48829 * cos( (2*i+1) * dt / 2.0) + 0.14128 * cos( (2*i+1) * dt) - 0.01168 * cos((2*i+1) * dt * 1.5);
}

double Factorial(int n)
{
    double ans = 1.0;
    for (int i = 1; i <= n; ++i) { ans *= i; }
    return ans;
}

double I0(double x)
{
    double ans = 0.0;
    for (int k = 0; k < 1000; ++k)
    {
        ans += pow(x/2, 2 * k) / POW2(Factorial(k));
    }
    return ans;
}

double KaiserFunction(int i, double dt, double a)
{
    return I0( g_Pi * a * sqrt(1 - POW2((2*i+1) * dt / g_Pi / 2.0 - 1) ) ) / I0 (g_Pi * a);
}

double NuttallFunction(int i, double dt, double arg)
{
    return 0.355768 - 0.487396 * cos( (2*i+1) * dt / 2.0) + 0.144232 * cos( (2*i+1) * dt) - 0.012604 * cos((2*i+1) * dt * 1.5);
}

WindowFunctionType GetWindowFunction(int type)
{
    WindowFunctionType function = reinterpret_cast<WindowFunctionType>(NULL);
    switch(type)
    {
    case WFI_HANNING:             function = HanningFunction; break;
    case WFI_HAMMING:             function = HammingFunction; break;
    case WFI_GENERALIZED_HAMMING: function = GeneralizedHammingFunction; break;
    case WFI_BLACKMAN:            function = BlackmanFunction; break;
    case WFI_BLACKMAN_HARRIS:     function = BlackmanHarrisFunction; break;
    case WFI_NUTTALL:             function = NuttallFunction; break;
    case WFI_KAISER:              function = KaiserFunction; break;
    }
    return function;
}

void GenerateWindow(double w[], size_t N, int type, double arg)
{
    WindowFunctionType function = GetWindowFunction(type);
    ASSERT(function != NULL);
    double dt = 2 * g_Pi / N;
    for (int i = 0; i < N; ++i)
    {
        w[i] = function(i, dt, arg);
    }
}

void GenerateWindow(Container::Vector<double>& w, int type, double arg)
{
    const size_t N = w.GetNumOfData();
    WindowFunctionType function = GetWindowFunction(type);
    ASSERT(function != NULL);

    double dt = 2 * g_Pi / N;
    for (int i = 0; i < N; ++i)
    {
        w[i] = function(i, dt, arg);
    }
}

void Hanning(double w[], size_t N)          { GenerateWindow(w, N, WFI_HANNING , 0.0); }
void Hanning(Container::Vector<double>& w)  { GenerateWindow(w,    WFI_HANNING , 0.0); }
void Hamming(double w[], size_t N)          { GenerateWindow(w, N, WFI_HAMMING , 0.0); }
void Hamming(Container::Vector<double>& w)  { GenerateWindow(w,    WFI_HAMMING , 0.0); }
void GeneralizedHamming(double w[], size_t N, double a)          { GenerateWindow(w, N, WFI_GENERALIZED_HAMMING , a); }
void GeneralizedHamming(Container::Vector<double>& w, double a)  { GenerateWindow(w,    WFI_GENERALIZED_HAMMING , a); }
void Blackman(double w[], size_t N)         { GenerateWindow(w, N, WFI_BLACKMAN, 0.0); }
void Blackman(Container::Vector<double>& w) { GenerateWindow(w,    WFI_BLACKMAN, 0.0); }
void BlackmanHarris(double w[], size_t N)         { GenerateWindow(w, N, WFI_BLACKMAN_HARRIS, 0.0); }
void BlackmanHarris(Container::Vector<double>& w) { GenerateWindow(w,    WFI_BLACKMAN_HARRIS, 0.0); }
void Nuttall(double w[], size_t N)          { GenerateWindow(w, N, WFI_NUTTALL, 0.0); }
void Nuttall(Container::Vector<double>& w)  { GenerateWindow(w,    WFI_NUTTALL, 0.0); }
void Kaiser(double w[], size_t N, double a)           { GenerateWindow(w, N, WFI_KAISER, a); }
void Kaiser(Container::Vector<double>& w, double a)   { GenerateWindow(w,    WFI_KAISER, a); }

} // namespace window {
} // namespace masp {
