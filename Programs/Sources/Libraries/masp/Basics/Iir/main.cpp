#include <stdio.h>
#include <math.h>
#include <stdint.h>
#include <string>

#include "mcon.h"
#include "mfio.h"
#include "mtbx.h"
#include "masp.h"

static const double g_Pi(M_PI);

static void generate_sample(int baseFreqs[])
{
    char name[256];
    int fs = 48000;
    int freq;
    int maxFreq = 0;
    int minFreq = 48000;
    int duration = 10;
    const double amp = 2048.0 * 2;
    mcon::Vector<int16_t> sineMixed(fs * duration);
    printf("Generating mixed sine waveform\n");
    for (int i = 0; baseFreqs[i] != 0; ++i)
    {
        int baseFreq = baseFreqs[i];
        for (freq = baseFreq; ; freq *= 2)
        {
            mcon::Vector<double> sineSingle(fs * duration);
            mtbx::WaveGen wg(fs, freq, mtbx::WaveGen::WT_SINE);
            wg.GenerateWaveform(sineSingle, amp);
            sineMixed += sineSingle;
            if (maxFreq < freq) { maxFreq = freq; }
            if (minFreq > freq) { minFreq = freq; }
            if ((2*freq) > fs/2)
            {
                printf("exit at %d\n", freq);
                break;
            }
        }
    }
    sprintf(name, "sine_%d-%d.wav", minFreq, maxFreq);
    printf("Write the mixed sine waveform to %s\n", name);
    mfio::Wave wave;
    wave.SetMetaData(fs, 1, 16);
    if (0 != wave.Write(name, sineMixed))
    {
        printf("Failed in writing %s\n", name);
    }
}

static void test_filtering(void)
{
    std::string name("sine_131-21120");
    mcon::Vector<double> sweep;
    mfio::Wave wave;
    wave.Read(name + std::string(".wav"), sweep);
    struct mfio::Wave::MetaData metaData = wave.GetMetaData();
    double Qf = 1 / sqrt(2);
    int fs = metaData.samplingRate;
    double fc = 2000;
    masp::iir::Biquad lpf(Qf, fc, masp::iir::Biquad::LPF, fs);
    masp::iir::Biquad hpf(Qf, fc, masp::iir::Biquad::HPF, fs);
    masp::iir::Biquad bpf(Qf, fc, masp::iir::Biquad::BPF, fs);
    masp::iir::Biquad bef(Qf, fc, masp::iir::Biquad::BEF, fs);

    mcon::Vector<int16_t> sweep_filtered_lpf(sweep.GetLength());
    mcon::Vector<int16_t> sweep_filtered_hpf(sweep.GetLength());
    mcon::Vector<int16_t> sweep_filtered_bpf(sweep.GetLength());
    mcon::Vector<int16_t> sweep_filtered_bef(sweep.GetLength());

    printf("fc=%f\n", masp::iir::Biquad::ConvertD2A(fc, fs));

    struct masp::iir::Biquad::Coefficients coef = lpf.GetCoefficients();
    printf("LPF: b0=%f,b1=%f,b2=%f,a1=%f,a2=%f\n",
        coef.b[0], coef.b[1], coef.b[2], coef.a[0], coef.a[1]);
    coef = hpf.GetCoefficients();
    printf("HPF: b0=%f,b1=%f,b2=%f,a1=%f,a2=%f\n",
        coef.b[0], coef.b[1], coef.b[2], coef.a[0], coef.a[1]);
    coef = bpf.GetCoefficients();
    printf("BPF: b0=%f,b1=%f,b2=%f,a1=%f,a2=%f\n",
        coef.b[0], coef.b[1], coef.b[2], coef.a[0], coef.a[1]);
    coef = bef.GetCoefficients();
    printf("BEF: b0=%f,b1=%f,b2=%f,a1=%f,a2=%f\n",
        coef.b[0], coef.b[1], coef.b[2], coef.a[0], coef.a[1]);

    for (uint i = 0; i < sweep.GetLength(); ++i)
    {
        double audioIn = static_cast<double>(sweep[i]);
        double v = lpf.ApplyFilter(audioIn); sweep_filtered_lpf[i] = static_cast<int16_t>(v);
               v = hpf.ApplyFilter(audioIn); sweep_filtered_hpf[i] = static_cast<int16_t>(v);
               v = bpf.ApplyFilter(audioIn); sweep_filtered_bpf[i] = static_cast<int16_t>(v);
               v = bef.ApplyFilter(audioIn); sweep_filtered_bef[i] = static_cast<int16_t>(v);
    }
    char strQf[5];
    sprintf(strQf, "_Q%2.1f", Qf);
    wave.Write((name + std::string(strQf) + std::string("_lpf.wav")).c_str(), sweep_filtered_lpf);
    wave.Write((name + std::string(strQf) + std::string("_hpf.wav")).c_str(), sweep_filtered_hpf);
    wave.Write((name + std::string(strQf) + std::string("_bpf.wav")).c_str(), sweep_filtered_bpf);
    wave.Write((name + std::string(strQf) + std::string("_bef.wav")).c_str(), sweep_filtered_bef);
}

class Melody
{
public:
    enum
    {
        C = 0,
        CD,
        D,
        DE,
        E,
        F,
        FG,
        G,
        GA,
        A,
        B,
        H
    };
    static double GetRate(int m)
    {
        return 220.0 * pow(2, (m - A) / 12.0);
    }
};

int main(void)
{
    if(0)
    {
        int freqs[] = { Melody::C, Melody::E, Melody::GA, 0 };
        for (uint i = 0; i < sizeof(freqs)/sizeof(freqs[0]) - 1; ++i)
        {
            int freq = static_cast<int>(Melody::GetRate(freqs[i]) + 0.5);
            printf("%i\n", freq);
            freqs[i] = freq;
        }
        generate_sample(freqs);
        return 0;
    }
    test_filtering();
    return 0;
}
