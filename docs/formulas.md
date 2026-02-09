### Hull Moving Average (HMA)

**Description:** Uses WMAs and square-root length to minimize lag while staying smooth.

$$
\text{HMA}_t = \text{WMA}\Big(
2 \cdot \text{WMA}\big(P, n/2\big) - \text{WMA}(P, n),
\sqrt{n}
\Big)
$$


### Kaufman Adaptive Moving Average (KAMA)

**Description:** Adapts speed based on market efficiency; fast in trends, slow in ranges.

$$
\text{KAMA}*t = \text{KAMA}*{t-1} + SC_t \cdot \big(P_t - \text{KAMA}_{t-1}\big)
$$

$$
SC_t = \Big[ ER_t \cdot (\alpha_f - \alpha_s) + \alpha_s \Big]^2
$$
