#include <stdio.h>
#include <math.h>
#include <typeinfo>

#include "mcon.h"

template <class T>
void DumpMatrix(mcon::Matrix<T>&m, const char* fmt = NULL)
{
    if (typeid(int) == typeid(T))
    {
    }
    else if (typeid(double) == typeid(T))
    {
    }
    for (uint i = 0; i < m.GetRowLength(); ++i)
    {
        printf("| ");
        for (uint j = 0; j < m.GetColumnLength(); ++j)
        {
            printf("%g", static_cast<double>(m[i][j]));
            printf("\t");
        }
        printf(" |\n");
    }
}

extern void test_vector_api();
extern void test_matrix_api();

static void test_matrix_determinant(void)
{
    const uint numArray = 4;
    const uint numData= 4;
    mcon::Matrix<double> mat1(numArray, numData);
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
    const uint numArray = 4;
    const uint numData= 4;
    mcon::Matrix<double> mat1(numArray, numData);
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
    mcon::Matrix<double> mat2(1,1), mat3(1, 1);
    mat2 = mat1.Inverse();
    printf("mat2:\n");
    DumpMatrix(mat2, "%f");
    mat3 = mat1.Multiply(mat2);
    printf("mat3:\n");
    DumpMatrix(mat3, "%f");

}

static void test_matrix_multiply(void)
{
    const uint numArray = 3;
    const uint numData= 4;
    mcon::Matrix<double> mat1(numArray, numData);
    for (uint c = 1, i = 0; i < numArray; ++i)
    {
        for (uint j = 0; j < numData; ++j, ++c)
        {
            mat1[i][j] = c;
        }
    }
    printf("mat1:\n");
    DumpMatrix(mat1, "%f");

    mcon::Matrix<double> mat2(mat1);
    mat2 = mat2.Transpose();
    printf("mat2:\n");
    DumpMatrix(mat2, "%f");

    mcon::Matrix<double> mat3(1, 1);

    mat3 = mat1.Multiply(mat2);
    printf("mat3:\n");
    DumpMatrix(mat3, "%f");
}

static void test_transpose(void)
{
    const uint numArray = 3;
    const uint numData= 5;
    mcon::Matrix<double> mat(numArray, numData);
    for (uint i = 0; i < numArray; ++i)
    {
        printf("mat [%d, 0 .. %d] ", i, numData-1);
        for (uint j = 0; j < numData; ++j)
        {
            mat[i][j] = (i+1)*10+(j+1);
            printf("%f ", mat[i][j]);
        }
        printf("\n");
    }

    mcon::Matrix<double> matt(1,1);
    matt = mat.Transpose();
    for (uint i = 0; i < matt.GetRowLength(); ++i)
    {
        printf("mat [%d,0-%d] ", i, matt.GetColumnLength()-1);
        for (uint j = 0; j < matt.GetColumnLength(); ++j)
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
    test_vector_api();
    //benchmark_vector_api();
    test_matrix_api();
    test_matrix_inverse();

    return 0;
}
