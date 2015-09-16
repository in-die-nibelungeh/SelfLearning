#include <stdio.h>
#include <math.h>

#include "Matrix.h"

template <class T>
void DumpMatrix(mcon::Matrix<T>&m, const char* fmt)
{
    for (int i = 0; i < m.GetNumOfArray(); ++i)
    {
        printf("| ");
        for (int j = 0; j < m.GetNumOfData(); ++j)
        {
            printf(fmt, m[i][j]);
            printf("\t");
        }
        printf(" |\n");
    }
}

static void test_vector_api(void)
{
    const int lower = -2, upper = 8;

    // Empty vector.
    mcon::Vector<double> dvec;

    // Zero length
    CHECK_VALUE(dvec.GetLength(), 0);

    // Accesses to out-of-range area.
    for (int i = lower; i <= upper; ++i)
    {
        CHECK_VALUE(dvec[i], 0);
    }
    // Resize
    int length = 6;
    dvec.Resize(length);
    CHECK_VALUE(dvec.GetLength(), length);

    for (int i = lower; i <= upper; ++i)
    {
        dvec[i] = i;//cos(2*M_PI*i/upper);
    }
    for (int i = lower; i <= upper; ++i)
    {
        if (0 <= i && i < length)
        {
            CHECK_VALUE(dvec[i], i);//cos(2*M_PI*i/upper));
        }
        else
        {
            CHECK_VALUE(dvec[i], 0);
        }
    }

    // operator+=(T)
    dvec += 1;
    for (int i = lower; i <= upper; ++i)
    {
        if (0 <= i && i < length)
        {
            CHECK_VALUE(dvec[i], i+1);
        }
        else
        {
            CHECK_VALUE(dvec[i], 0);
        }
    }
    // operator*=(T)
    dvec *= 10;
    for (int i = lower; i <= upper; ++i)
    {
        if (0 <= i && i < length)
        {
            CHECK_VALUE(dvec[i], (i+1)*10);
        }
        else
        {
            CHECK_VALUE(dvec[i], 0);
        }
    }
    // operator/=(T)
    dvec /= 5;
    for (int i = lower; i <= upper; ++i)
    {
        if (0 <= i && i < length)
        {
            CHECK_VALUE(dvec[i], (i+1)*2);
        }
        else
        {
            CHECK_VALUE(dvec[i], 0);
        }
    }
    // operator-=(T)
    dvec -= 5;
    for (int i = lower; i <= upper; ++i)
    {
        if (0 <= i && i < length)
        {
            CHECK_VALUE(dvec[i], (i+1)*2-5);
        }
        else
        {
            CHECK_VALUE(dvec[i], 0);
        }
    }
    mcon::Vector<double> dvec2(length*2);

    for (int i = 0; i < dvec2.GetLength(); ++i)
    {
        dvec2[i] = -(i+1);
    }
    // Copy
    dvec2.Copy(dvec);
    for (int i = lower; i <= length*2+1; ++i)
    {
        //printf("%d\n", i);
        if (0 <= i && i < length)
        {
            CHECK_VALUE(dvec2[i], (i+1)*2-5);
        }
        else if (length <= i && i < length*2)
        {
            CHECK_VALUE(dvec2[i], -(i+1));
        }
        else
        {
            CHECK_VALUE(dvec2[i], 0);
        }
    }
    // Substitution
    dvec2 = dvec;
    for (int i = lower; i <= length; ++i)
    {
        //printf("%d\n", i);
        if (0 <= i && i < length)
        {
            CHECK_VALUE(dvec2[i], (i+1)*2-5);
        }
        else
        {
            CHECK_VALUE(dvec2[i], 0);
        }
    }
    dvec = 10;
    for (int i = 0; i < dvec2.GetLength(); ++i)
    {
        dvec2[i] = i + 1;
    }

    dvec += dvec2;
    for (int i = lower; i < upper; ++i)
    {
        if (0 <= i && i < dvec.GetLength())
        {
            CHECK_VALUE(dvec[i], (i+1) + 10);
        }
        else
        {
            CHECK_VALUE(dvec[i], 0);
        }
    }

    for (int i = 0; i < dvec2.GetLength(); ++i)
    {
        dvec2[i] = (i & 1) ? 1.0 : 2.0;
    }

    dvec *= dvec2;
    for (int i = lower; i < upper; ++i)
    {
        if (0 <= i && i < dvec.GetLength())
        {
            CHECK_VALUE(dvec[i], ((i+1) + 10) * ((i & 1) ? 1.0 : 2.0));
        }
        else
        {
            CHECK_VALUE(dvec[i], 0);
        }
    }
    dvec /= dvec2;
    for (int i = lower; i < upper; ++i)
    {
        if (0 <= i && i < dvec.GetLength())
        {
            CHECK_VALUE(dvec[i], (i+1) + 10);
        }
        else
        {
            CHECK_VALUE(dvec[i], 0);
        }
    }
    for (int i = 0; i < dvec2.GetLength(); ++i)
    {
        dvec2[i] = i + 1;
    }
    dvec -= dvec2;
    for (int i = lower; i < upper; ++i)
    {
        if (0 <= i && i < dvec.GetLength())
        {
            CHECK_VALUE(dvec[i], 10);
        }
        else
        {
            CHECK_VALUE(dvec[i], 0);
        }
    }
    // cast
    mcon::Vector<int> ivec(dvec);
    for (int i = lower; i < upper; ++i)
    {
        if (0 <= i && i < ivec.GetLength())
        {
            CHECK_VALUE(ivec[i], 10);
        }
        else
        {
            CHECK_VALUE(ivec[i], 0);
        }
    }

    /*
    mcon::Vector<int> ivec;
    static_cast<int>(dvec);
    ivec.Resize(dvec.GetLength());
    for (int i = 0; i < ivec.GetLength(); ++i)
    {
        ivec[i] = static_cast<int>(dvec[i]);
    }
    */
    printf("END\n");
}

