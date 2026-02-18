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

tama::TripleExponentialMovingAverage::TripleExponentialMovingAverage(uint16_t period, std::vector<double> prevCalc)
	: period(static_cast<size_t>(require_period(period))),
	  ema1(period),
	  ema2(period),
	  ema3(period),
	  initialized(false),
	  lastTema(0.0) {
	if (!prevCalc.empty()) {
		if (prevCalc.size() != this->period) {
			throw std::invalid_argument("prevCalc buffer doesn't match period");
		}

		std::vector<double> emaAOut(prevCalc.size());
		std::vector<double> emaBOut(prevCalc.size());
		std::vector<double> emaCOut(prevCalc.size());

		status res = this->ema1.compute(prevCalc, emaAOut);
		if (res != status::ok) {
			return;
		}

		res = this->ema2.compute(emaAOut, emaBOut);
		if (res != status::ok) {
			return;
		}

		res = this->ema3.compute(emaBOut, emaCOut);
		if (res != status::ok) {
			return;
		}

		this->lastTema = 3.0 * emaAOut.back() - 3.0 * emaBOut.back() + emaCOut.back();
		this->initialized = true;
	}
}

status tama::TripleExponentialMovingAverage::compute(std::span<const double> prices, std::vector<double>& output) {
	if (prices.empty()) {
		return status::emptyParams;
	}

	const size_t pricesLen = prices.size();
	if (output.size() < pricesLen) {
		output.resize(pricesLen);
	}

	std::vector<double> emaAOut(pricesLen);
	std::vector<double> emaBOut(pricesLen);
	std::vector<double> emaCOut(pricesLen);

	status res = this->ema1.compute(prices, emaAOut);
	if (res != status::ok) {
		return res;
	}

	res = this->ema2.compute(emaAOut, emaBOut);
	if (res != status::ok) {
		return res;
	}

	res = this->ema3.compute(emaBOut, emaCOut);
	if (res != status::ok) {
		return res;
	}

	// simd
	for (size_t i = 0; i < pricesLen; i++) {
		output[i] = 3.0 * emaAOut[i] - 3.0 * emaBOut[i] + emaCOut[i];
	}

	this->lastTema = output.back();
	this->initialized = true;

	return status::ok;
}

double tama::TripleExponentialMovingAverage::update(double price) {
	if (!this->initialized) {
		return 0.0;
	}

	const double ema1Value = this->ema1.update(price);
	const double ema2Value = this->ema2.update(ema1Value);
	const double ema3Value = this->ema3.update(ema2Value);

	const double tema = 3.0 * ema1Value - 3.0 * ema2Value + ema3Value;
	this->lastTema = tema;
	return tema;
}

double tama::TripleExponentialMovingAverage::latest() {
	return this->lastTema;
}
