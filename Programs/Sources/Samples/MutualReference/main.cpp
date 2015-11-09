#if 0
#include "Test.h"
#include "TestTemplate.h"

int main(void)
{
    Test t;
    TestTemplate<int> ti;

    t = ti;
    ti = t;

    return 0;
}
#endif

#include "Class.h"

int main(void)
{
    ClassA a;
    ClassB b;

    a = b;
    b = a;

    return 0;
}
