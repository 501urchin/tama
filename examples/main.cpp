#include <tama/tama.hpp>
#include <vector>
#include <random>
#include <iostream>
#include <chrono>
#include <utility>
#include <cstdio>

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





int main() {
    std::vector<double> prices = make_random_doubles(100'000, 1.0, 100.0);


    SimpleMovingAverage sma(50);
    std::vector<double> out;
    status s = sma.compute(prices, out);

    std::cout << out.back() << std::endl;
    double newVal = 15.45;
    prices.push_back(newVal);
    double lema = sma.update(newVal);
    std::cout << lema << std::endl;


    std::vector<double> o1;
    SimpleMovingAverage sma2(50);
    sma2.compute(prices, o1);
    std::cout << o1.back() << std::endl;

    return 0;
}