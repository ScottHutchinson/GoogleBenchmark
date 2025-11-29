#include <benchmark/benchmark.h>
#include <string>
#include <array>
#include <unordered_map>
#include <random>

// Example enum
enum class StatusCode {
    Success = 57,
    InvalidInput = 58,
    NotFound = 59,
    Unauthorized = 60,
    ServerError = 61,
    Timeout = 62,
    RateLimited = 63,
    BadRequest = 64,
    Forbidden = 65,
    Conflict = 66
};

static constexpr int MIN_STATUS = 57;
static constexpr int MAX_STATUS = 66;
static constexpr int STATUS_OFFSET = static_cast<int>(StatusCode::Success);

// Method 1: Switch statement
const char* DecodeSwitch(const int code) {
    switch (code) {
    case 57: return "Success";
    case 58: return "InvalidInput";
    case 59: return "NotFound";
    case 60: return "Unauthorized";
    case 61: return "ServerError";
    case 62: return "Timeout";
    case 63: return "RateLimited";
    case 64: return "BadRequest";
    case 65: return "Forbidden";
    case 66: return "Conflict";
    default: return "Unknown";
    }
}

const char* DecodeCastSwitch(const int code) {
    switch (static_cast<StatusCode>(code)) {
    case StatusCode::Success: return "Success";
    case StatusCode::InvalidInput: return "InvalidInput";
    case StatusCode::NotFound: return "NotFound";
    case StatusCode::Unauthorized: return "Unauthorized";
    case StatusCode::ServerError: return "ServerError";
    case StatusCode::Timeout: return "Timeout";
    case StatusCode::RateLimited: return "RateLimited";
    case StatusCode::BadRequest: return "BadRequest";
    case StatusCode::Forbidden: return "Forbidden";
    case StatusCode::Conflict: return "Conflict";
    default: return "Unknown";
    }
}

// Method 2: Array lookup (fastest for contiguous values)
static constexpr std::array<const char*, 10> STATUS_NAMES = {
    "Success",
    "InvalidInput",
    "NotFound",
    "Unauthorized",
    "ServerError",
    "Timeout",
    "RateLimited",
    "BadRequest",
    "Forbidden",
    "Conflict"
};

const char* DecodeArray(const int code) {
    if (const int index = code - STATUS_OFFSET; index >= 0 && index < static_cast<int>(STATUS_NAMES.size())) {
        return STATUS_NAMES[index];
    }
    return "Unknown";
}

// Method 2b: C-style array lookup
static constexpr const char* STATUS_NAMES_C[] = {
    "Success",
    "InvalidInput",
    "NotFound",
    "Unauthorized",
    "ServerError",
    "Timeout",
    "RateLimited",
    "BadRequest",
    "Forbidden",
    "Conflict"
};
static constexpr int STATUS_NAMES_C_SIZE = std::size(STATUS_NAMES_C);

const char* DecodeCArray(const int code) {
    if (const int index = code - STATUS_OFFSET; index >= 0 && index < STATUS_NAMES_C_SIZE) {
        return STATUS_NAMES_C[index];
    }
    return "Unknown";
}

// Method 3: Hash map (good for sparse enums)
const std::unordered_map<int, const char*> STATUS_MAP = {
    {57, "Success"},
    {58, "InvalidInput"},
    {59, "NotFound"},
    {60, "Unauthorized"},
    {61, "ServerError"},
    {62, "Timeout"},
    {63, "RateLimited"},
    {64, "BadRequest"},
    {65, "Forbidden"},
    {66, "Conflict"}
};

const char* DecodeHashMap(int code) {
    auto it = STATUS_MAP.find(code);
    if (it != STATUS_MAP.end()) {
        return it->second;
    }
    return "Unknown";
}

// Method 4: If-else chain
const char* DecodeIfElse(int code) {
    if (code == 57) return "Success";
    else if (code == 58) return "InvalidInput";
    else if (code == 59) return "NotFound";
    else if (code == 60) return "Unauthorized";
    else if (code == 61) return "ServerError";
    else if (code == 62) return "Timeout";
    else if (code == 63) return "RateLimited";
    else if (code == 64) return "BadRequest";
    else if (code == 65) return "Forbidden";
    else if (code == 66) return "Conflict";
    else return "Unknown";
}

// Fixture to generate random test data
class EnumDecodeFixture : public benchmark::Fixture {
public:
    std::vector<int> random_codes;
    std::mt19937 rng;

