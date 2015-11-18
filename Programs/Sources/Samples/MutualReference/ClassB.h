#pragma once

#include <stdio.h>
#include "ClassA.h"

template <typename Type>
class ClassB
{
public:
    ClassB(int n) : n(n) {}

    ClassB<Type> operator=(ClassA& a)
    {
        printf("A (%d) => B (%d)\n", a.n, n);
        ClassB<Type> dummy(n+1);
        return dummy;
    }
    int n;
};
