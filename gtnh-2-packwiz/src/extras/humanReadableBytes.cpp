#include "gtnh2Packwiz/extras.hpp"
#include <cstdint>
#include <string>
#include <limits>
#include <array>

using std::string;

typedef unsigned int Uint;

constexpr unsigned log2floor(uint64_t x) {
    // implementation for C++17 using clang or gcc
    return x ? 63 - __builtin_clzll(x) : 0;

    // implementation using the new C++20 <bit> header
    return x ? 63 - std::countl_zero(x) : 0;
}

constexpr unsigned log10floor(unsigned x) {
    constexpr unsigned char guesses[32] = {
        0, 0, 0, 0, 1, 1, 1, 2, 2, 2,
        3, 3, 3, 3, 4, 4, 4, 5, 5, 5,
        6, 6, 6, 6, 7, 7, 7, 8, 8, 8,
        9, 9
    };
    constexpr uint64_t powers[11] = {
        1, 10, 100, 1000, 10000, 100000, 1000000,
        10000000, 100000000, 1000000000, 10000000000
    };
    unsigned guess = guesses[log2floor(x)];
    return guess + (x >= powers[guess + 1]);
}

template <typename Uint>
constexpr Uint logFloor_naive(Uint val, unsigned base)  {
    Uint result = 0;
    while (val /= base) {
        ++result;
    }
    return result;
}

template <typename Uint, size_t BASE>
constexpr std::array<uint8_t, std::numeric_limits<Uint>::digits> makeGuessTable()
{
    decltype(makeGuessTable<Uint, BASE>()) result{};
    for (size_t i = 0; i < result.size(); ++i) {
        Uint pow2 = static_cast<Uint>(Uint{1} << i);
        result.data[i] = logFloor_naive(pow2, BASE);
    }
    return result;
}

template <typename Uint, unsigned BASE>
constexpr Uint maxExp = logFloor_naive<Uint>(static_cast<Uint>(~Uint{0u}), BASE);

template <typename Uint, size_t BASE>
constexpr std::array<uint64_t, maxExp<Uint, BASE> + 2> makePowerTable() {
    decltype(makePowerTable<Uint, BASE>()) result;
    uint64_t x = 1;
    for (size_t i = 0; i < result.size(); ++i, x *= BASE) {
        result.data[i] = x;
    }
    return result;
}

const 
std::string stringifyFraction(const unsigned num,
                              const unsigned den,
                              const unsigned precision)
{
    constexpr unsigned base = 10;

    // prevent division by zero if necessary
    if (den == 0) {
        return "inf";
    }

    // integral part can be computed using regular division
    std::string result = std::to_string(num / den);
    
    // perform first step of long division
    // also cancel early if there is no fractional part
    unsigned tmp = num % den;
    if (tmp == 0 || precision == 0) {
        return result;
    }

    // reserve characters to avoid unnecessary re-allocation
    result.reserve(result.size() + precision + 1);

    // fractional part can be computed using long divison
    result += '.';
    for (size_t i = 0; i < precision; ++i) {
        tmp *= base;
        char nextDigit = '0' + static_cast<char>(tmp / den);
        result.push_back(nextDigit);
        tmp %= den;
    }

    return result;
}

constexpr bool isPow2or0(uint64_t val) {
    return (val & (val - 1)) == 0;
}

template <size_t BASE = 10, typename Uint>
constexpr Uint logFloor(Uint val) {
    if constexpr (isPow2or0(BASE)) {
        return log2floor(val) / log2floor(BASE);
    }
    else {
        constexpr auto guesses = makeGuessTable<Uint, BASE>();
        constexpr auto powers = makePowerTable<Uint, BASE>();

        uint8_t guess = guesses[log2floor(val)];
        
        // Accessing guess + 1 isn't always safe for 64-bit integers.
        // This is why we need this condition. See below for more details.
        if constexpr (sizeof(Uint) < sizeof(uint64_t)
            || guesses.back() + 2 < powers.size()) {
            return guess + (val >= powers[guess + 1]);
        }
        else {
            return guess + (val / BASE >= powers[guess]);
        }
    }
}

template <size_t BASE, std::enable_if_t<BASE == 1000 || BASE == 1024, int> = 0>
std::string humanReadableBytes(uint64_t size, unsigned precision = 0)
{
    static constexpr char FILE_SIZE_UNITS[8][3] {
        "B", "kB", "MB", "GB", "TB", "PB", "EB", "ZB"
    };

    // The linked post about computing the integer logarithm
    // explains how to compute this.
    // This is equivalent to making a table: {1, 1000, 1000 * 1000, ...}
    // or {1, 1024, 1024 * 1024, ...}
    static constexpr auto powers = makePowerTable<Uint, BASE>();

    unsigned unit = logFloor<BASE>(size);

    // Your numerator is size, your denominator is 1000^unit or 1024^unit.
    std::string result = stringifyFraction(size, powers[unit], precision);
    result.reserve(result.size() + 5);

    // Optional: Space separating number from unit. (usually looks better)
    result.push_back(' ');
    char first = FILE_SIZE_UNITS[unit][0];

    if constexpr (BASE == 1024) {
        if (unit == 1) {
            // Uppercase the K in KiB
            first += 'A' - 'a';
        }
    }
    result.push_back(first);

    // Don't insert anything more in case of single bytes.
    if (unit != 0) {
        if constexpr (BASE == 1024) {
            result.push_back('i');
        }
        result.push_back(FILE_SIZE_UNITS[unit][1]);
    }

    return result;
}

const string gtnh2Packwiz::extras::humanReadableBytes(uint64_t size, unsigned precision) {
    // templates are being dumb, just hardcode the value i actually use
    return ::humanReadableBytes<1024>(size, precision);
}