    void SetUp(const ::benchmark::State& /*state*/) override {
        // Generate random enum values
        rng.seed(42);  // Fixed seed for reproducibility
        std::uniform_int_distribution<int> dist(MIN_STATUS, MAX_STATUS);

        random_codes.reserve(10000);
        for (int i = 0; i < 10000; ++i) {
            random_codes.push_back(dist(rng));
        }
    }

    void TearDown(const ::benchmark::State& /*state*/) override {
        random_codes.clear();
    }
};

// Benchmark: Switch statement
BENCHMARK_F(EnumDecodeFixture, DE_DecodeSwitch)(benchmark::State& state) {
    size_t idx = 0;
    for (auto _ : state) {
        int code = random_codes[idx % random_codes.size()];
        const char* result = DecodeSwitch(code);
        benchmark::DoNotOptimize(result);
        ++idx;
    }
}

BENCHMARK_F(EnumDecodeFixture, DE_DecodeCastSwitch)(benchmark::State& state) {
    size_t idx = 0;
    for (auto _ : state) {
        int code = random_codes[idx % random_codes.size()];
        const char* result = DecodeCastSwitch(code);
        benchmark::DoNotOptimize(result);
        ++idx;
    }
}

// Benchmark: Array lookup
BENCHMARK_F(EnumDecodeFixture, DE_DecodeArray)(benchmark::State& state) {
    size_t idx = 0;
    for (auto _ : state) {
        int code = random_codes[idx % random_codes.size()];
        const char* result = DecodeArray(code);
        benchmark::DoNotOptimize(result);
        ++idx;
    }
}

// Benchmark: C-style array lookup
BENCHMARK_F(EnumDecodeFixture, DE_DecodeCArray)(benchmark::State& state) {
    size_t idx = 0;
    for (auto _ : state) {
        int code = random_codes[idx % random_codes.size()];
        const char* result = DecodeCArray(code);
        benchmark::DoNotOptimize(result);
        ++idx;
    }
}

// Benchmark: Hash map
BENCHMARK_F(EnumDecodeFixture, DE_DecodeHashMap)(benchmark::State& state) {
    size_t idx = 0;
    for (auto _ : state) {
        int code = random_codes[idx % random_codes.size()];
        const char* result = DecodeHashMap(code);
        benchmark::DoNotOptimize(result);
        ++idx;
    }
}

// Benchmark: If-else chain
BENCHMARK_F(EnumDecodeFixture, DE_DecodeIfElse)(benchmark::State& state) {
    size_t idx = 0;
    for (auto _ : state) {
        int code = random_codes[idx % random_codes.size()];
        const char* result = DecodeIfElse(code);
        benchmark::DoNotOptimize(result);
        ++idx;
    }
}

// Bonus: Test with edge cases (including invalid values)
static void DE_DecodeSwitch_WithInvalid(benchmark::State& state) {
    std::mt19937 rng(42);
    std::uniform_int_distribution<int> dist(52, 71);  // Include invalid values around the range

    for (auto _ : state) {
        state.PauseTiming();
        int code = dist(rng);
        state.ResumeTiming();

        const char* result = DecodeSwitch(code);
        benchmark::DoNotOptimize(result);
    }
}
BENCHMARK(DE_DecodeSwitch_WithInvalid);

static void DE_DecodeArray_WithInvalid(benchmark::State& state) {
    std::mt19937 rng(42);
    std::uniform_int_distribution<int> dist(52, 71);

    for (auto _ : state) {
        state.PauseTiming();
        int code = dist(rng);
        state.ResumeTiming();

        const char* result = DecodeArray(code);
        benchmark::DoNotOptimize(result);
    }
}
BENCHMARK(DE_DecodeArray_WithInvalid);

// Benchmark with different access patterns
static void DE_DecodeArray_Sequential(benchmark::State& state) {
    int code = 0;
    for (auto _ : state) {
        const char* result = DecodeArray(code);
        benchmark::DoNotOptimize(result);
        code = (code + 1) % (MAX_STATUS + 1);
    }
}
BENCHMARK(DE_DecodeArray_Sequential);

static void DE_DecodeArray_WorstCase(benchmark::State& state) {
    // Always decode the last value (worst case for linear search if any)
    for (auto _ : state) {
        const char* result = DecodeArray(MAX_STATUS);
        benchmark::DoNotOptimize(result);
    }
}
BENCHMARK(DE_DecodeArray_WorstCase);
