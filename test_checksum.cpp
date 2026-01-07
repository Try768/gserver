#include <vector>
#include <iostream>
#include "include/miniz.h"

static inline bool verifycheksum(
        std::vector<unsigned char>::const_iterator awal,
        std::vector<unsigned char>::const_iterator akhir,
        uint32_t checksum
    ){
        if (awal == akhir)
            return checksum == 0;

        return mz_crc32(
            0,
            std::addressof(*awal),
            static_cast<size_t>(std::distance(awal, akhir))
        ) == checksum;
}

int main() {
    std::vector<unsigned char> data = {'h', 'e', 'l', 'l', 'o'};
    uint32_t crc = mz_crc32(0, data.data(), data.size());
    std::cout << "CRC32 of 'hello': " << crc << std::endl;
    bool valid = verifycheksum(data.begin(), data.end(), crc);
    std::cout << "Verification: " << (valid ? "true" : "false") << std::endl;

    // Test empty
    std::vector<unsigned char> empty;
    uint32_t crc_empty = 0;
    bool valid_empty = verifycheksum(empty.begin(), empty.end(), crc_empty);
    std::cout << "Empty verification: " << (valid_empty ? "true" : "false") << std::endl;

    return 0;
}