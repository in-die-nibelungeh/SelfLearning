#pragma once

#include <stdio.h>

class Basis
{
public:
    Basis()
    {
        printf("Basis\n");
    }
    ~Basis() {};
};

class Basis2
{
public:
    Basis2()
    {
        printf("Basis2\n");
    }
    ~Basis2() {};
};

template <typename Type>
class Class : public Basis
{
public:
    void Print() const
    {
        printf("Type: %ld\n", sizeof(Type));
    }
};

template <>
class Class<int> : public Basis2
{
public:
    void Print() const;
};
