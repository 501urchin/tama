### Kaufman Adaptive Moving Average (KAMA)

**Description:** Adapts speed based on market efficiency; fast in trends, slow in ranges.

$$
\text{KAMA}*t = \text{KAMA}*{t-1} + SC_t \cdot \big(P_t - \text{KAMA}_{t-1}\big)
$$

$$
SC_t = \Big[ ER_t \cdot (\alpha_f - \alpha_s) + \alpha_s \Big]^2
$$

---

### McGinley Dynamic (MD)

**Description:** Adaptive moving average that adjusts smoothing based on price speed to reduce lag.

$$
MD_t = MD_{t-1} + \frac{P_t - MD_{t-1}}{k \cdot \frac{P_t}{MD_{t-1}}}
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

---

### Fractal Adaptive Moving Average (FRAMA)

**Description:** EMA with dynamic smoothing based on fractal dimension; fast in trends, slow in noise.

$$
D = \frac{\log(L_1 + L_2) - \log(L_N)}{\log(2)}
$$

$$
L_1 = \frac{\text{max(H)} - \text{min(L)}}{N/2}, \quad
L_2 = \frac{\text{max(H)} - \text{min(L)}}{N/2}, \quad
L_N = \frac{\text{max(H)} - \text{min(L)}}{N}
$$

$$
\alpha_t = \exp(-4.6 \cdot (D - 1))
$$

$$
FRAMA_t = FRAMA_{t-1} + \alpha_t \cdot (P_t - FRAMA_{t-1})
$$
