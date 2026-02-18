#include <vector>
#include <stdexcept>
#include <tama/tama.hpp>

namespace {
	uint16_t require_period(uint16_t period) {
		if (period == 0) {
			throw std::invalid_argument("invalid period");
		}
		return period;
	}
}

tama::DoubleExponentialMovingAverage::DoubleExponentialMovingAverage(uint16_t period, std::vector<double> prevCalc)
	: period(static_cast<size_t>(require_period(period))),
	  ema1(period),
	  ema2(period),
	  initialized(false),
	  lastDema(0.0) {
	if (!prevCalc.empty()) {
		if (prevCalc.size() != this->period) {
			throw std::invalid_argument("prevCalc buffer doesn't match period");
		}

		std::vector<double> emaAOut(prevCalc.size());
		std::vector<double> emaBOut(prevCalc.size());

		status res = this->ema1.compute(prevCalc, emaAOut);
		if (res != status::ok) {
			return;
		}

		res = this->ema2.compute(emaAOut, emaBOut);
		if (res != status::ok) {
			return;
		}

		this->lastDema = 2.0 * emaAOut.back() - emaBOut.back();
		this->initialized = true;
	}
}

status tama::DoubleExponentialMovingAverage::compute(std::span<const double> prices, std::vector<double>& output) {
	if (prices.empty()) {
		return status::emptyParams;
	}

	const size_t pricesLen = prices.size();
	if (output.size() < pricesLen) {
		output.resize(pricesLen);
	}

	std::vector<double> emaAOut(pricesLen);
	std::vector<double> emaBOut(pricesLen);

	status res = this->ema1.compute(prices, emaAOut);
	if (res != status::ok) {
		return res;
	}

	res = this->ema2.compute(emaAOut, emaBOut);
	if (res != status::ok) {
		return res;
	}

	// simd
	for (size_t i = 0; i < pricesLen; i++) {
		output[i] = 2.0 * emaAOut[i] - emaBOut[i];
	}

	this->lastDema = output.back();
	this->initialized = true;

	return status::ok;
}

double tama::DoubleExponentialMovingAverage::update(double price) {
	if (!this->initialized) {
		throw std::runtime_error("dema not initialized");
	}

	const double ema1Value = this->ema1.update(price);
	const double ema2Value = this->ema2.update(ema1Value);

	const double dema = 2.0 * ema1Value - ema2Value;
	this->lastDema = dema;
	return dema;
}

double tama::DoubleExponentialMovingAverage::latest() {
	return this->lastDema;
}
