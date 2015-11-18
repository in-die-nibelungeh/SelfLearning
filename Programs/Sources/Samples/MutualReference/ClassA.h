#pragma once

#include <stdio.h>


template <typename Type> class ClassB;
class ClassA
{
public:
    ClassA(int n) : n(n) {}

    template <typename Type>
    ClassA operator=(ClassB<Type>& b)
    {
        printf("B (%d) => A (%d)\n", b.n, n);
        ClassA dummy(n+1);
        return dummy;
    }
    int n;
};
