#include <tama/tama.hpp>
#include <vector>
#include <random>
#include <iostream>
#include <chrono>
#include <utility>
#include <cstdio>
#include <cmath>

using namespace tama;

std::vector<double> make_random_doubles(std::size_t n, double min = 0.0, double max = 1.0) {
    std::mt19937_64 gen(42);
    std::uniform_real_distribution<double> dist(min, max);

    std::vector<double> v;
    v.reserve(n);
    for (std::size_t i = 0; i < n; ++i) {
        v.push_back(dist(gen));
    }
    return v;
}

template <typename F>
long long measure_ns(F&& f) {
    auto start = std::chrono::high_resolution_clock::now();
    std::forward<F>(f)();
    auto end = std::chrono::high_resolution_clock::now();
    return std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
}

template <typename F>
void benchmark(const char* name, F&& f, int iterations = 10) {
    long long total_ns = 0;
    for (int i = 0; i < iterations; ++i) {
        total_ns += measure_ns(f);
    }
    double avg_ms = static_cast<double>(total_ns) / static_cast<double>(iterations) / 1'000'000.0;
    std::printf("%-5s -> %5.3f ms/op\n", name, avg_ms);
}

void benchmark_stateful_wma() {
    constexpr std::size_t initialCount = 100'000;
    constexpr std::size_t updateCount = 100'000;
    constexpr uint16_t period = 20;

    std::vector<double> initialPrices = make_random_doubles(initialCount, 1.0, 100.0);
    std::vector<double> updatePrices = make_random_doubles(updateCount, 1.0, 100.0);

    std::vector<double> out;
    WeightedMovingAverage w(period);

    long long computeNs = measure_ns([&]() {
        w.compute(initialPrices, out);
    });

    long long updatesNs = measure_ns([&]() {
        for (double price : updatePrices) {
            w.update(price);
        }
    });

    std::printf("\nStateful WMA timing\n");
    std::printf("compute (100k): %7.3f ms\n", static_cast<double>(computeNs) / 1'000'000.0);
    std::printf("update (100k): %7.3f ms\n", static_cast<double>(updatesNs) / 1'000'000.0);
    std::printf("avg update time: %.3f ns/update\n", static_cast<double>(updatesNs) / static_cast<double>(updateCount));
}

void benchmark_stateful_ema() {
    constexpr std::size_t initialCount = 100'000;
    constexpr std::size_t updateCount = 100'000;
    constexpr uint16_t period = 20;

    std::vector<double> initialPrices = make_random_doubles(initialCount, 1.0, 100.0);
    std::vector<double> updatePrices = make_random_doubles(updateCount, 1.0, 100.0);

    std::vector<double> out;
    ExponentialMovingAverage ema(period);

    long long computeNs = measure_ns([&]() {
        ema.compute(initialPrices, out);
    });

    long long updatesNs = measure_ns([&]() {
        for (double price : updatePrices) {
            ema.update(price);
        }
    });

    std::printf("\nStateful EMA timing\n");
    std::printf("compute (100k): %7.3f ms\n", static_cast<double>(computeNs) / 1'000'000.0);
    std::printf("update (100k): %7.3f ms\n", static_cast<double>(updatesNs) / 1'000'000.0);
    std::printf("avg update time: %.3f ns/update\n", static_cast<double>(updatesNs) / static_cast<double>(updateCount));
}

void benchmark_stateful_sma() {
    constexpr std::size_t initialCount = 100'000;
    constexpr std::size_t updateCount = 100'000;
    constexpr uint16_t period = 20;

    std::vector<double> initialPrices = make_random_doubles(initialCount, 1.0, 100.0);
    std::vector<double> updatePrices = make_random_doubles(updateCount, 1.0, 100.0);

    std::vector<double> out;
    SimpleMovingAverage sma(period);

    long long computeNs = measure_ns([&]() {
        sma.compute(initialPrices, out);
    });

    long long updatesNs = measure_ns([&]() {
        for (double price : updatePrices) {
            sma.update(price);
        }
    });

    std::printf("\nStateful SMA timing\n");
    std::printf("compute (100k): %7.3f ms\n", static_cast<double>(computeNs) / 1'000'000.0);
    std::printf("update (100k): %7.3f ms\n", static_cast<double>(updatesNs) / 1'000'000.0);
    std::printf("avg update time: %.3f ns/update\n", static_cast<double>(updatesNs) / static_cast<double>(updateCount));
}






int main() {
    std::vector<double> debugPrices{10, 12, 11, 13, 12, 14, 15, 13, 14, 16};
    constexpr uint16_t hullPeriod = 3;
    constexpr double newDebugPrice = 17.25;

    std::vector<double> statefulOut;
    HullMovingAverage statefulHull(hullPeriod);
    statefulHull.compute(debugPrices, statefulOut);

    debugPrices.push_back(newDebugPrice);
    const double updatedHull = statefulHull.update(newDebugPrice);

    std::vector<double> baselineOut;
    const auto baselineStatus = hull(debugPrices, baselineOut, hullPeriod);

    std::cout << "\nHMA debug verification\n";
    std::cout << "stateful update value: " << updatedHull << "\n";
    if (baselineStatus == status::ok && !baselineOut.empty()) {
        const double baselineLatest = baselineOut.back();
        std::cout << "baseline latest value: " << baselineLatest << "\n";
        std::cout << "absolute delta: " << std::abs(updatedHull - baselineLatest) << "\n";
    } else {
        std::cout << "baseline hull() failed with status: " << static_cast<int>(baselineStatus) << "\n";
    }



    benchmark_stateful_wma();
    benchmark_stateful_ema();
    benchmark_stateful_sma();

    return 0;
}