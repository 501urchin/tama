#include <tama/tama.hpp>
#include <vector>
#include <random>
#include <print>
#include <iostream>
#include <chrono>
#include <utility>

std::string status_to_string(tama::status s) {
    switch(s) {
        case tama::status::ok: return "ok";
        case tama::status::invalidParam: return "invalid_input";
        case tama::status::emptyParams: return "empty_params";
        default: return "unknown";
    }
}

int main() {
    std::vector<double> prices{10,12,11,13,12,14,15,13,14,16};
    std::vector<double> demaOut(prices.size());
    tama::status res = tama::tema(prices, demaOut, 3);
    if (res != tama::status::ok) {
        std::cout << status_to_string(res) << std::endl;
        return 1; 
    }

    std::cout<< "[ " ;
    for (double v : demaOut) {
        std::cout << v  << " ";
    }
    std::cout<< "]" <<std::endl;
}