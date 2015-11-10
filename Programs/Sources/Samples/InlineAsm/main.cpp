#include <stdio.h>
#include <stdint.h>

int main(int argc, char* argv[])
{
    const int iter = 10000000;
    int a = 1, b;
    for ( int i = 0; i < iter; ++i )
    {
        __asm__ volatile(
        ".rept 1000\n"
        "  add %[a], %[b]\n"
        ".endr\n"
        :[b]"=r"(b)
        :[a]"r"(a)
        );
    }
    printf("a=%d, b=%d\n", a, b);
    return 0;
}
