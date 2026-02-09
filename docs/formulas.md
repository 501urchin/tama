# Moving Averages in Trading

This document lists common moving averages used in trading, along with their mathematical formulas.

## 1. Volume Weighted Moving Average (VWMA)

**Description:** Prices weighted by trading volume, emphasizes high-volume levels.

**Formula:**

$$
\operatorname{VWMA}_t =
\frac{
  \displaystyle\sum_{i=0}^{n-1} P_{t-i} V_{t-i}
}{
  \displaystyle\sum_{i=0}^{n-1} V_{t-i}
}
$$

## 2. Double Exponential Moving Average (DEMA)

**Description:** Reduces lag using two EMAs.

**Formula:**

$$
\text{DEMA}_t = 2\cdot\text{EMA}_t - \text{EMA}(\text{EMA}_t)
$$

---

## 2. Triple Exponential Moving Average (TEMA)

**Description:** Further lag reduction using three EMAs.

**Formula:**

$$
\text{TEMA}_t =
3\cdot\text{EMA}_t

* 3\cdot\text{EMA}(\text{EMA}_t)

- \text{EMA}(\text{EMA}(\text{EMA}_t))
$$

---

## 3. Hull Moving Average (HMA)

**Description:** Uses WMAs and square-root length to minimize lag while staying smooth.

**Formula:**

$$
\text{HMA}_t =
\text{WMA}\Big(
2\cdot\text{WMA}(P, n/2) - \text{WMA}(P, n),
\sqrt{n}
\Big)
$$

---

## 4. Kaufman Adaptive Moving Average (KAMA)

**Description:** Adapts speed based on market efficiency; fast in trends, slow in ranges.

**Formula:**

$$
\text{KAMA}*t =
\text{KAMA}*{t-1}

* SC_t \cdot (P_t - \text{KAMA}_{t-1})
$$

$$
SC_t =
\big[ ER_t(\alpha_f - \alpha_s) + \alpha_s \big]^2
$$
