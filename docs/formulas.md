### Kaufman Adaptive Moving Average (KAMA)

**Description:** Adapts speed based on market efficiency; fast in trends, slow in ranges.

$$
\text{KAMA}*t = \text{KAMA}*{t-1} + SC_t \cdot \big(P_t - \text{KAMA}_{t-1}\big)
$$

$$
SC_t = \Big[ ER_t \cdot (\alpha_f - \alpha_s) + \alpha_s \Big]^2
$$

---

### Generalized Double Exponential Moving Average (GD)

**Description:** Reduces EMA lag using a weighted second EMA; tuning weight controls aggressiveness.

$$
GD_t = (1 + w) \cdot EMA_t - w \cdot EMA(EMA_t)
$$

$$
EMA_t = EMA_{t-1} + \alpha \cdot (P_t - EMA_{t-1}), \quad
\alpha = \frac{2}{N+1}
$$
