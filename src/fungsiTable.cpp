#include <gserFunction/formatTable.h>
#include <gserFunction/caster.h>
#include <iostream>
#include <gserFunction/fungsiTable.hpp>
#include <log.hpp>
#include <maploader/maploader.hpp>
#include <algorithm>
//std::vector<uint32_t> token;
//paket minimal hingga ke signature
const uint32_t minsigp =9;
void cekKoneksi(const std::vector<unsigned char>& msg,std::vector<unsigned char>& response,std::string& ip, std::string& port){
    if (msg.size() < 16) 
    {
        log_info("Invalid message size from " + ip + ":" + port);
        return;
        /* code */
    }
    
    for(size_t i=1;i<9;i++){
        if(msg[i] != ttd[i-1]){
            //std::cout << "Invalid connection signature from " << ip << ":" << port << "\n";
            log_info("Invalid connection signature from " + ip + ":" + port);
            return; // Invalid token, do not respond
        }
    }
    uint64_t token_value ;
     buffer_to_ll_bigendian(std::array<unsigned char, 8>{msg[9], msg[10], msg[11], msg[12],msg[13],msg[14],msg[15],msg[16]},token_value);
     for(auto anag:lplayers){
        if(anag.id=token_value){
            checkPlayer(ip, port); // Check player data
            response.clear();
            response.push_back(id_table::server::cek_koneksi);
            response.insert(response.end(), ttd.begin(), ttd.end());
        }
     }
    
}
bool checktoken(uint64_t tokenplayer){
    for(auto pplayer:lplayers){
        if(pplayer.id==tokenplayer){
            return true;
        }
    }
    return false;
}
void pralogin(const std::vector<unsigned char>& msg,std::vector<unsigned char>& response,std::string& ip, std::string& port){
    if (msg.size() < 9) 
    {
        log_info("Invalid message size from " + ip + ":" + port);
        return;
        /* code */
    }
    
    for(size_t i=1;i<9;i++){
        if(msg[i] != ttd[i-1]){
            //std::cout << "Invalid connection signature from " << ip << ":" << port << "\n";
            log_info("Invalid connection signature from " + ip + ":" + port);
            return; // Invalid token, do not respond
        }
    }
    response.clear();
    response.emplace_back(id_table::server::pralogin);
    response.insert(response.end(),ttd.begin(),ttd.end());
    std::string judul="aku mau 3";
    std::array<unsigned char,2> panjang;
    short_to_buffer_bigendian(static_cast<unsigned short>(judul.size()),panjang);
    response.insert(response.end(),panjang.begin(),panjang.end());
    response.insert(response.end(),judul.begin(),judul.end());
    

}
void login(const std::vector<unsigned char>& msg,std::vector<unsigned char>& response,std::string& ip, std::string& port){
    if (msg.size() < 10) 
    {
        log_info("Invalid message size from " + ip + ":" + port);
        return;
        /* code */
    }
    
    for(size_t i=1;i<9;i++){
        if(msg[i] != ttd[i-1]){
            //std::cout << "Invalid connection signature from " << ip << ":" << port << "\n";
            log_info("Invalid connection signature from " + ip + ":" + port);
            return; // Invalid token, do not respond
        }
    }
    response.clear();
    switch (msg[9])
    {
        case 0x01:{
        /* code */
            response.emplace_back(id_table::server::login);
            response.insert(response.end(),ttd.begin(),ttd.end());
            response.emplace_back(0x01);
            std::array<unsigned char,4> tmp;
            int_to_buffer_bigendian(version,tmp);
            response.insert(response.end(),tmp.begin(),tmp.end());
            break;
        }
        case 0x02:{
            if(msg.size()<minsigp+5){
                return;
            }
            uint32_t tmp;
            buffer_to_int_bigendian(std::array<unsigned char,4>({msg[10],msg[11],msg[12],msg[13]}),tmp);
            if(tmp==version){
                response.emplace_back(id_table::server::login);
                response.insert(response.end(),ttd.begin(),ttd.end());
                response.emplace_back(0x02);
                uint64_t tokenplayer=loginplayer(ip,port);
                std::array<unsigned char,8> tmpr;
                ll_to_buffer_bigendian(tokenplayer,tmpr);
                response.insert(response.end(),tmpr.begin(),tmpr.end());
            }
            break;
        }
        case 0x03:{
            if(msg.size()<minsigp+5){
                return;
            }
            uint64_t tmp;
            buffer_to_ll_bigendian(const std::array<unsigned char,8>({msg[10],msg[11],msg[12],msg[13],msg[14],msg[15],msg[16],msg[17]}),tmp);
            if(checktoken(tmp)){
                //give map buffer
                nlohmann::json mapdata;
                defaultMapGenerator(mapdata);
                std::vector<unsigned char> mapcompiled=mapCompiler(mapdata);
                response.emplace_back(id_table::server::login);
                response.insert(response.end(),ttd.begin(),ttd.end());
                response.emplace_back(0x03);
                if(mapcompiled.size()>225){
                    //dibagi
                    unsigned char chunkpart=msg[18];
                    unsigned char chunksize=(mapcompiled.size()/225)+1;
                    response.emplace_back(chunksize);
                    response.emplace_back(chunkpart);
                    response.insert(response.end(),mapcompiled.begin()+chunkpart*225,mapcompiled.begin()+std::min(static_cast<size_t>((chunkpart+1)*225),mapcompiled.size()));
                }
                
            }
            break;
        }
    default:
        break;
    }
    
   
    
    

}
std::vector<unsigned char> headerParser(const std::vector<unsigned char>& msg, std::string& ip, std::string& port) {
    std::vector<unsigned char> response;
    // Parse the header from the message
    switch (msg[0]) {
        case id_table::client::cek_koneksi:
            cekKoneksi(msg, response, ip, port);
            break;
        case id_table::client::pralogin:
            pralogin(msg,response,ip,port);
            break;
        case id_table::client::login:
            login(msg,response,ip,port);
            break;
        case id_table::client::keluar:
            break;
        case id_table::client::req:
            // Handle request
            break;
        case id_table::client::kirim:
            // Handle send
            break;
        case id_table::client::gagal:
            // Handle failed
            break;
        default:
            std::cout << "Unknown message type: " << static_cast<int>(msg[0]) << std::endl;
            return response; // Return empty response for unknown message
    }
    // Update the ip and port variables as needed
    return response;
}