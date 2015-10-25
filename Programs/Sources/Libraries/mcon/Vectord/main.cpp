#include <stdio.h>
#include <math.h>

#include "Vectord.h"

#include "test.cpp"
//#include "tune.cpp"

#if 0
#include "../Matrixd/Matrixd.h"

void DumpMatrix(mcon::Matrixd& m, const char* fmt = NULL)
{
    for (int i = 0; i < m.GetRowLength(); ++i)
    {
        printf("| ");
        for (int j = 0; j < m.GetColumnLength(); ++j)
        {
            printf("%g", static_cast<double>(m[i][j]));
            printf("\t");
        }
        printf(" |\n");
    }
}

static void test_transpose(void)
{
    int numArray = 3, numData= 5;
    mcon::Matrixd mat(numArray, numData);
    for (int i = 0; i < numArray; ++i)
    {
        printf("mat [%d, 0 .. %d] ", i, numData-1);
        for (int j = 0; j < numData; ++j)
        {
            mat[i][j] = (i+1)*10+(j+1);
            printf("%f ", mat[i][j]);
        }
        printf("\n");
    }

    mcon::Matrixd matt(1,1);
    matt = mat.Transpose();
    for (int i = 0; i < matt.GetRowLength(); ++i)
    {
        printf("mat [%d,0-%d] ", i, matt.GetColumnLength()-1);
        for (int j = 0; j < matt.GetColumnLength(); ++j)
        {
            printf("%f ", matt[i][j]);
        }
        printf("\n");
    }
}
#endif

int main(void)
{
    #if 0
    test_transpose();
    test_matrix_multiply();
    test_matrix_determinant();
    benchmark_vector_api();
#endif
    test_vector_api();

    return 0;
}
