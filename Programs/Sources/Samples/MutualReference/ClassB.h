#include <stdio.h>

#pragma once

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
