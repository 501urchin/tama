#include <tama/helpers.hpp>

#if defined(__aarch64__) || defined(_M_ARM64)
#include <arm_neon.h>
#elif defined(__x86_64__) || defined(_M_X64)
#include <immintrin.h>
#endif


// TODO: apparently on x86_64 the tests fail. switch to non arm computer and eval why
namespace tama::helpers {
    float simdSumF32(std::span<const float> elms) {
        float sum = 0.0f;
        const size_t n = elms.size();

        if (n == 0) {
            return 0.0f;
        }

        size_t i = 0;
    
        #if defined(__aarch64__) || defined(_M_ARM64)
            if (n < 4) {
                for (; i < n; i++) {sum += elms[i];}
                return sum;
            }

            float32x4_t acc = vdupq_n_f32(0);
            for (; i + 4 <= n; i += 4) {
                acc = vaddq_f32(acc, vld1q_f32(&elms[i]));
            }
            sum += vaddvq_f32(acc);
        #elif defined(__x86_64__) || defined(_M_X64)
            if (n < 8) {
                for (; i < n; i++) {sum += elms[i];}
                return sum;
            }

            __m256 acc = _mm256_setzero_ps();
            for (; i + 8 <= n; i += 8) {
                acc = _mm256_add_ps(acc, _mm256_loadu_ps(&elms[i]));
            }
    
            __m128 lo = _mm256_castps256_ps128(acc);
            __m128 hi = _mm256_extractf128_ps(acc, 1);
            __m128 sum2 = _mm_add_ps(lo, hi);
    
            __m128 t1 = _mm_hadd_ps(sum2, sum2);
            __m128 t2 = _mm_hadd_ps(t1, t1);
    
            sum += _mm_cvtss_f32(t2);
        #endif
    
        for (; i < n; i++) {
            sum += elms[i];
        }
    
        return sum;
    }
    
    double simdSumF64(std::span<const double> elms) {
        double sum = 0.0;
        const size_t n = elms.size();

        if (n == 0) {
            return 0.0;
        }

        size_t i = 0;
    
        #if defined(__aarch64__) || defined(_M_ARM64)
            if (n < 2) {
                for (; i < n; i++) {sum += elms[i];}
                return sum;
            }
            
            float64x2_t acc = vdupq_n_f64(0);
            for (; i + 2 <= n; i += 2) {
                acc = vaddq_f64(acc, vld1q_f64(&elms[i]));
            }
            sum += vaddvq_f64(acc);
        #elif defined(__x86_64__) || defined(_M_X64)
            if (n < 4) {
                for (; i < n; i++) {sum += elms[i];}
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
}