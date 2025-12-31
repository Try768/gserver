#include <gserfunction/filefunc.hpp>
#include <gserFunction/caster.h>
#include <fstream>
#include <iostream>
#include<json.hpp>
#include<miniz.h>
constexpr unsigned char chunklen=16;
std::vector<unsigned char> mapCompiler(nlohmann::json jsonmapperchunk){
    try{
         if (!jsonmapperchunk.contains("idTile") ||
            !jsonmapperchunk.contains("idLenght") ||
            !jsonmapperchunk.contains("idorder")) {
            std::cerr << "Error: Missing required fields in JSON mapper chunk.\n";
            return {};
        }
    nlohmann::json& idtile=jsonmapperchunk["idTile"];//namespace
    nlohmann::json& idlength=jsonmapperchunk["idLenght"];//uchar max and min(1,2,4,8)
    nlohmann::json& idorder=jsonmapperchunk["idorder"];//{tile,var_component}its a number
   
    std::vector<unsigned char> buffer;
    // Add idlength to buffer
    buffer.emplace_back(static_cast<unsigned char>(idlength.get<int>()));
    // Add idtile to buffer
    if(!idtile.is_array()){
    std::cerr << "Error: idTile is not array\n";
        return {};
    }
    std::array<unsigned char,8> tmpll;
    ll_to_buffer_bigendian(idtile.size(),tmpll);
    buffer.insert(buffer.end(),tmpll.begin(),tmpll.end());
    for(size_t a=0;a<idtile.size();a++){
        std::array<unsigned char,4> tmp;
        int_to_buffer_bigendian((uint32_t)idtile[a].size(),tmp);
        buffer.insert(buffer.end(),tmp.begin(),tmp.end());
        buffer.insert(buffer.end(),idtile[a].get_ref<const std::string&>().begin(),idtile[a].get_ref<const std::string&>().end());
    }
    // Add idorder to buffer
    std::array<unsigned char,8> tmpord;
    ll_to_buffer_bigendian(idorder.size(),tmpord);
    buffer.insert(buffer.end(),tmpord.begin(),tmpord.end());
    for(size_t a=0;a<idorder.size();a++){
        std::array<unsigned char,8> tmp;
        ll_to_buffer_bigendian(idorder[a].get_ref<const uint64_t&>(),tmp);
        buffer.insert(buffer.end(),tmp.begin(),tmp.end());
    }
    return buffer;
    }catch(const nlohmann::json::exception& e){
        std::cerr << "Error at map compiler: " << e.what() << "\n";
        return {};
    }
}


void Tfile::setup() {
            const std::string defaultConfig=R"({
    "capacity":10,
    "debug":false,
    "mainMap":"lobby.tmg"
})";
    // Initialize any necessary resources or configurations for file operations
    std::ifstream file("config.json");
    if(!file.is_open()) {
        // Handle error, e.g., log it or throw an exception
        std::cout << "Config file not found, creating a default one.\n";

        writeFile("config.json", defaultConfig);

        return;
    }
    else if (!file) {
        // Handle error, e.g., log it or throw an exception
        std::cout<<"file not found(maybe a permission problem)\n";
        return;
    }
    nlohmann::json konfigurasi;
    try{
        konfigurasi=nlohmann::json::parse(readFile("config.json"));
    }catch(const nlohmann::json::parse_error& err){
        konfigurasi=nlohmann::json::parse(defaultConfig);
        std::cout<<"config.json failed to load:"<<err.what()<<"\nusing default config";

    }
    std::ifstream file(konfigurasi["mainMap"]);
    if(!file.is_open()) {
        // Handle error, e.g., log it or throw an exception
        std::cout << "Config file not found, creating a default one.\n";
        const std::string defaultmap="{\ncapacity:10,\ndebug:false,\nmainMap:lobby.tmg\n}";
        writeFile("lobby.tmg", defaultmap);

        return;
    }
    else if (!file) {
        // Handle error, e.g., log it or throw an exception
        std::cout<<"file not found(mungkin masalah izin)\n";
        return;
    }
    
}