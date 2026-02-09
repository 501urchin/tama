# tama

tama is a small C++ library focused on moving average indicators commonly used in algorithmic trading. It provides straightforward, fast implementations for working with price series and can be used in backtests, live systems, or research workflows.

Included indicators:

- Simple Moving Average (SMA)
- Exponential Moving Average (EMA)
- Weighted Moving Average (WMA)
- Volume-Weighted Moving Average (VWMA)
- Double Exponential Moving Average (DEMA)
- Triple Exponential Moving Average (TEMA)

Design goals:

- Minimal API with explicit inputs and outputs
- Easy to integrate into existing C++ trading code

## TODO

- Add a way to reuse previous calculation. A idea could be to add a optionnal 'prevCalc' param to each func and if its not empty use it
- Implement hma.
- Implement kama.
- Implement mcginley dynamic.
- Implement gd.

## Example: EMA usage

```cpp
#include <tama/tama.hpp>
#include <vector>
#include <iostream>

int main() {
	std::vector<double> prices{10, 12, 11, 13, 12, 14, 15, 13, 14, 16};
	std::vector<double> emaOut(prices.size());

	tama::status res = tama::ema(prices, emaOut, 3);
	if (res != tama::status::ok) {
		std::cerr << "tama::ema failed" << std::endl;
		return 1;
	}

	std::cout << "Latest EMA: " << emaOut.back() << std::endl;
	return 0;
}
```
