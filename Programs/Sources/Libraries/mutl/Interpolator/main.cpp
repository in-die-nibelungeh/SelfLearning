#include "mcon.h"
#include "mfio.h"
#include "Linear.h"

void test_linear()
{
    const int N = 100;
    const int newN = 150;
    mcon::Vector<double> data(N);
    for ( int k = 0; k < data.GetLength(); ++k )
    {
        data[k] = k;
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

int main(void)
{
    test_linear();
    return 0;
}
