#ifndef MIDI_PARSERSYNTHESIZER_SINELOOKUPTABLE_H
#define MIDI_PARSERSYNTHESIZER_SINELOOKUPTABLE_H
#include <array>
#include <cstddef>
#include <cmath>

class SineLookupTable {

public:
    static constexpr std::size_t TABLE_SIZE = 4096;

private:
    static constexpr std::size_t TABLE_MASK = TABLE_SIZE - 1;
    static constexpr double TWO_PI = 6.2831853071795862;
    inline static std::array<float, TABLE_SIZE> table;
    inline static bool is_initialized = false;

public:
    SineLookupTable() = delete;

    static void init() {
        if (is_initialized) return;

        for (size_t i = 0; i < TABLE_SIZE; i++) {
            float angle = static_cast<float>(i) / TABLE_SIZE * TWO_PI;
            table[i] = std::sin(angle);
        }

        is_initialized = true;
    }

    [[nodiscard]] static inline float sin(float raw_idx) {
        if (!is_initialized) init();

        int idx_1 = static_cast<int>(raw_idx);
        int idx_2 = idx_1 + 1;
        const float frac = raw_idx - static_cast<float>(idx_1);

        // Bitwise AND on 32-bit signed integers with TABLE_MASK to ensure
        // idx_1 and idx_2 are within array boundaries
        idx_1 &= TABLE_MASK;
        idx_2 &= TABLE_MASK;

        // Linear interpolation
        return table[idx_1] + frac * (table[idx_2] - table[idx_1]);
    }
};

#endif //MIDI_PARSERSYNTHESIZER_SINELOOKUPTABLE_H