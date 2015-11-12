#include "mcon.h"
#include "mfio.h"
#include "Linear.h"
#include "Spline.h"

#define VALUE(k) (((k) - 20) * ((k) - 50) * ((k) - 70) * 1.0e-4)

void test_linear()
{
    const int N = 100;
    const int newN = 150;
    mcon::Vector<double> data(N);
    for ( int k = 0; k < data.GetLength(); ++k )
    {
        data[k] = VALUE(k);
    }
    mcon::Vector<double> output;
    mutl::interp::Linear::Interpolate(output, data, static_cast<int>(newN));

    {
        mcon::Matrix<double> saved(2, newN);

        memcpy(saved[0], data, sizeof(double) * data.GetLength());
        saved[1] = output;
        mfio::Csv::Write("linear.csv", saved);
    }
}

void test_spline()
{
    const int N = 100;
    const int newN = 150;
    mcon::Vector<double> data(N);
    for ( int k = 0; k < data.GetLength(); ++k )
    {
        data[k] = VALUE(k);
    }
    mcon::Vector<double> output;
    mutl::interp::Spline::Interpolate(output, data, static_cast<int>(newN));

    {
        mcon::Matrix<double> saved(2, newN);

        memcpy(saved[0], data, sizeof(double) * data.GetLength());
        saved[1] = output;
        mfio::Csv::Write("spline.csv", saved);
    }
}

int main(void)
{
    //test_linear();
    test_spline();
    return 0;
}
