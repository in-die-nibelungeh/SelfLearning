
static void test_vector_api(void)
{
    // Empty vector.
    mcon::Vector<double> dvec;

    // Zero length
    CHECK_VALUE(dvec.GetLength(), 0);
    // IsNull() is true.
    CHECK_VALUE(dvec.IsNull(), true);

    // Resize
    const int length = 6;
    dvec.Resize(length);
    CHECK_VALUE(dvec.GetLength(), length);
    // IsNull() is false.
    CHECK_VALUE(dvec.IsNull(), false);

    for (int i = 0; i < length; ++i)
    {
        dvec[i] = i;
    }
    for (int i = 0; i < length; ++i)
    {
        CHECK_VALUE(dvec[i], i);
    }

    // operator+=(T)
    dvec += 1;
    for (int i = 0; i < length; ++i)
    {
        CHECK_VALUE(dvec[i], i+1);
    }
    // operator*=(T)
    dvec *= 10;
    for (int i = 0; i < length; ++i)
    {
        CHECK_VALUE(dvec[i], (i+1)*10);
    }
    // operator/=(T)
    dvec /= 5;
    for (int i = 0; i < length; ++i)
    {
        CHECK_VALUE(dvec[i], (i+1)*2);
    }
    // operator-=(T)
    dvec -= 5;
    for (int i = 0; i < length; ++i)
    {
        CHECK_VALUE(dvec[i], (i+1)*2-5);
    }
    mcon::Vector<double> dvec2(length*2);

    for (int i = 0; i < dvec2.GetLength(); ++i)
    {
        dvec2[i] = -(i+1);
    }
    // Copy
    dvec2.Copy(dvec);
    for (int i = 0; i < dvec2.GetLength(); ++i)
    {
        if (0 <= i && i < length)
        {
            CHECK_VALUE(dvec2[i], (i+1)*2-5);
        }
        else if (length <= i && i < length*2)
        {
            CHECK_VALUE(dvec2[i], -(i+1));
        }
    }
    // Substitution
    dvec2 = dvec;
    for (int i = 0; i < length; ++i)
    {
        CHECK_VALUE(dvec2[i], (i+1)*2-5);
    }
    dvec = 10;
    for (int i = 0; i < dvec2.GetLength(); ++i)
    {
        dvec2[i] = i + 1;
    }

    dvec += dvec2;
    for (int i = 0; i < length; ++i)
    {
        CHECK_VALUE(dvec[i], (i+1) + 10);
    }
    for (int i = 0; i < dvec2.GetLength(); ++i)
    {
        dvec2[i] = (i & 1) ? 1.0 : 2.0;
    }

    dvec *= dvec2;
    for (int i = 0; i < length; ++i)
    {
        CHECK_VALUE(dvec[i], ((i+1) + 10) * ((i & 1) ? 1.0 : 2.0));
    }
    dvec /= dvec2;
    for (int i = 0; i < length; ++i)
    {
        CHECK_VALUE(dvec[i], (i+1) + 10);
    }
    for (int i = 0; i < dvec2.GetLength(); ++i)
    {
        dvec2[i] = i + 1;
    }
    dvec -= dvec2;
    for (int i = 0; i < length; ++i)
    {
        CHECK_VALUE(dvec[i], 10);
    }
    // cast
    mcon::Vector<int> ivec(dvec);
    for (int i = 0; i < length; ++i)
    {
        CHECK_VALUE(ivec[i], 10);
    }
    // Carve-out
    for (int i = 0; i < dvec.GetLength(); ++i)
    {
        dvec[i] = i + 1;
    }
    dvec2 = dvec(0, 0);
    CHECK_VALUE(dvec2.IsNull(), true);
    dvec2 = dvec(-1, 1);
    CHECK_VALUE(dvec2.IsNull(), true);
    dvec2 = dvec(dvec.GetLength(), 1);
    CHECK_VALUE(dvec2.IsNull(), true);
    dvec2 = dvec(dvec.GetLength()-1, 5);
    CHECK_VALUE(dvec2.IsNull(), false);
    for (int i = 0; i < dvec2.GetLength(); ++i)
    {
        CHECK_VALUE(dvec2[i], 6);
    }

    dvec2 = dvec(1, 3);
    for (int i = 0; i < dvec2.GetLength(); ++i)
    {
        CHECK_VALUE(dvec2[i], i+2);
    }
    // Fifo
    printf("[Fifo]\n");
    double v = dvec2.Fifo(5);
    CHECK_VALUE(v, 2);
    for (int i = 0; i < dvec2.GetLength(); ++i)
    {
        CHECK_VALUE(dvec2[i], i+3);
    }
    // Unshift
    printf("[Unshift]\n");
    v = dvec2.Unshift(2);
    CHECK_VALUE(v, 5);
    for (int i = 0; i < dvec2.GetLength(); ++i)
    {
        CHECK_VALUE(dvec2[i], i+2);
    }

    // Cast (default function?)
    printf("[Cast]\n");
    ivec = static_cast< mcon::Vector<int> >(dvec);
    for (int i = 0; i < ivec.GetLength(); ++i)
    {
        CHECK_VALUE(ivec[i], i+1);
        CHECK_VALUE(dvec[i], i+1);
    }
    ivec = dvec;
    for (int i = 0; i < ivec.GetLength(); ++i)
    {
        CHECK_VALUE(ivec[i], i+1);
        CHECK_VALUE(dvec[i], i+1);
    }
    // Maximum/Minimum
    printf("[GetMaximum]\n");
    CHECK_VALUE(dvec2.GetMaximum(),4);
    printf("[GetMinimum]\n");
    CHECK_VALUE(dvec2.GetMinimum(),2);
    printf("[GetMaximumAbsolute/GetMinimumAbsolute]\n");
    // MaximumAbsolute/MinimumAbsolute
    {
        const int length = 6;
        mcon::Vector<double> vec(length);
        // 1, -2, 3, -4, 5, -6
        for (int i = 0; i < length; ++i)
        {
            vec[i] = (i+1) * ((i&1) ? -1 : 1);
        }
        double max_abs = vec.GetMaximumAbsolute();
        double min_abs = vec.GetMinimumAbsolute();
        CHECK_VALUE(max_abs, 6);
        CHECK_VALUE(min_abs, 1);
    }
    printf("[GetSum/Average/GetNorm]\n");
    {
        const int length = 10;
        mcon::Vector<double> v(length);
        for (int i = 0; i < length; ++i)
        {
            v[i] = i + 1;
        }
        const double sum = v.GetSum();
        const double ave = v.GetAverage();
        CHECK_VALUE(sum, 55);
        CHECK_VALUE(ave, 5.5);
        v[length - 1] = 2;
        const double norm = v.GetNorm();
        CHECK_VALUE(norm, 17);
    }
    printf("END\n");
}
