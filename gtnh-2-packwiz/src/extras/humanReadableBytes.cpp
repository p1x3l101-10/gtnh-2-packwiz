#include "gtnh2Packwiz/extras.hpp"
#include <string>

using std::string;

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

template <size_t BASE, std::enable_if_t<BASE == 1000 || BASE == 1024, int>>
const string gtnh2Packwiz::extras::humanReadableBytes(uintmax_t size, unsigned precision) {
    static constexpr const char* fileSizeUnits[5] {
        "B", "kB", "MB", "GB", "TB"
    };
    static constexpr auto powers = makePowerTable<unsigned int, BASE>();
    unsigned unit = logFloor<BASE>(size);
    string result = stringifyFraction(size, powers[unit], precision);
    result.reserve(result.size() + 5);
    result.push_back(' ');
    if constexpr (BASE == 1000) {
        result.push_back(fileSizeUnits[unit][0]);
    } else {
        char pfx = fileSizeUnits[unit][0];
        if (unit == 1) {
            pfx += 'A' - 'a'; // Convert to uppercase bc its KiB, not kiB
        }
        result.push_back(pfx);
    }
    if (unit != 0) {
        if constexpr (BASE == 1024) {
            result.push_back('i');
        }
        result.push_back(fileSizeUnits[size][1]);
    }
    result.shrink_to_fit();
    return result;
};