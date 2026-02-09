#include <tama/tama.hpp>
#include <vector>
#include <random>
#include <print>
#include <iostream>
#include <chrono>
#include <utility>
#include <cmath>

using namespace tama;


int main() {
    std::vector<double> prices{10,12,11,13,12,14,15,13,14,16};
    std::vector<double> out(prices.size());

    status res = hull(prices, out, 3);
    if (res != status::ok) {
        return 1; 
    }

    std::cout<< "[ " ;
    for (double v : out) {
        std::cout << v  << " ";
    }
    std::cout<< "]" <<std::endl;
}