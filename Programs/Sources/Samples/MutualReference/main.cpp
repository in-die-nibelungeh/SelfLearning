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

#include "ClassA.h"
#include "ClassB.h"

int main(void)
{
    ClassA a(2);
    ClassB<int> b(3);

    a = b;
    b = a;

    return 0;
}
