#include <tama/helpers.hpp>

#if defined(__aarch64__) || defined(_M_ARM64)
#include <arm_neon.h>
#elif defined(__x86_64__) || defined(_M_X64)
#include <immintrin.h>
#endif

double helpers::simdSum(std::span<const double> elms) {
    double sum = 0.0;
    const size_t n = elms.size();

    if (n == 0) {
        return 0.0;
    }

    size_t i = 0;

#if defined(__aarch64__) || defined(_M_ARM64)
    if (n < 2) {
        for (; i < n; i++) {
            sum += elms[i];
        }
        return sum;
    }

    float64x2_t acc = vdupq_n_f64(0);
    for (; i + 2 <= n; i += 2) {
        acc = vaddq_f64(acc, vld1q_f64(&elms[i]));
    }
    sum += vaddvq_f64(acc);
#elif defined(__x86_64__) || defined(_M_X64)
    if (n < 4) {
        for (; i < n; i++) {
            sum += elms[i];
        }
        return sum;
    }

    __m256d acc = _mm256_setzero_pd();
    for (; i + 4 <= n; i += 4) {
        acc = _mm256_add_pd(acc, _mm256_loadu_pd(&elms[i]));
    }

    __m128d lo = _mm256_castpd256_pd128(acc);
    __m128d hi = _mm256_extractf128_pd(acc, 1);
    __m128d sum2 = _mm_add_pd(lo, hi);

    sum += _mm_cvtsd_f64(_mm_hadd_pd(sum2, sum2));
#endif

    for (; i < n; i++) {
        sum += elms[i];
    }

    return sum;
}
