#include <stdio.h>
#include <stdint.h>

inline uint64_t GetTickCount()
{
    uint64_t ret;
    __asm__ volatile ("rdtsc; shlq $32, %%rdx; orq %%rdx, %%rax" : "=A" (ret) :: "%rdx");
    return ret;
}

int main(int argc, char* argv[])
{
    setvbuf(stdout, (char *)NULL, _IONBF, 0);
    //const int iter = 1;
    const int cases = 10;
    uint64_t scores[cases];

    for (int iter = 1; iter < cases; ++iter)
    {
        int a = 1, b = 0;
        uint64_t base = GetTickCount();
        for ( int i = 0; i < iter * 100000; ++i )
        {
            __asm__ volatile(
            ".rept 1000\n"
            "  add %[a], %[b]\n"
            ".endr\n"
            :[b]"+r"(b)
            :[a]"r"(a)
            );
        }
        uint64_t end = GetTickCount();
        //printf("a=%d, b=%d (==%d)\n", a, b, static_cast<int>(iter * 1000L));
        scores[iter] = end - base;
        //printf("tick=%ld (iter=%d)\n", end-base, iter);
    }
    for (int iter = 1; iter < cases; ++iter)
    {
        printf("tick=%ld, ave=%ld (iter=%d)\n", scores[iter], scores[iter]/iter, iter);
    }
    return 0;
}
