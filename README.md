# Binance European Options Ticker Parser (C++ Assignment)

## Overview

This project implements a custom C++ parser for instrument statistics retrieved from Binance's European Options API. It focuses on parsing data from the `/eapi/v1/ticker` endpoint and is designed to be efficient, low-latency, and fully self-contained with **no external JSON libraries**.

The parser:
- Uses a statically embedded JSON string (example response from Binance).
- Extracts all fields of each instrument.
- Measures the parsing speed for a **single entry**.
- Operates with `O(N)` time complexity where `N` is the number of entries.

---

## Parsing Strategy

The JSON returned by Binance is a flat array of instrument objects with the same structure. Instead of using a full JSON parser, we implemented:

### ✅ Manual JSON Scanner
- Each object is scanned and parsed between `{}`.
- Fields are parsed **in order**, based on Binance’s known response structure.
- Values are extracted using lightweight pointer arithmetic and `std::from_chars` (for speed).
- Strings are extracted as `std::string_view` to avoid memory copies.

This approach offers **low memory overhead**, high performance, and remains robust as long as field order is consistent.

---

## Complexity & Performance

- **Per-entry complexity**: `O(1)` – each object is parsed linearly once, with fixed field logic.
- **Full array complexity**: `O(N)` – total cost grows linearly with number of entries.
- **Memory allocation**: Zero-copy parsing using `string_view`, heap allocations only from vector growth.

Time to parse one entry is measured using `std::chrono::high_resolution_clock` and reported in microseconds.

---

## Optimizations Considered

We intentionally avoided:
- Searching for each key repeatedly (`find`, `strstr`) to reduce parsing overhead.
- Using external JSON libraries like `nlohmann::json` or `rapidjson` to demonstrate manual control.
- Fixed-offset parsing, as values vary in length and cannot be reliably extracted by absolute byte positions.

Alternative optimizations (not implemented here) may include:
- Field position offset caching.
- SIMD or binary parsing for ultra-low latency applications.

---

## How to Build

### Release Mode (optimized):
```bash
g++ -O3 -DNDEBUG -std=c++17 -o parse_tickers main.cpp
