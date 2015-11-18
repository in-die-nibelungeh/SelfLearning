#include <math.h>

#include "mcon.h"
#include "mfio.h"
#include "Linear.h"
#include "Spline.h"

#define VALUE(k) (((k) - 10) * ((k) - 40) * ((k) - 80) * 1.0e-4)
#define POW2(v) ((v)*(v))

double GetLse(mcon::Vector<double>& data, mcon::Vector<double>& ref)
{
    const int smaller = data.GetLength() > ref.GetLength() ? ref.GetLength() : data.GetLength();
    double err = 0.0;
    for ( int k = 0; k < smaller; ++k )
    {
        err += POW2(data[k] - ref[k]);
    }
    return sqrt(err);
}

void test_interp()
{
    const int ratio = 10;
    const int N = 101;
    mcon::Matrix<double> matrix(5, N);
    matrix = 0;

    mcon::Vector<double>& data   = matrix[0];
    mcon::Vector<double>& coarse_index = matrix[3];
    //mcon::Vector<double>& coarse = matrix[2];
    mcon::Vector<double>& linear = matrix[1];
    mcon::Vector<double>& spline = matrix[2];

    const int every = N / ratio;
    mcon::Vector<double> coarse(every + (N % ratio ? 1 : 0));
    for ( int k = 0; k < data.GetLength(); ++k )
    {
        data[k] = VALUE(k);
        if ( (k % every) == 0 )
        {
            coarse_index[k/every] = k;
            coarse[k/every] = VALUE(k);
        }
    }
    mutl::interp::Linear::Interpolate(linear, coarse, N);
    mutl::interp::Spline::Interpolate(spline, coarse, N);

    matrix[4].Copy(coarse);
    mfio::Csv::Write("interp.csv", matrix);
    printf("error (linear) = %g\n", GetLse(linear, data));
    printf("error (spline) = %g\n", GetLse(spline, data));

}

int main(void)
{
    // Display messages real-time
    setvbuf(stdout, NULL, _IONBF, 0);

    test_interp();
    return 0;
}
