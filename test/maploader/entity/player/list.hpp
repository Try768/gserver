#pragma once 
#include "../../internal/datatype.hpp"
#include "../../common.hpp"
class playerlist:public Coord_manager{
    private:
    std::string playerName;
    bool nameAproved;
    public:
    static std::unordered_set<std::string> usedPlayername;
    void dump(std::vector<unsigned char> keluaran){
        string_short_to_buffer_bigendian(playerName,keluaran);
        this->co_dump(keluaran);
    }
    static bool is_buffer_valid(const std::vector<unsigned char>& buffer,size_t& offset){
        size_t length;
        if(!zt::Internal::parse::checkStringBigendian(buffer,offset,length))return false;
        offset+=length;
        if(!Coord_manager::is_co_valid(buffer,offset))return false;
        return true;
    }
    //this function can throw error
    void parse(const std::vector<unsigned char>& buffer,size_t& offset){
        buffer_bigendian_to_string_short(buffer,offset,playerName);
        this->co_parse(buffer,offset);
    }
    playerlist(std::vector<unsigned char>& buffer,size_t& offset){
        parse(buffer,offset);
    }
    playerlist()=default;
    playerlist(Coord<unsigned int>lokal,
        Coord<long long> global,std::string name):playerName(name),Coord_manager(global){
        this->lokal=lokal;
        
        this->nameAproved=false;
    }
    inline std::string getname()const{
        return playerName;
    }
    bool setnametest(){
        //std::cout<<this->playerName<<std::endl;
        if(playerlist::usedPlayername.find(this->playerName)==playerlist::usedPlayername.end()){
            playerlist::usedPlayername.insert(this->playerName);
            this->nameAproved=true;
            return true;
        }
        return false;
    }
    bool setname(std::string nama){
       
        if(playerlist::usedPlayername.find(nama)==playerlist::usedPlayername.end()){
            if(this->nameAproved){
                playerlist::usedPlayername.erase(this->playerName);
            }
            playerlist::usedPlayername.insert(nama);
            this->playerName=nama;
            return true;
        }
        return false;
    }
};
std::unordered_set<std::string> playerlist::usedPlayername={};