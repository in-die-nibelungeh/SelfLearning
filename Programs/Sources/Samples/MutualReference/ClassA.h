#include <stdio.h>

#pragma once

template <typename Type>
class ClassB;

class ClassA
{
public:
ClassA(int n) : n(n) {}
template <typename Type>
operator=(ClassB<Type> &b)
{
    printf("B (%d) => A (%d)\n", b.n, n);
};
int n;
};
