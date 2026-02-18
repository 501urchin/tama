#include <tama/tama.hpp>
#include <vector>
#include <iostream>

int main() {
	std::vector<double> prices{10, 12, 11, 13, 12, 14, 15, 13, 14, 16};
	std::vector<double> emaOut;

	tama::ExponentialMovingAverage ema(3);
    
    status res = ema.compute(prices, emaOut);
	if (res != status::ok) {
		std::cerr << "tama::ema failed" << std::endl;
		return 1;
	}

	double newTick = 17;

	// update() consumes one new price and advances the EMA without reprocessing the full series.
	// Initialize via compute() or pass a previous EMA value to the constructor (e.g., ExponentialMovingAverage(3, 14.875)).
	double latestEma = ema.update(newTick);

	std::cout << "Latest EMA: " << latestEma << std::endl;
	return 0;
}