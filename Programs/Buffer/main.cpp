#include <stdio.h>

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

int main(void)
{
    test_vector();
    test_matrix();
    test_vector_outrange();
    test_vector_operation();
    test_matrix_outrange();
    return 0;
}
