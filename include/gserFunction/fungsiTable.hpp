#if !defined(FUNGSI_TABLE_HPP)
#define FUNGSI_TABLE_HPP

#include <array>
#include <cstdint>
#include <vector>
#include <formatTable.h>
#include <gserFunction/caster.h>
#include <iostream>
constexpr int gtimeout = 5000; // Timeout in milliseconds
std::vector<unsigned char> ttd={
    (0x01), (0x08), (0xFF),(0x02),
     (0xfb), (0xa3),(0x01), (0x08),
};
uint32_t version=1;
unsigned char subversi=0;
unsigned long long player_id = 0;
bool checktoken(uint64_t tokenplayer);
std::vector<unsigned char> current_data;
uint64_t loginplayer(std::string& ip, std::string& port);
std::vector<player> lplayers;
void checkPlayer(std::string& ip, std::string& port);
std::vector<unsigned char> headerParser(const std::vector<unsigned char>& msg, std::string& ip, std::string& port); 
void isiParser(const std::vector<unsigned char>& msg);
#endif // FUNGSI_TABLE_HPP