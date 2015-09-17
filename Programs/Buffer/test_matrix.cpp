
static void test_matrix_api(void)
{
    int length = 6;
    mcon::Vector<double> dvec(length);
    for (int i = 0; i < length; ++i)
    {
        dvec[i] = i + 1;
    }
    mcon::Matrix<double> m(1,1);

    m = dvec.Transpose();

    CHECK_VALUE(m.GetNumOfArray(), length);
    CHECK_VALUE(m.GetNumOfData(), 1);

    for (int i = 0; i < length; ++i)
    {
        CHECK_VALUE(m[i][0], i+1);
    }
}
