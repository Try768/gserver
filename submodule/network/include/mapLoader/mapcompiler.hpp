#ifndef MAPCOMPILERHPP
#define MAPCOMPILERHPP
//constexpr unsigned char chunklen=16;
#include <gserFunction/caster.h>
#include <json.hpp>
#include <iostream>
#include <fstream>
#include <limits>
#include <maploader/typeandconst.hpp>
std::vector<unsigned char> mapCompiler(nlohmann::json jsonmapperchunk){
    try{
         if (!jsonmapperchunk.contains("idTile") ||
            !jsonmapperchunk.contains("idComponent") ||
            !jsonmapperchunk.contains("idOrder")) {
            std::cerr << "Error: Missing required fields in JSON mapper chunk.\n";
            return {};
        }
    nlohmann::json& idtile=jsonmapperchunk["idTile"];//namespace
    nlohmann::json& idcomponent=jsonmapperchunk["idComponent"];//uchar max and min(1,2,4,8)
    nlohmann::json& idorder=jsonmapperchunk["idOrder"];//{tile,var_component}its a number
   
    std::vector<unsigned char> buffer;
    // Add idlength to buffer
    //buffer.emplace_back(static_cast<unsigned char>(idlength.get<int>()));
    uint8_t idlength = 1;
    if(idtile.size()<=std::numeric_limits<unsigned char>::max()){
        idlength = 1;
    }else if(idtile.size()<=std::numeric_limits<uint16_t>::max()){
        idlength = 2;
    }else if(idtile.size()<=std::numeric_limits<uint32_t>::max()){
        idlength = 4;
    }else if(idtile.size()<=std::numeric_limits<uint64_t>::max()){
        idlength = 8;
    }else{
        std::cerr << "Error: idTile size exceeds maximum limit\n";
        return {};
    }
    buffer.emplace_back(idlength);
    // Add idtile to buffer
    if((!idtile.is_array())||(!idcomponent.is_array())){
    std::cerr << "Error: idTile is not array\n";
        return {};
    }
    std::array<unsigned char,8> tmpll;
    ll_to_buffer_bigendian(idtile.size(),tmpll);
    buffer.insert(buffer.end(),tmpll.begin(),tmpll.end());
    size_t cmp=idcomponent.size();
    for (size_t i = cmp; i < idtile.size(); i++)
    {
        idcomponent.emplace_back(0);
    }
    
    for(size_t a=0;a<idtile.size();a++){
        std::array<unsigned char,4> tmp;
        int_to_buffer_bigendian((uint32_t)idtile[a].get_ref<const std::string&>().size(),tmp);
        buffer.insert(buffer.end(),tmp.begin(),tmp.end());
        buffer.insert(buffer.end(),idtile[a].get_ref<const std::string&>().begin(),idtile[a].get_ref<const std::string&>().end());
        buffer.emplace_back(static_cast<unsigned char>(idcomponent[a].get<int>()));
    }
    // Add idorder to buffer
    std::array<unsigned char,8> tmpord;
    ll_to_buffer_bigendian(idorder.size(),tmpord);
    buffer.insert(buffer.end(),tmpord.begin(),tmpord.end());
    for(size_t a=0;a<idorder.size();a++){
        std::array<unsigned char,8> tmp;
        ll_to_buffer_bigendian(idorder[a].get<const uint64_t>(),tmp);
        buffer.insert(buffer.end(),(tmp.end()-idlength),tmp.end());
        for (size_t i = 0; i < idcomponent[idorder[a].get<size_t>()].get<size_t>(); i++)
        {
            ++a; // maju ke component berikutnya
            if (a < idorder.size()) {
                buffer.emplace_back(static_cast<unsigned char>(idorder[a].get<int>()));
            }
        }
        
    }
    return buffer;
    }catch(const nlohmann::json::exception& e){
        std::cerr << "Error at map compiler: " << e.what() << "\n";
        return {};
    }
}

#endif