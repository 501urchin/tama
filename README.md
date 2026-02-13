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

- Minimal API with explicit inputs and outputs
- Easy to integrate into existing C++ trading code

## TODO

- Add a way to reuse previous calculation. A idea could be to add a optionnal 'prevCalc' param to each func and if its not empty use it
- Implement kama.
- Implement Generalized dema.
- Implement frama.

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
	std::vector<double> emaOut(prices.size());

	// Ensure the output vector is at least as large as the input vector.
	// If it is smaller, tama::ema will attempt to resize it internally.
	tama::status res = tama::ema(prices, emaOut, 3);
	if (res != tama::status::ok) {
		std::cerr << "tama::ema failed" << std::endl;
		return 1;
	}

	std::cout << "Latest EMA: " << emaOut.back() << std::endl;
	return 0;
}
```
