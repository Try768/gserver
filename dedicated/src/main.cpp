#include <iostream>
#include <thread>
#include <atomic>
#include <iomanip>
#include <asio.hpp>
#include <string>
#include <vector>
using asio::ip::udp;
void print_hex(const std::vector<unsigned char>& data) {
    for (unsigned char c : data) {
        std::cout << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(c) << ' ';
    }
    std::cout << '\n';
}
void ll_to_string_bigendian(unsigned long long value, std::string& str) {
    str.resize(8);
    for (int i = 0; i < 8; ++i) {
        str[i] = static_cast<char>((value >> (56 - i * 8)) & 0xFF);
    }
}
bool rltdt=false;
constexpr int gtimeout = 5000; // Timeout in milliseconds
struct player{
    unsigned long long id;
    int timeout;
    std::string name;
    std::string ip;
    std::string port;
};
std::string ttd={
    static_cast<char>(0x01), static_cast<char>(0x08), static_cast<char>(0xFF),
     static_cast<char>(0x02), static_cast<char>(0xfb), static_cast<char>(0xa3),
      static_cast<char>(0x01), static_cast<char>(0x08),
};
unsigned long long player_id = 0;
std::atomic<bool> running = true;
std::vector<unsigned char> current_data;
std::vector<player> lplayers;
bool is_checking = false;
void playerKicker() {
    while (running) {
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        if (is_checking) continue; // Skip if already checking
        
        std::vector<player> cpl(lplayers); // Copy current players
        for (auto it = lplayers.begin(); it != lplayers.end();) {
            if (it->timeout <= 0) {
                std::cout << "Player " << it->name << " has timed out and will be removed.\n";
                it = lplayers.erase(it); // Remove player
            } else {
                it->timeout -=500; // Decrease timeout
                ++it;
            }
        }
        lplayers =cpl;
    }
}
void checkPlayer(std::vector<unsigned char>& msg,asio::ip::udp::endpoint& sender) {
    is_checking =true;
    for(auto& pl : lplayers) {
        if (pl.ip == sender.address().to_string() && pl.port == std::to_string(sender.port())) {
            // Player already exists, update data
            pl.timeout = gtimeout; // Reset timeout
             // Update name or other data as needed
            return;
        }
    }

    player ppl;
    ppl.timeout = gtimeout; // Set initial timeout
    ppl.id = ++player_id; // Assign an ID based on current size
    ppl.name = "Player" + std::to_string(ppl.id); // Example name
    ppl.ip = sender.address().to_string();
    ppl.port = std::to_string(sender.port());
    lplayers.push_back(ppl);
    is_checking = false;
    std::cout << "New player detected: " << ppl.name << " (ID: " << ppl.id << ") from "
              << ppl.ip << ":" << ppl.port << "\n";
    // Implementasi untuk memeriksa pemain
    // Misalnya, periksa apakah pemain baru telah bergabung atau keluar
    // Update current_data atau lplayers sesuai kebutuhan
}
void udp_server_thread() {
   asio::io_context io;
   udp::socket socket(io, udp::endpoint(udp::v4(), 12345));

  socket.non_blocking(true); // Ubah ke non-blocking

  char buffer[1024];
  udp::endpoint sender;

  std::cout << "UDP server running on port 12345...\nwrite help for commands\n";

  while (running) {
    std::error_code ec;
    size_t len = socket.receive_from(asio::buffer(buffer), sender, 0, ec);

    if (!ec) {
      std::vector<unsigned char> msg;
      msg.assign(buffer,buffer+len);
      if(len==0){
        std::cout << "Received empty message from " << sender << "\n";
        continue;
      }
      checkPlayer(msg, sender); // Check player data
      current_data.assign(buffer,buffer+len);
      if(rltdt){
        std::cout << "Received data: ";
        print_hex(msg);
        std::cout<<"\n";
      }
      //std::cout << "Received: " << msg << " from " << sender << "\n";
      
    } else if (ec == asio::error::would_block) {
      // Tidak ada data, delay kecil biar CPU tidak full
      std::this_thread::sleep_for(std::chrono::milliseconds(50));
    } else {
      std::cerr << "Receive error: " << ec.message() << "\n";
      break;
    }
  }

  std::cout << "UDP server stopped.\n";
}

int main() {
  std::thread server(udp_server_thread);
    std::thread kicker(playerKicker);
  std::string cmd;
  while (true) {
    std::getline(std::cin, cmd);
    if (cmd == "help") {
      std::cout << "Available commands:\n";
      std::cout << "  help - Show this help message\n";
      std::cout << "  list - to show all player and their data\n";
      std::cout << "  exit or quit - Stop the server and exit\n";
      std::cout << "  data - to show current data\n";
      std::cout << "  rltdata - to show current data in real time\n";
    } 
    else if (cmd == "list") {
      std::cout << "List of players and their data:\n";
        std::cout<< "masih dikerjain\n";
      // Implementasi untuk menampilkan daftar pemain
    } 
    else if (cmd == "data") {
      std::cout << "Current data:\n";
        std::vector<unsigned char> msg(current_data);
        if(msg.empty()){
            std::cout << "No data received yet.\n";
            continue;
        }
        std::cout << "Data: ";
        print_hex(msg);
        std::cout<<"\n";
      // Implementasi untuk menampilkan data saat ini
    } 
    else if (cmd == "rltdata") {
      std::cout << "Real-time data:";
      if(rltdt){
          std::cout<<"deactivated\n";
        }else{
            std::cout<<"activated\n";
        }
        rltdt = !rltdt; // Toggle real-time data display
      // Implementasi untuk menampilkan data secara real-time
    } 
    else
    if (cmd == "exit" || cmd == "quit") {
      running = false;
      break;
 
   }
  }
  kicker.join();
  server.join();
  return 0;
}