#include <benchmark/benchmark.h>
#include "fmt/core.h"
#include <string>
#include <sstream>
#include <format>  // C++20
#include <stdio.h>
#include <stdarg.h>
#include <iostream>

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
    std::ranges::fill_n(dst_buffer, 100'000, ' ');
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

static void GML_sprintf(benchmark::State& state) {
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

BENCHMARK(GML_sprintf);

static void GML_sprintf_length(benchmark::State& state) {
    for (auto _ : state) {
        state.PauseTiming();
        // This runs EVERY iteration (once per loop)
        initBuffers();
        state.ResumeTiming();
        size_t length = strlen(dst_buffer);
        const auto yesOrNo = (sample.flag == 0) ? "No" : "Yes";
        length += sprintf_s(dst_buffer + length, MAX_DST - length, ";$Flag Value:$ %s", yesOrNo);
        length += sprintf_s(dst_buffer + length, MAX_DST - length, ";$Launcher ID:$ %d", sample.id);
        length += sprintf_s(dst_buffer + length, MAX_DST - length, ";$Predicted Intercept Range:$ %.3f dm", sample.value);
        length += sprintf_s(dst_buffer + length, MAX_DST - length, ";$Platform Name:$ %s", sample.name);
        benchmark::DoNotOptimize(dst_buffer);
        benchmark::DoNotOptimize(tmp_buffer);
    }
}

BENCHMARK(GML_sprintf_length);

static void GML_std_format(benchmark::State& state) {
    for (auto _ : state) {
        state.PauseTiming();
        // This runs EVERY iteration (once per loop)
        initBuffers();
        state.ResumeTiming();
        const auto yesOrNo = (sample.flag == 0) ? "No" : "Yes";
        const auto flagStr = std::format(";$Flag Value:$ {:s}", yesOrNo);
        strcat_s(dst_buffer, MAX_DST, flagStr.c_str());
        const auto idStr = std::format(";$Launcher ID:$ {}", sample.id);
        strcat_s(dst_buffer, MAX_DST, idStr.c_str());
        const auto interceptStr = std::format(";$Predicted Intercept Range:$ {:.3f} dm", sample.value);
        strcat_s(dst_buffer, MAX_DST, interceptStr.c_str());
        const auto nameStr = std::format(";$Platform Name:$ {}", sample.name);
        strcat_s(dst_buffer, MAX_DST, nameStr.c_str());
        benchmark::DoNotOptimize(dst_buffer);
        benchmark::DoNotOptimize(tmp_buffer);
    }
}

BENCHMARK(GML_std_format);

static void GML_std_format_to(benchmark::State& state) {
    for (auto _ : state) {
        state.PauseTiming();
        // This runs EVERY iteration (once per loop)
        initBuffers();
        state.ResumeTiming();
        auto pEnd = dst_buffer + strlen(dst_buffer);
        const auto yesOrNo = (sample.flag == 0) ? "No" : "Yes";
        pEnd = std::format_to(pEnd, ";$Flag Value:$ {:s}", yesOrNo);
        pEnd = std::format_to(pEnd, ";$Launcher ID:$ {}", sample.id);
        pEnd = std::format_to(pEnd, ";$Predicted Intercept Range:$ {:.3f} dm", sample.value);
        pEnd = std::format_to(pEnd, ";$Platform Name:$ {}", sample.name);
        *pEnd = '\0'; // Null-terminate
        benchmark::DoNotOptimize(dst_buffer);
        benchmark::DoNotOptimize(tmp_buffer);
    }
}

BENCHMARK(GML_std_format_to);

static void GML_fmt_format(benchmark::State& state) {
    for (auto _ : state) {
        state.PauseTiming();
        // This runs EVERY iteration (once per loop)
        initBuffers();
        state.ResumeTiming();
        const auto yesOrNo = (sample.flag == 0) ? "No" : "Yes";
        const auto flagStr = fmt::format(";$Flag Value:$ {:s}", yesOrNo);
        strcat_s(dst_buffer, MAX_DST, flagStr.c_str());
        const auto idStr = fmt::format(";$Launcher ID:$ {}", sample.id);
        strcat_s(dst_buffer, MAX_DST, idStr.c_str());
        const auto interceptStr = fmt::format(";$Predicted Intercept Range:$ {:.3f} dm", sample.value);
        strcat_s(dst_buffer, MAX_DST, interceptStr.c_str());
        const auto nameStr = fmt::format(";$Platform Name:$ {}", sample.name);
        strcat_s(dst_buffer, MAX_DST, nameStr.c_str());
        benchmark::DoNotOptimize(dst_buffer);
        benchmark::DoNotOptimize(tmp_buffer);
    }
}

BENCHMARK(GML_fmt_format);

static void GML_fmt_format_to(benchmark::State& state) {
    for (auto _ : state) {
        state.PauseTiming();
        // This runs EVERY iteration (once per loop)
        initBuffers();
        state.ResumeTiming();
        auto pEnd = dst_buffer + strlen(dst_buffer);
        const auto yesOrNo = (sample.flag == 0) ? "No" : "Yes";
        pEnd = fmt::format_to(pEnd, ";$Flag Value:$ {:s}", yesOrNo);
        pEnd = fmt::format_to(pEnd, ";$Launcher ID:$ {}", sample.id);
        pEnd = fmt::format_to(pEnd, ";$Predicted Intercept Range:$ {:.3f} dm", sample.value);
        pEnd = fmt::format_to(pEnd, ";$Platform Name:$ {}", sample.name);
        *pEnd = '\0'; // Null-terminate
        benchmark::DoNotOptimize(dst_buffer);
        benchmark::DoNotOptimize(tmp_buffer);
    }
}

BENCHMARK(GML_fmt_format_to);

BENCHMARK_MAIN();
