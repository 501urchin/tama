#include <tama/tama.hpp>
#include <vector>
#include <random>
#include <print>
#include <iostream>
#include <chrono>
#include <utility>


std::vector<double> make_random_vec(std::size_t n = 10'000) {
    std::mt19937_64 rng{std::random_device{}()};
    std::uniform_real_distribution<double> dist(0.0, 1.0);

    std::vector<double> v;
    v.reserve(n);
    for (std::size_t i = 0; i < n; ++i)
        v.push_back(dist(rng));

    return v;
}


template<typename F>
auto time_ns(F&& f) {
    auto start = std::chrono::steady_clock::now();
    std::forward<F>(f)();
    auto end = std::chrono::steady_clock::now();
    return std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
}

int main() {
    auto prices = make_random_vec(1000);
    std::vector<double> emaOut(prices.size());

    const int64_t iterations = 100;
    int64_t totalTime = 0;
    

    for (int64_t i = 0; i < iterations; i++) {
        auto elapsed_ns = time_ns([&] {
            tama::status res = tama::ema(prices, emaOut, 100);
            if (res != tama::status::ok) {
                std::println("tama threw an error");
                std::exit(1);
            }
        });
        totalTime += elapsed_ns;
    }

    std::printf("Latest EMA: %f\n", emaOut.back());
    std::printf("%lld iterations: %lld ns/op\n", iterations,  (totalTime / iterations));
}

// int main() {
//     std::vector<double> prices{11,12,14,18,12,15,13,16,10};
//     std::vector<double> wmaOut(prices.size());
//     tama::status res = tama::wma(prices, wmaOut, 3);

//     for (double v : wmaOut) {
//         std::cout << v << " ";
//     }

//     std::cout<<std::endl;
    
// }