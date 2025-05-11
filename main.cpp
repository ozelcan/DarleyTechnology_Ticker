#include <charconv>
#include <chrono>
#include <cstring>
#include <iostream>
#include <string>
#include <string_view>
#include <vector>

struct TickerEntry {
  std::string_view symbol;
  double priceChange;
  double priceChangePercent;
  double lastPrice;
  double lastQty;
  double open;
  double high;
  double low;
  double volume;
  double amount;
  double bidPrice;
  double askPrice;
  long long openTime;
  long long closeTime;
  long long firstTradeId;
  int tradeCount;
  double strikePrice;
  double exercisePrice;
};

// Advance pointer to next quote-enclosed string and return view
std::string_view extractQuotedStringView(const char *&ptr) {
  ptr = std::strchr(ptr, '"') + 1;
  const char *end = std::strchr(ptr, '"');
  std::string_view view(ptr, end - ptr);
  ptr = std::strchr(end, ',') + 1;
  return view;
}

// Parse double using from_chars for speed
double extractNumber(const char *&ptr) {
  while (*ptr != '-' && (*ptr < '0' || *ptr > '9'))
    ++ptr;
  const char *start = ptr;
  while (*ptr != ',' && *ptr != '}')
    ++ptr;
  double val = 0;
  std::from_chars(start, ptr, val);
  if (*ptr == ',')
    ++ptr;
  return val;
}

long long extractLong(const char *&ptr) {
  while (*ptr != '-' && (*ptr < '0' || *ptr > '9'))
    ++ptr;
  const char *start = ptr;
  while (*ptr != ',' && *ptr != '}')
    ++ptr;
  long long val = 0;
  std::from_chars(start, ptr, val);
  if (*ptr == ',')
    ++ptr;
  return val;
}

int extractInt(const char *&ptr) { return static_cast<int>(extractLong(ptr)); }

TickerEntry fastParseEntry(const char *&ptr) {
  ptr = std::strchr(ptr, '{') + 1;
  TickerEntry t;
  t.symbol = extractQuotedStringView(ptr);
  t.priceChange = extractNumber(ptr);
  t.priceChangePercent = extractNumber(ptr);
  t.lastPrice = extractNumber(ptr);
  t.lastQty = extractNumber(ptr);
  t.open = extractNumber(ptr);
  t.high = extractNumber(ptr);
  t.low = extractNumber(ptr);
  t.volume = extractNumber(ptr);
  t.amount = extractNumber(ptr);
  t.bidPrice = extractNumber(ptr);
  t.askPrice = extractNumber(ptr);
  t.openTime = extractLong(ptr);
  t.closeTime = extractLong(ptr);
  t.firstTradeId = extractLong(ptr);
  t.tradeCount = extractInt(ptr);
  t.strikePrice = extractNumber(ptr);
  t.exercisePrice = extractNumber(ptr);
  ptr = std::strchr(ptr, '}') + 1;
  return t;
}

std::vector<TickerEntry> parseAllEntries(const std::string &json) {
  std::vector<TickerEntry> results;
  const char *ptr = json.c_str();
  while ((ptr = std::strchr(ptr, '{'))) {
    results.push_back(fastParseEntry(ptr));
  }
  return results;
}

void printTicker(const TickerEntry &t) {
  std::cout << "Symbol: " << t.symbol << ", Last: " << t.lastPrice
            << ", Bid: " << t.bidPrice << ", Ask: " << t.askPrice
            << ", Strike: " << t.strikePrice
            << ", Trade Count: " << t.tradeCount << "\n";
}

const std::string staticJson = R"([
    {
        "symbol": "BTC-251226-120000-P",
        "priceChange": "0",
        "priceChangePercent": "0",
        "lastPrice": "0",
        "lastQty": "0",
        "open": "0",
        "high": "0",
        "low": "0",
        "volume": "0",
        "amount": "0",
        "bidPrice": "21415",
        "askPrice": "0",
        "openTime": 0,
        "closeTime": 0,
        "firstTradeId": 0,
        "tradeCount": 0,
        "strikePrice": "120000",
        "exercisePrice": "104174.00586957"
    },
    {
        "symbol": "ETH-250516-2550-C",
        "priceChange": "-1.6",
        "priceChangePercent": "-0.0201",
        "lastPrice": "78",
        "lastQty": "0.2",
        "open": "79.6",
        "high": "115.8",
        "low": "77.2",
        "volume": "72.26",
        "amount": "6090.82",
        "bidPrice": "83.8",
        "askPrice": "85.2",
        "openTime": 1746898120943,
        "closeTime": 1746954696155,
        "firstTradeId": 1,
        "tradeCount": 24,
        "strikePrice": "2550",
        "exercisePrice": "2508.87674419"
    },
    {
        "symbol": "BTC-250725-130000-C",
        "priceChange": "0",
        "priceChangePercent": "0",
        "lastPrice": "2520",
        "lastQty": "0",
        "open": "2520",
        "high": "2520",
        "low": "2520",
        "volume": "0",
        "amount": "0",
        "bidPrice": "2630",
        "askPrice": "2885",
        "openTime": 0,
        "closeTime": 0,
        "firstTradeId": 0,
        "tradeCount": 0,
        "strikePrice": "130000",
        "exercisePrice": "104174.00586957"
    }
])";

int main() {
  const char *ptr = staticJson.c_str();

  // Move to the first object
  ptr = strchr(ptr, '{');

  auto start = std::chrono::high_resolution_clock::now();
  TickerEntry ticker = fastParseEntry(ptr);
  auto end = std::chrono::high_resolution_clock::now();

  std::chrono::duration<double, std::micro> duration = end - start;

  std::cout << "Parsed 1 entry in " << duration.count() << " µs\n";

  printTicker(ticker);

  return 0;
}
