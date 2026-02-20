#pragma once
#include "../common.hpp"
#include "../json.hpp"
class GeneratorOption{
   
    private:
    unsigned int seed;
    bool useGenerator;
    public:
    GeneratorOption(bool useGen,unsigned int seed=0):seed(seed),useGenerator(useGen){}
    std::vector<unsigned char> dump(){
        std::vector<unsigned char> keluaran;
        to_buffer_bigendian<unsigned int>(seed,keluaran);
        keluaran.push_back(useGenerator?1:0);
        return keluaran;
    }
    GeneratorOption(){
        seed=0;
        useGenerator=true;
    }
    void setSeed(unsigned int newseed){
        seed=newseed;
    }
    void setUseGenerator(bool useGen){
        useGenerator=useGen;
    }
    unsigned int getSeed()const{
        return seed;
    }
    bool getUseGenerator()const{
        return useGenerator;
    }
};
class MapOption{
    public:
    std::string mapName;
    bool beta;
    int version;
    std::string author;
    unsigned int maxPlayers;
    GeneratorOption genOption;
    
   // #define getMapopsi(x) const auto& mapGet_##x()const{return x;}
    //#undef getMapopsi
    //#define setMapopsi(x,type) void mapSet_##x(type newx){x=newx;}
    //#undef setMapopsi
    std::vector<unsigned char> dump(){
        std::vector<unsigned char> keluaran;
        string_short_to_buffer_bigendian(mapName,keluaran);
        keluaran.push_back(beta?1:0);
        to_buffer_bigendian<int>(version,keluaran);
        string_short_to_buffer_bigendian(author,keluaran);
        to_buffer_bigendian<unsigned int>(maxPlayers,keluaran);
        auto genBuffer=genOption.dump();
        array_to_buffer_bigendian(genBuffer,keluaran);
        return keluaran;
    }
    bool getjson(nlohmann::json hmm){
        if(hmm.contains("map_name") && hmm.contains("author") && hmm.contains("version") && hmm.contains("max_player") && hmm.contains("is_beta") && hmm.contains("gen_opsi")){
            mapName=hmm["map_name"];
            author=hmm["author"];
            version=hmm["version"];
            maxPlayers=hmm["max_player"];
            beta=hmm["is_beta"];
            if(hmm["gen_opsi"].contains("seed")){
                genOption.setSeed(hmm["gen_opsi"]["seed"]);
            }
            if(hmm["gen_opsi"].contains("is_using_generator")){
                genOption.setUseGenerator(hmm["gen_opsi"]["is_using_generator"]);
            }
            return true;
        }
        return false;
    }
    MapOption(std::string name,bool betaMode,int ver,std::string auth,unsigned int maxPly,GeneratorOption genOpt):mapName(name),beta(betaMode),version(ver),author(auth),maxPlayers(maxPly),genOption(genOpt){}
    MapOption(){
        mapName="default";
        beta=false;
        version=1;
        author="admin";
        maxPlayers=10;
        genOption=GeneratorOption();
    }
};
