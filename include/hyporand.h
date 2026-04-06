//
//  hyporand.h
//  HypoModOSX
//
//  Created by Duncan MacGregor on 06/04/2026.
//


#ifndef HYPORAND_H
#define HYPORAND_H

#include <cstdint>
#include <cmath>

class HypoRand
{
public:
    HypoRand() = default;

    HypoRand(uint64_t base_seed, uint64_t stream_id = 0)
    {
        seed(base_seed, stream_id);
    }

    void seed(uint64_t base_seed, uint64_t stream_id = 0)
    {
        uint64_t x = mix_seed(base_seed, stream_id);
        s[0] = splitmix64_next(x);
        s[1] = splitmix64_next(x);
        s[2] = splitmix64_next(x);
        s[3] = splitmix64_next(x);

        if ((s[0] | s[1] | s[2] | s[3]) == 0) {
            s[0] = 0x9e3779b97f4a7c15ULL;
        }

        has_spare = false;
        spare = 0.0;
    }

    uint64_t u64()
    {
        return next();
    }

    uint32_t u32()
    {
        return static_cast<uint32_t>(next() >> 32);
    }

    double uniform01()
    {
        return (next() >> 11) * 0x1.0p-53;
    }

    float uniform01f()
    {
        return (u32() >> 8) * 0x1.0p-24f;
    }

    double uniform_open01()
    {
        return ((next() >> 11) + 0.5) * 0x1.0p-53;
    }

    double uniform(double a, double b)
    {
        return a + (b - a) * uniform01();
    }

    double normal(double mean = 0.0, double sd = 1.0)
    {
        if (has_spare) {
            has_spare = false;
            return mean + sd * spare;
        }

        const double u1 = uniform_open01();
        const double u2 = uniform_open01();
        const double r = std::sqrt(-2.0 * std::log(u1));
        const double theta = 6.283185307179586476925286766559 * u2;

        spare = r * std::sin(theta);
        has_spare = true;
        return mean + sd * (r * std::cos(theta));
    }

    double lognormal_from_mean_sd(double mean, double sd)
    {
        if (mean <= 0.0) return 0.0;
        if (sd <= 0.0) return mean;

        const double mean2 = mean * mean;
        const double sd2 = sd * sd;
        const double lognMean = std::log(mean2 / std::sqrt(mean2 + sd2));
        const double lognSD = std::sqrt(std::log((mean2 + sd2) / mean2));

        return std::exp(lognMean + lognSD * normal());
    }
    
    uint32_t bounded_u32(uint32_t bound)
    {
        if (bound == 0) return 0;

        const uint64_t threshold = (uint64_t(0) - uint64_t(bound)) % uint64_t(bound);

        for (;;) {
            uint32_t r = u32();
            uint64_t m = uint64_t(r) * uint64_t(bound);
            uint32_t l = static_cast<uint32_t>(m);

            if (l >= threshold) {
                return static_cast<uint32_t>(m >> 32);
            }
        }
    }
    
    int randint(int a, int b)
    {
        if (b <= a) return a;
        return a + static_cast<int>(bounded_u32(static_cast<uint32_t>(b - a + 1)));
    }
    

private:
    uint64_t s[4]{};
    bool has_spare = false;
    double spare = 0.0;

    static inline uint64_t rotl(uint64_t x, int k)
    {
        return (x << k) | (x >> (64 - k));
    }

    uint64_t next()
    {
        const uint64_t result = s[0] + s[3];
        const uint64_t t = s[1] << 17;

        s[2] ^= s[0];
        s[3] ^= s[1];
        s[1] ^= s[2];
        s[0] ^= s[3];

        s[2] ^= t;
        s[3] = rotl(s[3], 45);

        return result;
    }

    static uint64_t splitmix64_next(uint64_t& x)
    {
        uint64_t z = (x += 0x9e3779b97f4a7c15ULL);
        z = (z ^ (z >> 30)) * 0xbf58476d1ce4e5b9ULL;
        z = (z ^ (z >> 27)) * 0x94d049bb133111ebULL;
        return z ^ (z >> 31);
    }

    static uint64_t mix_seed(uint64_t base_seed, uint64_t stream_id)
    {
        uint64_t x = base_seed ^ (0x9e3779b97f4a7c15ULL * (stream_id + 1));
        x ^= (x >> 30);
        x *= 0xbf58476d1ce4e5b9ULL;
        x ^= (x >> 27);
        x *= 0x94d049bb133111ebULL;
        x ^= (x >> 31);
        return x;
    }
};

#endif
