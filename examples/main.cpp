#include <tama/tama.hpp>
#include <vector>
#include <random>
#include <print>
#include <iostream>
#include <chrono>
#include <utility>
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


template <typename F, typename... Args>
long long measure_ns(F&& f, Args&&... args) {
    auto start = std::chrono::high_resolution_clock::now();
    std::forward<F>(f)(std::forward<Args>(args)...);
    auto end = std::chrono::high_resolution_clock::now();
    return std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
}

int main() {
    std::size_t n = 1'000'00;
    std::vector<double> prices = make_random_doubles(n, 1.0, 100.0);
    std::vector<double> out(prices.size());

    long long ns = measure_ns([&] {
        status res = sma(prices, out, 100);
        if (res != status::ok) {
            std::cerr << "func failed with status " << static_cast<int>(res) << '\n';
        }
    });

    std::cout << "execution time: " << ns << " ns" << std::endl;
}