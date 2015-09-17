
static void test_matrix_api(void)
{
    int length = 6;
    mcon::Vector<double> dvec(length);
    for (int i = 0; i < length; ++i)
    {
        dvec[i] = i + 1;
    }
    mcon::Matrix<double> m;

    CHECK_VALUE(m.IsNull(), true);

    m = dvec.Transpose();

    CHECK_VALUE(m.GetNumOfArray(), length);
    CHECK_VALUE(m.GetNumOfData(), 1);
    for (int i = 0; i < length; ++i)
    {
        CHECK_VALUE(m[i][0], i+1);
    }
    mcon::Matrix<double> m1(dvec.ToMatrix());

    CHECK_VALUE(m1.GetNumOfArray(), 1);
    CHECK_VALUE(m1.GetNumOfData(), length);
    for (int i = 0; i < length; ++i)
    {
        CHECK_VALUE(m1[0][i], i+1);
    }
}
