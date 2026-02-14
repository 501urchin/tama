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




void newEma() {
    std::vector<double> prices = make_random_doubles(100'000, 1.0, 100.0);



    ExponentialMovingAverage ema(50);
    std::vector<double> out;
    status s = ema.compute(prices, out);

    std::cout << out.back() << std::endl;
    double newVal = 15.45;
    prices.push_back(newVal);
    double lema = ema.update(newVal);
    std::cout << lema << std::endl;


    std::vector<double> o1;
    tama::ema(prices, o1, 50);
    std::cout << o1.back() << std::endl;

}


int main() {
    std::size_t n = 100'000;
    std::vector<double> prices = make_random_doubles(n, 1.0, 100.0);
    std::vector<double> volume = make_random_doubles(n, 100.0, 10'000.0);

    std::vector<double> out(prices.size());
    std::vector<double> out2(prices.size());

    const uint16_t period = 50;
    const int iterations = 10;



    newEma();

    benchmark("SMA", [&] {
        status res = sma(prices, out, period);
        if (res != status::ok) {
            std::cerr << "SMA failed with status " << static_cast<int>(res) << '\n';
        }
    }, iterations);

    benchmark("EMA", [&] {
        status res = ema(prices, out, period);
        if (res != status::ok) {
            std::cerr << "EMA failed with status " << static_cast<int>(res) << '\n';
        }
    }, iterations);

    benchmark("WMA", [&] {
        status res = wma(prices, out, period);
        if (res != status::ok) {
            std::cerr << "WMA failed with status " << static_cast<int>(res) << '\n';
        }
    }, iterations);

    benchmark("VWMA", [&] {
        status res = vwma(prices, volume, out, period);
        if (res != status::ok) {
            std::cerr << "VWMA failed with status " << static_cast<int>(res) << '\n';
        }
    }, iterations);

    benchmark("DEMA", [&] {
        status res = dema(prices, out, period);
        if (res != status::ok) {
            std::cerr << "DEMA failed with status " << static_cast<int>(res) << '\n';
        }
    }, iterations);

    benchmark("TEMA", [&] {
        status res = tema(prices, out, period);
        if (res != status::ok) {
            std::cerr << "TEMA failed with status " << static_cast<int>(res) << '\n';
        }
    }, iterations);

    benchmark("HMA", [&] {
        status res = hull(prices, out, period);
        if (res != status::ok) {
            std::cerr << "HMA failed with status " << static_cast<int>(res) << '\n';
        }
    }, iterations);

    benchmark("MD", [&] {
        status res = md(prices, out, period);
        if (res != status::ok) {
            std::cerr << "MD failed with status " << static_cast<int>(res) << '\n';
        }
    }, iterations);

    return 0;
}