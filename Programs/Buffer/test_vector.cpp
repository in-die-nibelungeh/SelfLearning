
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
    for (int i = lower; i < upper; ++i)
    {
        if (i == 0)
        {
            CHECK_VALUE(dvec2[i], 6);
        }
        else
        {
            CHECK_VALUE(dvec2[i], 0);
        }
    }

    dvec2 = dvec(1, 3);
    for (int i = lower; i < upper; ++i)
    {
        if (0 <= i && i < dvec2.GetLength())
        {
            CHECK_VALUE(dvec2[i], i+2);
        }
        else
        {
            CHECK_VALUE(dvec2[i], 0);
        }
    }
    // Fifo
    printf("[Fifo]\n");
    double v = dvec2.Fifo(5);
    CHECK_VALUE(v, 2);
    for (int i = lower; i < upper; ++i)
    {
        if (0 <= i && i < dvec2.GetLength())
        {
            CHECK_VALUE(dvec2[i], i+3);
        }
        else
        {
            CHECK_VALUE(dvec2[i], 0);
        }
    }
    // Unshift
    printf("[Unshift]\n");
    v = dvec2.Unshift(2);
    CHECK_VALUE(v, 5);
    for (int i = lower; i < upper; ++i)
    {
        if (0 <= i && i < dvec2.GetLength())
        {
            CHECK_VALUE(dvec2[i], i+2);
        }
        else
        {
            CHECK_VALUE(dvec2[i], 0);
        }
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

    printf("END\n");
}
