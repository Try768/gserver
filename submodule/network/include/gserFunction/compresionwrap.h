#pragma once
#include <miniz.h>
#include <vector>
void compress_data(const std::vector<unsigned char>& input, std::vector<unsigned char>& output) {
    // Implementasi kompresi menggunakan miniz
    mz_ulong compressed_size = compressBound(input.size());
    output.resize(compressed_size);
    if (compress(output.data(), &compressed_size, input.data(), input.size()) == Z_OK) {
        output.resize(compressed_size);
    } else {
        output.clear();
    }
}

void decompress_data(const std::vector<unsigned char>& input, std::vector<unsigned char>& output,size_t original_size) {
    // Implementasi dekompresi menggunakan miniz
    mz_ulong decompressed_size = original_size;
    output.resize(decompressed_size);
    if (uncompress(output.data(), &decompressed_size, input.data(), input.size()) == Z_OK) {
        output.resize(decompressed_size);
    } else {
        output.clear();
    }

}
