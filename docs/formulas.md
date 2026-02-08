# Moving Averages in Trading

This document lists common moving averages used in trading, along with their mathematical formulas.

---

## 1. Simple Moving Average (SMA)

**Description:** Arithmetic mean of the last ( n ) prices. Smooth but slower to react.

**Formula:**

[
\text{SMA}*t = \frac{1}{n}\sum*{i=0}^{n-1} P_{t-i}
]

---

## 2. Exponential Moving Average (EMA)

**Description:** Weights recent prices more heavily, faster reaction than SMA.

**Formula:**

[
\text{EMA}*t = \alpha P_t + (1-\alpha)\text{EMA}*{t-1}, \quad \alpha = \frac{2}{n+1}
]

---

## 3. Weighted Moving Average (WMA)

**Description:** Linear weighting of prices; recent prices carry more weight.

**Formula:**

[
\text{WMA}*t = \frac{\sum*{i=0}^{n-1} (n-i),P_{t-i}}{\sum_{i=1}^{n} i}
]

---

## 4. Volume Weighted Moving Average (VWMA)

**Description:** Prices weighted by trading volume, emphasizes high-volume levels.

**Formula:**

[
\text{VWMA}*t = \frac{\sum*{i=0}^{n-1} P_{t-i}V_{t-i}}{\sum_{i=0}^{n-1} V_{t-i}}
]

---

## 5. Smoothed Moving Average (SMMA / RMA)

**Description:** EMA-like smoothing with very low noise and higher lag.

**Formula:**

[
\text{SMMA}*t = \frac{(n-1)\text{SMMA}*{t-1} + P_t}{n}
]

---

## 6. Double Exponential Moving Average (DEMA)

**Description:** Reduces lag using two EMAs.

**Formula:**

[
\text{DEMA}_t = 2\cdot\text{EMA}_t - \text{EMA}(\text{EMA}_t)
]

---

## 7. Triple Exponential Moving Average (TEMA)

**Description:** Further lag reduction using three EMAs.

**Formula:**

[
\text{TEMA}_t = 3\cdot\text{EMA}_t - 3\cdot\text{EMA}(\text{EMA}_t) + \text{EMA}(\text{EMA}(\text{EMA}_t))
]

---

## 8. Hull Moving Average (HMA)

**Description:** Uses WMAs and square-root length to minimize lag while staying smooth.

**Formula:**

[
\text{HMA}_t = \text{WMA}\Big(2\cdot\text{WMA}(P, n/2) - \text{WMA}(P, n), \sqrt{n}\Big)
]

---

## 9. Kaufman Adaptive Moving Average (KAMA)

**Description:** Adapts speed based on market efficiency; fast in trends, slow in ranges.

**Formula:**

[
\text{KAMA}*t = \text{KAMA}*{t-1} + SC_t \cdot (P_t - \text{KAMA}_{t-1})
]

[
SC_t = \big[ ER_t(\alpha_f-\alpha_s) + \alpha_s \big]^2
]

