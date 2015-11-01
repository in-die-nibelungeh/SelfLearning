#include <stdio.h>
#include <math.h>

#include "Vectord.h"
#include "Matrixd.h"

void DumpMatrix(mcon::Matrixd&m, const char* fmt = NULL)
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

extern void test_Matrixd(void);

static void test_matrix_determinant(void)
{
    int numArray = 4;
    int numData= 4;
    mcon::Matrixd mat1(numArray, numData);
    mat1[0][0] = 1;
    mat1[0][1] = 2;
    mat1[0][2] = 1;
    mat1[0][3] = 1;
    mat1[1][0] = 3;
    mat1[1][1] = 2;
    mat1[1][2] = 4;
    mat1[1][3] = 1;
    mat1[2][0] = 5;
    mat1[2][1] =-1;
    mat1[2][2] = 3;
    mat1[2][3] = 1;
    mat1[3][0] = 1;
    mat1[3][1] = 1;
    mat1[3][2] = 1;
    mat1[3][3] = 1;
    printf("mat1:\n");
    DumpMatrix(mat1, "%f");
    CHECK_VALUE(mat1.Determinant(), 1);
}

static void test_matrix_inverse(void)
{
    int numArray = 4;
    int numData= 4;
    mcon::Matrixd mat1(numArray, numData);
#if 0
    mat1[0][0] = 1;
    mat1[0][1] = 2;
    mat1[0][2] = 1;
    mat1[0][3] = 1;
    mat1[1][0] = 3;
    mat1[1][1] = 2;
    mat1[1][2] = 4;
    mat1[1][3] = 1;
    mat1[2][0] = 5;
    mat1[2][1] =-1;
    mat1[2][2] = 3;
    mat1[2][3] = 1;
    mat1[3][0] = 1;
    mat1[3][1] = 2;
    mat1[3][2] = 3;
    mat1[3][3] =-1;
#else
    mat1[0][0] = 3;
    mat1[0][1] = 2;
    mat1[0][2] = 1;
    mat1[0][3] = 0;
    mat1[1][0] = 1;
    mat1[1][1] = 2;
    mat1[1][2] = 3;
    mat1[1][3] = 4;
    mat1[2][0] = 2;
    mat1[2][1] = 1;
    mat1[2][2] = 0;
    mat1[2][3] = 1;
    mat1[3][0] = 2;
    mat1[3][1] = 0;
    mat1[3][2] = 2;
    mat1[3][3] = 1;
#endif
    printf("mat1:\n");
    DumpMatrix(mat1, "%f");
    mcon::Matrixd mat2(1,1), mat3(1, 1);
    mat2 = mat1.Inverse();
    printf("mat2:\n");
    DumpMatrix(mat2, "%f");
    mat3 = mat1.Multiply(mat2);
    printf("mat3:\n");
    DumpMatrix(mat3, "%f");

}

static void test_matrix_multiply(void)
{
    int numArray = 3;
    int numData= 4;
    mcon::Matrixd mat1(numArray, numData);
    for (int c = 1, i = 0; i < numArray; ++i)
    {
        for (int j = 0; j < numData; ++j, ++c)
        {
            mat1[i][j] = c;
        }
    }
    printf("mat1:\n");
    DumpMatrix(mat1, "%f");

    mcon::Matrixd mat2(mat1);
    mat2 = mat2.Transpose();
    printf("mat2:\n");
    DumpMatrix(mat2, "%f");

    mcon::Matrixd mat3(1, 1);

    mat3 = mat1.Multiply(mat2);
    printf("mat3:\n");
    DumpMatrix(mat3, "%f");
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

int main(void)
{
    test_transpose();
    test_matrix_multiply();
    test_matrix_determinant();
    test_Matrixd();
    test_matrix_inverse();

    return 0;
}
