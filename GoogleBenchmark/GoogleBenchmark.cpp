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

struct Sample_t {
    uint8_t flag;
    char pad[3];
    int id;
    double value;
    char name[256];
};

static constexpr size_t MAX_DST{ 300'000 };
static char dst_buffer[MAX_DST]{};
static char tmp_buffer[MAX_DST]{};

static void initBuffers() {
    std::ranges::fill(dst_buffer, '\0');
    std::ranges::fill(tmp_buffer, '\0');
}

static Sample_t sample{
    1,
    {0,0,0},
    12345,
    99.99,
    "Sample Name"
};

static void doYesOrNo(char* dst, const char* lbl, const uint8_t flag) {
    strcat_s(dst, MAX_DST, lbl);
    const auto yesOrNo = (flag == 0) ? "No" : "Yes";
    strcat_s(dst, MAX_DST, yesOrNo);
}

static void doCharArray(char* dst, const char* lbl, const char* src) {
    strcat_s(dst, MAX_DST, lbl);
    strcat_s(dst, MAX_DST, src);
}

static void GML_strcat(benchmark::State& state) {
    for (auto _ : state) {
        state.PauseTiming();
        // This runs EVERY iteration (once per loop)
        initBuffers();
        state.ResumeTiming();
        doYesOrNo(dst_buffer, ";$Flag Value:$ ", sample.flag);
        sprintf_s(tmp_buffer, MAX_DST, ";$Launcher ID:$ %d", sample.id); strcat_s(dst_buffer, MAX_DST, tmp_buffer);
        sprintf_s(tmp_buffer, MAX_DST, ";$Predicted Intercept Range:$ %.3f dm", sample.value); strcat_s(dst_buffer, MAX_DST, tmp_buffer);
        doCharArray(dst_buffer, ";$Platform Name:$ ", sample.name);
        benchmark::DoNotOptimize(dst_buffer);
        benchmark::DoNotOptimize(tmp_buffer);
    }
}

BENCHMARK(GML_strcat);

BENCHMARK_MAIN();