static void test_matrix_determinant(void)
{
    int numArray = 4;
    int numData= 4;
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
    CHECK_VALUE_FLT(mat1.Determinant(), 1);
}

static void test_matrix_inverse(void)
{
    int numArray = 4;
    int numData= 4;
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
    int numArray = 3;
    int numData= 4;
    mcon::Matrix<double> mat1(numArray, numData);
    for (int c = 1, i = 0; i < numArray; ++i)
    {
        for (int j = 0; j < numData; ++j, ++c)
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
    int numArray = 3, numData= 5;
    mcon::Matrix<double> mat(numArray, numData);
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

    mcon::Matrix<double> matt(1,1);
    matt = mat.Transpose();
    for (int i = 0; i < matt.GetNumOfArray(); ++i)
    {
        printf("mat [%d,0-%d] ", i, matt.GetNumOfData()-1);
        for (int j = 0; j < matt.GetNumOfData(); ++j)
        {
            printf("%f ", matt[i][j]);
        }
        printf("\n");
    }
}

#include "Buffer.h"

using namespace Container;

static void test_vector(void)
{
    int CheckPoint = 0;
    int numData = 8;
    DEBUG_LOG("Calling Vector<T> constructors\n");
    Vector<double> iodouble(numData/2);
    Vector<int16_t> ioint16_t(numData);

    DEBUG_LOG("Initializing objects with using Vector<T>::operator[]\n");
    for (int i = 0; i < iodouble.GetNumOfData(); ++i)
    {
        iodouble[i] = (i+1);
        printf("%d: %f\n", i, iodouble[i]);
    }

    // Substituting
    DEBUG_LOG("Substituting with using Vector<T>::operator=\n");
    ioint16_t = iodouble;

    for (int i = 0; i < ioint16_t.GetNumOfData(); ++i)
    {
        printf("%d: %d\n", i, ioint16_t[i]);
    }

    DEBUG_LOG("Substituting with using Vector<T>::copy-constructor\n");
    // Copy-constructor
    Vector<float> iofloat(ioint16_t);

    for (int i = 0; i < iofloat.GetNumOfData(); ++i)
    {
        printf("%d: %f\n", i, iofloat[i]);
    }
}

static void test_matrix(void)
{
    int numData = 8;
    int numArray = 2;
    DEBUG_LOG("Calling Matrix<T> constructors\n");
    Matrix<double> m1(numArray, numData);

    for (int i = 0; i < m1.GetNumOfArray(); ++i)
    {
        for (int j = 0; j < m1.GetNumOfData(); ++j)
        {
            m1[i][j] = i * m1.GetNumOfData() + (j+1);
        }
    }

#define DUMP_MATRIX(m, t) \
    do { printf("Dump " # m ":\n"); \
    for (int i = 0; i < m.GetNumOfArray(); ++i) \
    {\
        for (int j = 0; j < m.GetNumOfData(); ++j)\
        {\
            printf("    "#m"[%d, %d] = "#t"\n", i, j, m[i][j]);\
        }\
    } } while (0)

    DUMP_MATRIX(m1, %f);

    DEBUG_LOG("Substituting with using Matrix<T>::copy-constructor\n");
    Matrix<int16_t> m2(m1);

    DUMP_MATRIX(m2, %d);

    Matrix<float> m3(numArray/2, numData*2);

    for (int i = 0; i < m3.GetNumOfArray(); ++i)
    {
        for (int j = 0; j < m3.GetNumOfData(); ++j)
        {
            m3[i][j] = (i+1) * 4 + (j+1) * 2;
            printf("m3[%d][%d]=%f\n", i, j, m3[i][j]);
        }
    }

    DEBUG_LOG("Substituting with using Matrix<T>::=\n");
    m2 = m3;

    DUMP_MATRIX(m2, %d);

    Vector<double> vec(m2[0]);

    for (int i = 0; i < vec.GetNumOfData(); ++i)
    {
        printf("vec[%d]=%f\n", i, vec[i]);
    }
    /*
    Vector<int16_t> zero(0);
    Matrix<uint16_t> zeros20(2, 0);
    Matrix<uint16_t> zeros03(0, 3);

    printf("CheckPoint: %d\n", __LINE__);
    zero = zeros20[0];
    printf("CheckPoint: %d\n", __LINE__);
    zero = zeros20[1];
    printf("CheckPoint: %d\n", __LINE__);
    */
}

#define SHOW_IVALUE(v) \
    do { for (int _i = 0; _i < v.GetNumOfData(); ++_i) { \
        printf("%4i ", v[_i]); } printf("\n"); } while(0)

#define SHOW_FVALUE(v) \
    do { for (int _i = 0; _i < v.GetNumOfData(); ++_i) { \
        printf("%f ", v[_i]); } printf("\n"); } while(0)

#define ITER(_i, v, s) \
    for (int _i = 0; _i < v.GetNumOfData(); ++_i) { s; }

static void test_vector_outrange(void)
{
    int numData = 8;
    Container::Vector<int> vec(numData);
    int indices[] =
    {
        0, 1, 2, 6, 7, 8, 10000, -1, -1000
    };

    for (int i = 0; i < vec.GetNumOfData(); vec[i] = ++i);

    for (int i = 0; i < sizeof(indices)/sizeof(int); ++i)
    {
        int index = indices[i];
        printf("vec[%5d]=%d ... ", index, vec[index]);
        vec[index] = index;
        printf("vec[%5d]=%d\n", index, vec[index]);
    }
}

static void test_matrix_outrange(void)
{
    int numArray = 2;
    int numData = 8;
    Container::Matrix<int> mat(numArray, numData);
    int array_indices[] = { 0, 2, 100, -2};
    int indices[] =
    {
        0, 1, 2, 6, 7, 8, 10000, -1, -1000
    };

    for (int a = 0; a < mat.GetNumOfArray(); ++a)
    {
        Vector<int>& vec = mat[a];
        for (int i = 0; i < mat.GetNumOfData(); ++i)
        {
            vec[i] = (i+1) + (a+1)*10;
            printf("mat[%5d][%5d]=%d (%d)\n", a, i, mat[a][i], vec[i]);
        }
    }

    for (int a = 0; a < sizeof(array_indices)/sizeof(int); ++a)
    {
        int ai = array_indices[a];
        Vector<int>& vec = mat[ai];
        for (int i = 0; i < sizeof(indices)/sizeof(int); ++i)
        {
            int index = indices[i];
            printf("mat[%5d][%5d]=%d ... ", ai, index, vec[index]);
            vec[index] = index;
            printf("mat[%5d][%5d]=%d\n", ai, index, vec[index]);
        }
    }
}


static void test_vector_operation(void)
{
    int numData = 8;
    Container::Vector<int> ivec(numData);

    ITER(i, ivec, ivec[i] = i + 1);

    SHOW_IVALUE(ivec);
    ivec += 2;
    SHOW_IVALUE(ivec);
    ivec *= 2;
    SHOW_IVALUE(ivec);
    ivec -= 1;
    SHOW_IVALUE(ivec);
    ivec /= 2.0;
    SHOW_IVALUE(ivec);

    Container::Vector<int> ivec1(numData/2);
    ITER(i, ivec1, ivec1[i] = (i+1)*100);
    ivec += ivec1;
    SHOW_IVALUE(ivec);

    Container::Vector<int> ivec2(numData);
    ITER(i, ivec2, ivec2[i] = (i+1)*10);
    ivec -= ivec2;
    SHOW_IVALUE(ivec);

    Container::Vector<int> ivec3(numData);
    ITER(i, ivec3, ivec3[i] = (i+1));
    ivec *= ivec3;
    SHOW_IVALUE(ivec);

    Container::Vector<int> ivec4(numData);
    ITER(i, ivec4, ivec4[i] = (2*i+1));
    ivec /= ivec4;
    SHOW_IVALUE(ivec);
}

class Test
{
public:
    Test()
    {printf("%s\n", __func__);}
    ~Test()
    {printf("%s\n", __func__);}
};

int main(void)
{
#if 0
    test_vector();
    test_matrix();
    test_vector_outrange();
    test_vector_operation();
    test_matrix_outrange();
#endif
    /*
    Test t;
    //test_transpose();
    mcon::Matrix<double> m(2,2);
#define dump() \
    for ( int i = 0; i < 2; ++i)\
    {\
        for (int j = 0; j < 2; ++j)\
        {\
            printf("%d, %d: %p\n", i, j, &m[i][j]);\
        }\
    }
    dump();
    printf("**** mat2(m)\n");
    mcon::Matrix<double> mat2(m);
    dump();
    printf("**** Transpose\n");
    mat2 = m.Transpose();
    dump();
    printf("**** Transpose2\n");
    mat2 = m.Transpose();
    dump();
    */
    //test_matrix_multiply();
    //test_matrix_determinant();
    test_vector_api();
    //test_matrix_inverse();
    return 0;
}
