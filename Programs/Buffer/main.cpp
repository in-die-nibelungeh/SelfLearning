#include <stdio.h>

#include "Buffer.h"

using namespace Container;

static void test_vector(void)
{
    int CheckPoint = 0;
    int numData = 8;
    Vector<f64> iof64(numData/2);
    Vector<s16> ios16(numData);

    printf("CheckPoint: %d\n", CheckPoint++);
    for (int i = 0; i < iof64.GetNumOfData(); ++i)
    {
        iof64[i] = (i+1);
        printf("%d: %f\n", i, iof64[i]);
    }

    // Substituting
    ios16 = iof64;

    printf("CheckPoint: %d\n", CheckPoint++);
    printf("ios16.GetNumOfData()=%d\n", ios16.GetNumOfData());
    for (int i = 0; i < ios16.GetNumOfData(); ++i)
    {
        printf("%d: %d\n", i, ios16[i]);
    }

    printf("CheckPoint: %d\n", CheckPoint++);
    // Copy-constructor
    Vector<f32> iof32(ios16);

    printf("CheckPoint: %d\n", CheckPoint++);
    for (int i = 0; i < iof32.GetNumOfData(); ++i)
    {
        printf("%d: %f\n", i, iof32[i]);
    }
}

static void test_matrix(void)
{
    int numData = 8;
    int CheckPoint = 0;
    printf("CheckPoint: %d\n", CheckPoint++);
    int numArray = 2;
    Matrix<f64> m1(numArray, numData);

    for (int i = 0; i < m1.GetNumOfArray(); ++i)
    {
        for (int j = 0; j < m1.GetNumOfData(); ++j)
        {
            m1[i][j] = i * m1.GetNumOfData() + (j+1);
        }
    }

#define DUMP_MATRIX(m, t) \
    for (int i = 0; i < m.GetNumOfArray(); ++i) \
    {\
        for (int j = 0; j < m.GetNumOfData(); ++j)\
        {\
            printf(#m"[%d, %d] = "#t"\n", i, j, m[i][j]);\
        }\
    }

    DUMP_MATRIX(m1, %f);

    printf("CheckPoint: %d\n", CheckPoint++);
    Matrix<s16> m2(m1);

    printf("CheckPoint: %d\n", CheckPoint++);
    DUMP_MATRIX(m2, %d);

    Matrix<f32> m3(numArray/2, numData*2);

    printf("CheckPoint: %d\n", CheckPoint++);
    for (int i = 0; i < m3.GetNumOfArray(); ++i)
    {
        for (int j = 0; j < m3.GetNumOfData(); ++j)
        {
            m3[i][j] = (i+1) * 4 + (j+1) * 2;
            printf("m3[%d][%d]=%f\n", i, j, m3[i][j]);
        }
    }

    printf("CheckPoint: %d\n", __LINE__);
    m2 = m3;

    printf("CheckPoint: %d\n", __LINE__);
    DUMP_MATRIX(m2, %d);

    printf("CheckPoint: %d\n", __LINE__);
    Vector<f64> vec(m2[0]);

    printf("CheckPoint: %d\n", __LINE__);
    for (int i = 0; i < vec.GetNumOfData(); ++i)
    {
        printf("vec[%d]=%f\n", i, vec[i]);
    }
    Vector<s16> zero(0);
    Matrix<u16> zeros20(2, 0);
    Matrix<u16> zeros03(0, 3);

    /*
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
    return 0;
}
