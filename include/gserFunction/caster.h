#ifndef casTER
#define casTER
#include <vector>
#include <array>
#include <cstdint>
/// @brief everything here is unsigned
/// @param value 
/// @param str 
void ll_to_buffer_bigendian(uint64_t value, std::array<unsigned char, 8>& str) {
    for (int i = 0; i < 8; ++i) {
        str[i] = static_cast<unsigned char>((value >> (56 - i * 8)) & 0xFF);
    }
}
void int_to_buffer_bigendian(uint32_t value, std::array<unsigned char, 4>& str) {
    for (int i = 0; i < 4; ++i) {
        str[i] = static_cast<unsigned char>((value >> (24 - i * 8)) & 0xFF);
    }
}
void short_to_buffer_bigendian(uint16_t value, std::array<unsigned char, 2>& str) {
    for (int i = 0; i < 2; ++i) {
        str[i] = static_cast<unsigned char>((value >> (8 - i * 8)) & 0xFF);
    }
}
void buffer_to_ll_bigendian(const std::array<unsigned char, 8>& str, uint64_t& value) {
    value = 0;
    for (int i = 0; i < 8; ++i) {
        value |= (static_cast<uint64_t>(str[i]) << (56 - i * 8));
    }
}
void buffer_to_int_bigendian(const std::array<unsigned char, 4>& str, uint32_t& value) {
    value = 0;
    for (int i = 0; i < 4; ++i) {
        value |= (static_cast<uint32_t>(str[i]) << (24 - i * 8));
    }
}

// uint16_t (2 byte)
void buffer_to_short_bigendian(const std::array<unsigned char, 2>& str, uint16_t& value) {
    value = 0;
    for (int i = 0; i < 2; ++i) {
        value |= (static_cast<uint16_t>(str[i]) << (8 - i * 8));
    }
}
#endif // casTER