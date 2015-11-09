#include <stdio.h>

#pragma once

class ClassB;

class ClassA
{
public:
operator=(ClassB &b);
};

class ClassB
{
public:
operator=(ClassA &a)
{
    printf("A => B\n");
}
};

inline ClassA::operator=(ClassB &b)
{
    printf("B => A\n");
}
