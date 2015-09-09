#include <stdio.h>
#include <math.h>
#include <sys/types.h>

#include "Buffer.h"
#include "FileIo.h"
#include "Iir.h"
#include "Window.h"

static const double g_Pi(M_PI);

#define POW2(x) ((x)*(x))

class Biquad
{
public:
    enum FilterTypeId
    {
        LPF = 1,
        HPF,
        BPF,
        BEF
    };

    Biquad() : m_Type(LPF)
    {
        m_Y[0] = 0; m_Y[1] = 0;
        m_X[0] = 0; m_X[1] = 0;
        m_B[0] = 1; m_B[1] = 0; m_B[2] = 0;
        m_A[0] = 0; m_A[1] = 0;
    };
    ~Biquad() {};

    void CalculateCoefficients(double Q, double fc1, double fc2, int type)
    {
        switch(type)
        {
        case LPF:
            {
                const double a = POW2(2.0 * g_Pi * fc1);
                m_B[0] = a / (1 + 2 * g_Pi * fc1 / Q + a);
                m_B[1] = 2.0 * a / (1 + 2 * g_Pi * fc1 / Q + a);
                m_B[2] = a / (1 + 2 * g_Pi * fc1 / Q + a);
                m_A[0] = (2.0 * a - 2.0) / (1 + 2 * g_Pi * fc1 / Q + a);
                m_A[1] = (1 - 2 * g_Pi * fc1 / Q + a) / (1 + 2 * g_Pi * fc1 / Q + a);
                printf("b0=%f,b1=%f,b2=%f,a0=%f,a=%f\n", m_B[0], m_B[1], m_B[2], m_A[0], m_A[1]);
            }
            break;
        }
    }

    double ApplyFilter(double x0)
    {
        double y = m_B[0] * x0
                   + m_B[1] * m_X[0]
                   + m_B[2] * m_X[1]
                   - m_A[0] * m_Y[0]
                   - m_A[1] * m_Y[1];
        m_Y[1] = m_Y[0];
        m_Y[0] = y;
        m_X[2] = m_X[1];
        m_X[1] = m_X[0];
        m_X[0] = x0;
        return y;
    }

private:
    int    m_Type;
    double m_X[2];
    double m_Y[2];
    double m_B[3];
    double m_A[2];
};

double CalculateAf(double fd, double fs)
{
    return tan(g_Pi * fd / fs) / (2.0 * g_Pi);
}

double b0(double fc, double Q)
{
    const double a = POW2(2.0 * g_Pi * fc);
    return  a / (1 + 2 * g_Pi * fc / Q + a);
}

double b1(double fc, double Q)
{
    const double a = POW2(2.0 * g_Pi * fc);
    return  2.0 * a / (1 + 2 * g_Pi * fc / Q + a);
}

double b2(double fc, double Q)
{
    const double a = POW2(2.0 * g_Pi * fc);
    return  a / (1 + 2 * g_Pi * fc / Q + a);
}

double a1(double fc, double Q)
{
    const double a = POW2(2.0 * g_Pi * fc);
    return  (2.0 * a - 2.0) / (1 + 2 * g_Pi * fc / Q + a);
}

double a2(double fc, double Q)
{
    const double a = POW2(2.0 * g_Pi * fc);
    return  (1 - 2 * g_Pi * fc / Q + a) / (1 + 2 * g_Pi * fc / Q + a);
}

static void test_filtering(void)
{
    Container::Vector<int16_t> sweep(1);
    FileIo wave;
    wave.Read("sweep_440-8800.wav", sweep);
    struct FileIo::MetaData metaData = wave.GetMetaData();
    double Qf = 1 / sqrt(2);
    double fs = metaData.samplingRate;
    double fd = 1000;
    Biquad lpf;
    double fc = CalculateAf(fd, fs);
    lpf.CalculateCoefficients(Qf, fc, 0.0, Biquad::LPF);

    Container::Vector<int16_t> sweep_filtered(sweep.GetNumOfData());

    printf("Fc=%f\n", fc);
    printf("b0=%f,b1=%f,b2=%f,a1=%f,a2=%f\n",
        b0(fc, Qf), b1(fc, Qf), b2(fc, Qf), a1(fc, Qf), a2(fc, Qf));

    for (int i = 0; i < sweep_filtered.GetNumOfData(); ++i)
    {
        double v = lpf.ApplyFilter(static_cast<double>(sweep[i]));
        if (i < 30)
            printf("%d,%d,%f\n", i, sweep[i], v);
        sweep_filtered[i] = static_cast<int16_t>(v);
    }
    wave.Write("sweep_440-8800_filtered.wav", sweep_filtered);

}

int main(void)
{
    test_filtering();
    return 0;
}
