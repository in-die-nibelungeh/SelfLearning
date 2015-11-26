#include <stdio.h>

class Test
{
public:
    Test() : m_Value(0) {}

    Test operator*(double v) const
    {
        return *this;
    }
private:
    int m_Value;
};

Test operator*(double v, Test& test)
{
    return test;
}

int main(void)
{
    Test test;
    Test test1 = 1.0 * test;
    Test test2 = test * 1.0;
    return 0;

}
