#include <stdlib.h>
#include <stdio.h>

#include "types.h"
#include "debug.h"

#include "Csv.h"

static void test_csv(void)
{
    const int length = 16;
    double darray[length];
    mcon::Vector<double> vector(length);
    mcon::Matrix<double> matrix(3, length);

    for ( int i = 0; i < length; ++i)
    {
        darray[i] = i + 1.0;
        vector[i] = i + 1.0;
        for ( int k = 0; k < matrix.GetRowLength(); ++k)
        {
            matrix[k][i] = 10*(k+1) + i + 1.0;
        }
    }
    {
        mfio::Csv::Write("static_darray.csv", darray, length);
        mfio::Csv::Write("static_vector.csv", vector);
        mfio::Csv::Write("static_matrix.csv", matrix);
    }
    {
        mfio::Csv csv("darray.csv");
        csv.Write("i,data\n");
        csv.Write(darray, length);
        csv.Write(darray, length);
        csv.Close();

        csv.Open("vector.csv");
        csv.Write("i,data\n");
        csv.Write(vector);
        csv.Crlf();
        csv.Write(vector);
        csv.Close();

        csv.Open("matrix.csv");
        csv.Write("i,data,data,data\n");
        csv.Write(matrix);
        csv.Crlf();
        csv.Write(matrix);
        csv.Close();
    }
}

int main(void)
{
    test_csv();
    return 0;
}
