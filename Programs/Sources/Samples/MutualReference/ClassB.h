#pragma once

#include <stdio.h>
#include "ClassA.h"

template <typename Type>
class ClassB
{
public:
ClassB(int n) : n(n) {}
operator=(ClassA &a)
{
    printf("A (%d) => B (%d)\n", a.n, n);
}
int n;
};
