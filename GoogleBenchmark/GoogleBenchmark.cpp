#include <benchmark/benchmark.h>
#include <string>
#include <sstream>
#include <format>  // C++20
#include <stdio.h>

// Method 1: std::ostringstream
static void BM_Ostringstream(benchmark::State& state) {
    int id = 12345;
    double price = 99.99;
    const char* name = "Widget";

    for (auto _ : state) {
        std::ostringstream oss;
        oss << "Product: " << name << ", ID: " << id << ", Price: $" << price;
        std::string result = oss.str();
        benchmark::DoNotOptimize(result);
    }
}
BENCHMARK(BM_Ostringstream);

// Method 2: String concatenation with std::to_string
static void BM_StringConcat(benchmark::State& state) {
    int id = 12345;
    double price = 99.99;
    const char* name = "Widget";

    for (auto _ : state) {
        std::string result = std::string("Product: ") + name +
            ", ID: " + std::to_string(id) +
            ", Price: $" + std::to_string(price);
        benchmark::DoNotOptimize(result);
    }
}
BENCHMARK(BM_StringConcat);

// Method 3: sprintf to buffer then string
static void BM_Sprintf(benchmark::State& state) {
    int id = 12345;
    double price = 99.99;
    const char* name = "Widget";
    char buffer[256];

    for (auto _ : state) {
        sprintf_s(buffer, sizeof(buffer),
            "Product: %s, ID: %d, Price: $%.2f", name, id, price);
        std::string result(buffer);
        benchmark::DoNotOptimize(result);
    }
}
BENCHMARK(BM_Sprintf);

// Method 4: C++20 std::format (fastest, if available)
static void BM_StdFormat(benchmark::State& state) {
    int id = 12345;
    double price = 99.99;
    const char* name = "Widget";

    for (auto _ : state) {
        std::string result = std::format("Product: {}, ID: {}, Price: ${:.2f}",
            name, id, price);
        benchmark::DoNotOptimize(result);
    }
}
BENCHMARK(BM_StdFormat);

// Method 5: Reserve + append (manual optimization)
static void BM_ReserveAppend(benchmark::State& state) {
    int id = 12345;
    double price = 99.99;
    const char* name = "Widget";

    for (auto _ : state) {
        std::string result;
        result.reserve(64);  // Pre-allocate
        result += "Product: ";
        result += name;
        result += ", ID: ";
        result += std::to_string(id);
        result += ", Price: $";
        result += std::to_string(price);
        benchmark::DoNotOptimize(result);
    }
}
BENCHMARK(BM_ReserveAppend);

// Bonus: Compare with different string lengths
static void BM_Format_ShortString(benchmark::State& state) {
    int n = 42;
    for (auto _ : state) {
        std::string result = std::format("Value: {}", n);
        benchmark::DoNotOptimize(result);
    }
}
BENCHMARK(BM_Format_ShortString);

static void BM_Concat_ShortString(benchmark::State& state) {
    int n = 42;
    for (auto _ : state) {
        std::string result = "Value: " + std::to_string(n);
        benchmark::DoNotOptimize(result);
    }
}
BENCHMARK(BM_Concat_ShortString);

BENCHMARK_MAIN();
