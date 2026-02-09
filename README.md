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
- Efficient computation on full price vectors
- Easy to integrate into existing C++ trading code
