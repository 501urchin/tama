# tama

tama is a small C++ library focused on moving average indicators commonly used in algorithmic trading. It provides straightforward, fast implementations for working with price series and can be used in backtests, live systems, or research workflows.

Included indicators:

- Simple Moving Average (SMA)
- Exponential Moving Average (EMA)
- Weighted Moving Average (WMA)
- Volume-Weighted Moving Average (VWMA)
- Double Exponential Moving Average (DEMA)
- Triple Exponential Moving Average (TEMA)
- Hull Moving Average (HMA)
- McGinley Dynamic (MD)

Design goals:

- Minimal, stateful API with explicit inputs and outputs
- Easy to integrate into existing C++ trading code

## TODO

- Implement KAMA.
- Implement generalized DEMA.
- Implement FRAMA.

## Installation

```cmake
include(FetchContent)


# Disable tama's own tests/example binary
set(TAMA_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(TAMA_RUN_BUILD OFF CACHE BOOL "" FORCE)

FetchContent_Declare(
	tama
	GIT_REPOSITORY https://github.com/501urchin/tama.git
	GIT_TAG        main
)

FetchContent_MakeAvailable(tama)

# Link against the library
target_link_libraries(your_target PRIVATE tama)
```

## Example usage

```cpp
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
	// Initialize via compute() 
	// or pass a previous EMA value to the constructor (e.g., ExponentialMovingAverage(3, 14.875)).
	double latestEma = ema.update(newTick);

	std::cout << "Latest EMA: " << latestEma << std::endl;
	return 0;
}
```
