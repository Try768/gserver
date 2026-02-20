#include <iostream>
#include <thread>
#include <atomic>
#include <iomanip>
#include <asio.hpp>
#include <string>
#include <vector>
#include <stdint.h>
#include <gserFunction/caster.h>
#include <gserFunction/formatTable.h>
#include <gserFunction/fungsiTable.hpp>
using asio::ip::udp;

void print_hex(const std::vector<unsigned char>& data) {
    for (unsigned char c : data) {
        std::cout << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(c) << ' ';
    }
    std::cout << '\n';
}

bool rltdt=false;




std::atomic<bool> running = true;

bool is_checking = false;
void playerKicker() {
    while (running) {
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        if (is_checking) continue; // Skip if already checking
        
        std::vector<player> cpl(lplayers); // Copy current players
        for (auto it = cpl.begin(); it != cpl.end();) {
            if (it->timeout <= 0) {
                std::cout << "Player " << it->name << " has timed out and will be removed.\n";
                it = cpl.erase(it); // Remove player
            } else {
                it->timeout -=500; // Decrease timeout
                ++it;
            }
        }
        lplayers =cpl;
    }
}
void checkPlayer(std::string& ip, std::string& port) {
    is_checking =true;
    
    for(auto& pl : lplayers) {
        if (pl.ip == ip && pl.port == port) {
            // Player already exists, update data
            pl.timeout = gtimeout; // Reset timeout
             // Update name or other data as needed
            return;
        }
    }

    
    // Implementasi untuk memeriksa pemain
    // Misalnya, periksa apakah pemain baru telah bergabung atau keluar
    // Update current_data atau lplayers sesuai kebutuhan
    is_checking = false;
}
uint64_t loginplayer(std::string& ip, std::string& port){
    player ppl;
    ppl.timeout = gtimeout; // Set initial timeout
    ppl.id = ++player_id; // Assign an ID based on current size
    ppl.name = "Player" + std::to_string(ppl.id); // Example name
    ppl.ip = ip;
    ppl.port = port;
    lplayers.push_back(ppl);
    std::cout << "New player detected: " << ppl.name << " (ID: " << ppl.id << ") from "
              << ppl.ip << ":" << ppl.port << "\n";
    return ppl.id;
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
      std::vector<unsigned char> response = headerParser(msg, sender.address().to_string(), std::to_string(sender.port()));
      //checkPlayer(msg, sender); // Check player data
      current_data.assign(buffer,buffer+len);
      std::error_code send_ec;
      socket.send_to(asio::buffer(response), sender, 0, send_ec);

      if (send_ec) {
        std::cerr << "Send error: " << send_ec.message() << "\n";
        continue;
      }
      if(rltdt){
        std::cout << "Received data: ";
        print_hex(msg);
        std::cout<<"response data: ";
        print_hex(response);
        std::cout << "from " << sender.address().to_string() << ":" << sender.port() << "\n";
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
        
        std::vector<player> cpl(lplayers);
        if(cpl.empty()){
          std::cout<<"no one here :(\n";
        }
        for(auto plc:cpl){
          std::cout<<"player id:"<<plc.id<<"\n";
          std::cout<<"player ip:"<<plc.ip<<"\n";
          std::cout<<"player name:"<<plc.name<<"\n";
          std::cout<<"player port:"<<plc.port<<"\n";
          std::cout<<"player hitung mundur koneksi:"<<plc.timeout<<"\n";
        }
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