#pragma once
#include<vector>
#include<string>
#include <maploader/typeandconst.hpp>
#include<json.hpp>
//urutan{air,grass,belowgrass}
std::vector<unsigned char> defaultChunkOrderGenerator(const std::vector<unsigned char>& component){
    std::vector<unsigned char> result;
    if (component.size()<3)
    {
        return {};
    }
    
    
    for(size_t a=0;a<chunkheight;a++){
        for(size_t b=0;b<chunklen;b++){
            if (a < 40)
            {
                result.push_back(0);
                for (size_t i = 0; i < component[0]; i++)
                {
                    result.emplace_back(0);
                }
                
            }else if(a < 50){
                result.push_back(1);
                for (size_t i = 0; i < component[1]; i++)
                {
                    result.emplace_back(0);
                }
            }else{
                result.push_back(2);
            }
        }
    }
    return result;
}
void defaultMapGenerator(nlohmann::json& map){
    map = nlohmann::json::parse(R"({
        "idTile": ["gps:air", "gps:grass", "gps:stone"],
        "idComponent":[0,0,0],
        "idLenght": 1,
        "idOrder": []
    })");
    map["idOrder"]=defaultChunkOrderGenerator(map["idComponent"]);
}