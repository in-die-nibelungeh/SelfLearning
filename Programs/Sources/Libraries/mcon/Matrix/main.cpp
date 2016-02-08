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
    for (size_t i = 0; i < m.GetRowLength(); ++i)
    {
        printf("| ");
        for (size_t j = 0; j < m.GetColumnLength(); ++j)
        {
            printf("%g", static_cast<TestType>(m[i][j]));
            printf("\t");
        }
        printf(" |\n");
    }
}

TestType initializer(size_t row, size_t rowLength, size_t column, size_t columnLength)
{
    return (row+rowLength) + (column+columnLength) * 10;
}

void test_Matrix()
{
    const size_t row = 4;
    const size_t col = 4;

    mcon::Matrix<TestType> m;

    CHECK_VALUE(m.IsNull(), true);
    CHECK_VALUE(m.GetRowLength(), 0);
    CHECK_VALUE(m.GetColumnLength(), 0);

    bool status = m.Resize(row, col);
    CHECK_VALUE(status, true);
    CHECK_VALUE(m.GetRowLength(), row);
    CHECK_VALUE(m.GetColumnLength(), col);

    const double init = 0;
    m = init;

    for (size_t k = 0; k < m.GetRowLength(); ++k )
    {
        for (size_t n = 0; n < m.GetColumnLength(); ++n )
        {
            CHECK_VALUE(m[k][n], init);
        }
    }

    m = 1;

    // Initialize
/*
    m.Initialize();
    for (size_t k = 0; k < m.GetRowLength(); ++k )
    {
        for (size_t n = 0; n < m.GetColumnLength(); ++n )
        {
            CHECK_VALUE(m[k][n], n);
        }
    }
    m.Initialize( 1 );
    m.Initialize( 1,  2);
*/
    m.Initialize( initializer );
    for (size_t k = 0; k < m.GetRowLength(); ++k )
    {
        for (size_t n = 0; n < m.GetColumnLength(); ++n )
        {
            CHECK_VALUE(m[k][n], initializer(k, m.GetRowLength(), n, m.GetColumnLength()));
        }
    }
}

static void test_matrix_determinant(void)
{
    int numArray = 4;
    int numData= 4;
    mcon::Matrix<TestType> mat1(numArray, numData);
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
    mcon::Matrix<TestType> mat1(numArray, numData);
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
    mcon::Matrix<TestType> mat2(1,1), mat3(1, 1);
    mat2 = mat1.Inverse();
    printf("mat2:\n");
    DumpMatrix(mat2, "%f");
    mat3 = mat1.Multiply(mat2);
    printf("mat3:\n");
    DumpMatrix(mat3, "%f");

}

static void test_matrix_multiply(void)
{
    const size_t numArray = 3;
    const size_t numData= 4;
    mcon::Matrix<TestType> mat1(numArray, numData);
    for (size_t c = 1, i = 0; i < numArray; ++i)
    {
        for (size_t j = 0; j < numData; ++j, ++c)
        {
            mat1[i][j] = c;
        }
    }
    printf("mat1:\n");
    DumpMatrix(mat1, "%f");

    mcon::Matrix<TestType> mat2(mat1);
    mat2 = mat2.Transpose();
    printf("mat2:\n");
    DumpMatrix(mat2, "%f");

    mcon::Matrix<TestType> mat3(1, 1);

    mat3 = mat1.Multiply(mat2);
    printf("mat3:\n");
    DumpMatrix(mat3, "%f");
}

static void test_transpose(void)
{
    const size_t numArray = 3;
    const size_t numData= 5;
    mcon::Matrix<TestType> mat(numArray, numData);
    for (size_t i = 0; i < numArray; ++i)
    {
        printf("mat [%d, 0 .. %d] ", static_cast<int32_t>(i), static_cast<int32_t>(numData-1) );
        for (size_t j = 0; j < numData; ++j)
        {
            mat[i][j] = (i+1)*10+(j+1);
            printf("%f ", mat[i][j]);
        }
        printf("\n");
    }

    mcon::Matrix<TestType> matt(1,1);
    matt = mat.Transpose();
    for (size_t i = 0; i < matt.GetRowLength(); ++i)
    {
        printf("mat [%d,0-%d] ", static_cast<int32_t>(i), static_cast<int32_t>(matt.GetColumnLength()-1) );
        for (size_t j = 0; j < matt.GetColumnLength(); ++j)
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
    test_matrix_inverse();
    test_Matrix();
    return 0;
}
