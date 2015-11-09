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
