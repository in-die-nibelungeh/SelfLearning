#include <stdio.h>

#include "Stopwatch.h"
#include "Clockwatch.h"

double g_Value = 0.0;

#include <stdio.h>
#include <unistd.h>          // sysconf/_SC_CLK_TCK
#include <sys/resource.h>    // getrusage
#include <sys/time.h>        // clock/CLOCKS_PER_SEC/gettimeofday
#include <sys/times.h>       // times
#include <stdint.h>          // int64_t

//-----------------------------------------------------------------
/* Provided for compatibility with code that assumes that
   the presence of gettimeofday function implies a definition
   of struct timezone. */

/*
   Implementation as per:
   The Open Group Base Specifications, Issue 6
   IEEE Std 1003.1, 2004 Edition

   The timezone pointer arg is ignored.  Errors are ignored.
*/

#ifdef  __cplusplus
#include <Windows.h>
//void GetSystemTimeAsFileTime(FILETIME*);

inline int mtgettimeofday(struct timeval* p, void* tz /* IGNORED */)
{
    union {
        long long int ns100; /*time since 1 Jan 1601 in 100ns units */
        FILETIME ft;
    } now;
    GetSystemTimeAsFileTime( &(now.ft) );
    p->tv_usec=(long)((now.ns100 / 10LL) % 1000000LL );
    p->tv_sec= (long)((now.ns100-(116444736000000000LL))/10000000LL);
    printf("now=%lld\n", now.ns100);
    return 0;
}

#else
    /* Must be defined somewhere else */
    int gettimeofday(struct timeval* p, void* tz /* IGNORED */);
#endif

//-----------------------------------------------------------------
double gettimeofday_sec()
{
    struct timeval tv;
    mtgettimeofday(&tv,NULL);
    return tv.tv_sec + tv.tv_usec * 1e-6;
}

double getrusage_sec()
{
    struct rusage t;
    struct timeval tv;
    getrusage(RUSAGE_SELF, &t);
    tv = t.ru_utime;
    return tv.tv_sec + tv.tv_usec * 1e-6;
}

clock_t times_clock()
{
    struct tms t;
    return times(&t);
}

double gettime_sec()
{
    double sec;
    struct timespec tp;
    clock_gettime(CLOCK_REALTIME, &tp);
    sec  = tp.tv_nsec * 1.0e-9;
    sec += tp.tv_sec;
    return sec;
}

#define RDTSC(X) __asm__ volatile ("rdtsc" : "=A" (X))
//-----------------------------------------------------------------

static void test_methods(void)
{
    clock_t ct1,ct2;     // clock() 関数を使った時間計測
    double rut1,rut2;    // getrusage() 関数を使った時間計測
    double todt1,todt2;  // gettimeofday() 関数を使った時間計測
    double cgt1,cgt2;    // clock_gettime()
    clock_t tmt1,tmt2;   // times() 関数を使った時間計測
    uint64_t tsc1,tsc2;  // RDTSC (read-time stamp counter) を使ったクロック計測

    printf("sizof(long unsigned int)=%ld\n", sizeof(long unsigned int));
    printf("sizof(uint64_t)=%ld\n", sizeof(uint64_t));
    printf("sizof(long long unsigned int)=%ld\n", sizeof(long long unsigned int));

    printf("CLOCKS_PER_SEC : %d\n", CLOCKS_PER_SEC);
    printf("sysconf(_SC_CLK_TCK) : %ld\n", sysconf(_SC_CLK_TCK));

#if 1
    mutl::Clockwatch cw;
    mutl::Stopwatch sw;
    for (int k = 0; k < 10; ++k)
    {
        sleep(1);
        uint64_t c = cw.Push();
        double s = sw.Push();
        printf("time: sw=%ld, cw=%g (%g)\n", c, s, c/s);
    }
#endif

    // * SHORT TERM
    ct1   = clock();
    rut1  = getrusage_sec();
    todt1 = gettimeofday_sec();
    tmt1  = times_clock();
    cgt1 = gettime_sec();
    RDTSC(tsc1);

    // 時間を計測する処理
    const int iter = 1000000;
    int a = 1, b;
    for ( int i = 0; i < iter; ++i )
    {
        __asm__ volatile(
        ".rept 1000\n"
        "  nop\n"
        ".endr\n"
        :[b]"=r"(b)
        :[a]"r"(a)
        );
    }
    ct2   = clock();
    rut2  = getrusage_sec();
    todt2 = gettimeofday_sec();
    tmt2  = times_clock();
    cgt2 = gettime_sec();
    RDTSC(tsc2);

    printf("* NOP process\n");
    printf("    clock: time of %lf[sec], %ld clock\n", (double)(ct2-ct1)/CLOCKS_PER_SEC, ct2-ct1);
    printf("   rusage: time of %lf[sec]\n", rut2-rut1);
    printf("timeofday: time of %lf[sec]\n", todt2-todt1);
    printf("  gettime: time of %lf[sec]\n", cgt2-cgt1);
    printf("    times: time of %lf[sec]\n", (double)(tmt2-tmt1) / sysconf(_SC_CLK_TCK));
    printf("    RDTSC: elapsed tsc = %ld : %ld ==> %ld\n", tsc2-tsc1, tsc1, tsc2);

    // * SLEEP WAIT
    ct1   = clock();
    rut1  = getrusage_sec();
    todt1 = gettimeofday_sec();
    tmt1  = times_clock();
    cgt1 = gettime_sec();
    RDTSC(tsc1);

    // 時間を計測する処理
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

    ct2   = clock();
    rut2  = getrusage_sec();
    todt2 = gettimeofday_sec();
    tmt2  = times_clock();
    cgt2 = gettime_sec();
    RDTSC(tsc2);

    printf("* ADD process\n");
    printf("    clock: time of %lf[sec], %ld clock\n", (double)(ct2-ct1)/CLOCKS_PER_SEC, ct2-ct1);
    printf("   rusage: time of %lf[sec]\n", rut2-rut1);
    printf("timeofday: time of %lf[sec]\n", todt2-todt1);
    printf("  gettime: time of %lf[sec]\n", cgt2-cgt1);
    printf("    times: time of %lf[sec]\n", (double)(tmt2-tmt1) / sysconf(_SC_CLK_TCK));
    printf("    RDTSC: elapsed tsc = %ld : %ld ==> %ld\n", tsc2-tsc1, tsc1, tsc2);

}

void test_stopwatch(void)
{
    mutl::Stopwatch sw;
    const int a = 0;
    int b;
    int limit = 100;
    for ( int k = 0; k < 6; ++k, limit *= 10)
    {
        sw.Tick();
        // 時間を計測する処理
        for ( int i = 0; i < limit; ++i )
        {
            __asm__ volatile(
            ".rept 1000\n"
            "  add %[a], %[b]\n"
            ".endr\n"
            :[b]"=r"(b)
            :[a]"r"(a)
            );
        }
        printf("Time consumed: %g\n", sw.Tick());
    }
}

int main(void)
{
    test_methods();
    test_stopwatch();
    return 0;
}
