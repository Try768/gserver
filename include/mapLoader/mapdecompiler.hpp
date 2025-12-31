#ifndef mapeloaderdothapepe
#define mapeloaderdothapepe
#include <gserFunction/caster.h>
#include <json.hpp>
#include <iostream>
#include <fstream>
#include <limits>
nlohmann::json mapDecompiler(const std::vector<unsigned char>& buffer) {
    nlohmann::json result;
    size_t offset = 0;

    auto readU8 = [&](unsigned char& out) {
            if (offset + 1 > buffer.size()) throw std::runtime_error("Buffer underrun (u8)");
            out = buffer[offset++];
        };

    auto readUInt = [&](uint64_t& out, unsigned char size) {
        if (offset + size > buffer.size())
            throw std::runtime_error("Buffer underrun");

        out = 0;
        for (unsigned char i = 0; i < size; i++) {
            out = (out << 8) | buffer[offset + i];
        }
        offset += size;
    };

    auto readU32 = [&](uint32_t& out) {
        if (offset + 4 > buffer.size()) throw std::runtime_error("Buffer underrun (u32)");
        out = (static_cast<uint32_t>(buffer[offset])   << 24) |
              (static_cast<uint32_t>(buffer[offset+1]) << 16) |
              (static_cast<uint32_t>(buffer[offset+2]) << 8)  |
              (static_cast<uint32_t>(buffer[offset+3]));
        offset += 4;
    };

    auto readU64 = [&](uint64_t& out) {
        if (offset + 8 > buffer.size()) throw std::runtime_error("Buffer underrun (u64)");
        out = 0;
        for (int i = 0; i < 8; i++) {
            out = (out << 8) | buffer[offset+i];
        }
        offset += 8;
    };

    auto readString = [&](std::string& out, size_t len) {
        if (offset + len > buffer.size()) throw std::runtime_error("Buffer underrun (string)");
        out.assign(reinterpret_cast<const char*>(&buffer[offset]), len);
        offset += len;
    };

    try {
        // idLenght
        unsigned char idLength;
        readU8(idLength);
        result["idLenght"] = idLength;
        std::cout << "idLenght: " << static_cast<int>(idLength) << "\n";
        // idTile
        uint64_t tileCount;
        readU64(tileCount);
        std::cout << "Tile count: " << tileCount << "\n";
        nlohmann::json idTile = nlohmann::json::array();
        nlohmann::json idcomponent = nlohmann::json::array();
        for (size_t i = 0; i < tileCount; i++) {
            uint32_t strLen;
            readU32(strLen);
            std::cout << "Tile " << i << " length: " << strLen << "\n";
            std::string s;
            readString(s, strLen);
            std::cout << "Tile " << i << ": " << s << "\n";
            idTile.push_back(s);
            unsigned char component;
            readU8(component);
            idcomponent.push_back(component);
        }
        result["idTile"] = idTile;
        result["idComponent"]=idcomponent;
        // idOrder
        uint64_t orderCount;
        readU64(orderCount);

        nlohmann::json idOrder = nlohmann::json::array();
        for (size_t i = 0; i < orderCount; i++) {
            uint64_t val;
            //readU64(val);
            readUInt(val, idLength);
            idOrder.push_back(val);
            for (size_t j = 0; j < idcomponent[val].get<size_t>(); j++) {
                unsigned char comp;
                readU8(comp);
                idOrder.push_back(comp);
                i++; // Increment i to account for the component
            }
        }
        result["idOrder"] = idOrder;

        return result;
    }
    catch (const std::exception& e) {
        std::cerr << "Error at map decompiler: " << e.what() << "\n";
        return {};
    }
}
#endif