#include <vector>
#include <tama/tama.hpp>
#if defined(__aarch64__) || defined(_M_ARM64)
#include <arm_neon.h>
#elif defined(__x86_64__) || defined(_M_X64)
#include <immintrin.h>
#endif


using namespace std;

tama::WeightedMovingAverage::WeightedMovingAverage(uint16_t period, std::vector<double> prevCalc) {
    this->denominator = static_cast<double>(period) * static_cast<double>((period + 1)) / 2;
    this->period = period;

    if (this->period > 0 && prevCalc.size() >= this->period) {
        const size_t offset = prevCalc.size() - this->period;
        this->priceBuf.assign(prevCalc.begin() + static_cast<std::ptrdiff_t>(offset), prevCalc.end());

        double weightedSum = 0.0;
        for (size_t i = 0; i < this->period; i++) {
            weightedSum += this->priceBuf[i] * (i + 1);
        }

        this->lastWma = weightedSum / this->denominator;
        this->initalized = true;
    }
}

double tama::WeightedMovingAverage::latest() {
    return this->lastWma;
}

status tama::WeightedMovingAverage::compute(std::span<const double> prices, std::vector<double>& output) {
    size_t pricesLen = prices.size();

    if (pricesLen == 0) {
        return status::emptyParams;
    }

    if (this->period == 0 || this->period > pricesLen) {
        return status::invalidParam;
    }

    if (output.size() < pricesLen) {
        output.resize(pricesLen);
    }

    double sSum = 0;
    double weightedSum = 0;


    for (size_t i = 0; i < this->period; i++) {
        sSum += prices[i];
        weightedSum += prices[i] * (i+1);
    }
    output[this->period-1] = weightedSum / this->denominator;

    for (size_t t = this->period; t < pricesLen; t++) {
        weightedSum -= sSum;
        sSum -= prices[t-this->period];
        sSum += prices[t];
        weightedSum += prices[t] * this->period;

        output[t] = weightedSum / this->denominator;
    }

    const size_t offset = pricesLen - this->period;
    this->priceBuf.assign(prices.begin() + static_cast<std::ptrdiff_t>(offset), prices.end());
    this->lastWma = output.back();
    this->initalized = true;

    return status::ok;
}

double tama::WeightedMovingAverage::update(double price) {
    if (!this->initalized || this->priceBuf.size() < this->period) {
        return 0;
    }

    this->priceBuf.erase(this->priceBuf.begin());
    this->priceBuf.push_back(price);


    double weightedSum = 0.0;
    size_t i = 0;

    #if defined(__aarch64__) || defined(_M_ARM64)
        double tmp[2];
        float64x2_t acc = vdupq_n_f64(0);
        
        for (; i+2 <= this->period; i += 2) {
            tmp[0] = i+1;
            tmp[1] = i+2;

            acc = vaddq_f64(acc, vmulq_f64(vld1q_f64(&this->priceBuf[i]), vld1q_f64(tmp)));
        }

        weightedSum += vaddvq_f64(acc);
    #elif defined(__x86_64__) || defined(_M_X64)
        double tmp[4];
        __m256d acc = _mm256_setzero_pd();
        
        for (; i+4 <= this->period; i += 4) {
            tmp[0] = i+1;
            tmp[1] = i+2;
            tmp[2] = i+3;
            tmp[3] = i+4;

            acc = _mm256_add_pd(acc, _mm256_mul_pd(_mm256_loadu_pd(&this->priceBuf[i]), _mm256_loadu_pd(tmp)));
        }

        __m128d lo = _mm256_castpd256_pd128(acc);
        __m128d hi = _mm256_extractf128_pd(acc, 1);
        __m128d sum2 = _mm_add_pd(lo, hi);
        weightedSum += _mm_cvtsd_f64(_mm_hadd_pd(sum2, sum2));
    #endif

    for (; i < this->period; i++) {
       weightedSum += this->priceBuf[i] * (i + 1);
    }

    this->lastWma = weightedSum / this->denominator;


    return this->lastWma;
}