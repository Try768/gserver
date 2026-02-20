#pragma once
#include "playermanager.hpp"
class chunkmap:public checksumparent
{
    public:
    static constexpr unsigned int sizex=16;
    static constexpr unsigned int sizey=16;
    friend class EntityManager;
    friend class PlayerManager;
    private:
    unsigned char perm_len;
    using ID=unsigned long long;
    using enc=unsigned long long;
    
    //indeks and count
    IDMaker<unsigned char> tileinchunk;
    std::unordered_map<std::string,std::pair<unsigned char,unsigned short>> tileCounterId;
    std::unordered_map<unsigned char,std::string> tileId;
    void internal_swap_Tile(const std::string& before,const std::string& after);
    //indeks and count
    //static std::unordered_map<std::string,std::unordered_map<ID,EntityData*>> entitybystring;
    std::array<std::pair<TileData,unsigned char>,sizex*sizey> tilesBuffer;
    public:
    bool dirty;
    
    inline void setTile(size_t x,size_t y,const Tile& newTile){
        if(y>=16||x>=16)return;
        auto& tile=tilesBuffer[(x)+(y*sizex)];
        internal_swap_Tile(tileId.at(tile.second),newTile.get_name());
        tile=std::pair<TileData,unsigned char>(newTile.data,tileCounterId.at(newTile.name).first);
    }
    inline static const std::array<unsigned char,8> chunkSignature ={3,5,7,255,12,34,56,78};
    //can throw error
    //max 15,min 0
    inline const Tile getTile(unsigned short x,unsigned short y){
        return Tile(tilesBuffer[x+(y*sizex)].first,tileId.at(tilesBuffer[x*y].second));
    }
    inline const std::unordered_map<unsigned char,std::string>& getAllTypeTile()const{
        return tileId;
    }
    std::vector<unsigned char> dump()const;
    void dump_ref(std::vector<unsigned char>& buff)const;
    std::vector<unsigned char> bufferdump()override;
    void buffer_refdump(std::vector<unsigned char>& buffer)const;
    static bool is_Databuffer_valid(const std::vector<unsigned char>& buffer,size_t& offset);
    static bool is_buffer_valid(const std::vector<unsigned char>& buffer,size_t& offset);
    void parse_buffer(const std::vector<unsigned char>& buffer,size_t& offset);
    void parse(const std::vector<unsigned char>& buffer,size_t& offset);
    chunkmap(const std::vector<unsigned char>& buffer,size_t& offset){
        parse(buffer,offset);
    }
    chunkmap(){
        std::cout<<"creating a new chunk\n";
        dirty=true;
    };
};
/*inline bool delTileType(unsigned char indeks){
    //    if(tilelistId.size()<indeks)return 0;
    //    tilelistId.erase(tilelistId.begin()+indeks);
    //    for(auto& tilebuff:tilesBuffer){
    //        if(tilebuff.getIdinChunk()>indeks){
    //            tilebuff.setIdinChunk(tilebuff.getIdinChunk()-1);
    //        }else if(tilebuff.getIdinChunk()==indeks){
    //            tilebuff.setIdinChunk(0);
    //        }
    //    }
    //    return 1;
    //}
    //it will sort all indeks
    //inline bool delTileTypes(std::vector<unsigned char>& multiIndeks){
    //    for(auto indeks:multiIndeks){
    //        if(tilelistId.size()<indeks)return false;
    //    }
    //    for(auto indeks:multiIndeks){
    //        tilelistId.erase(tilelistId.begin()+indeks);
    //    }
    //    cssortchar(multiIndeks);
    //    for(auto& tilebuff:tilesBuffer){
    //        for(auto& indeks:multiIndeks){
    //            if(tilebuff.getIdinChunk()>indeks)
    //            tilebuff.setIdinChunk(tilebuff.getIdinChunk()-1);
    //            else if(tilebuff.getIdinChunk()==indeks){
    //                tilebuff.setIdinChunk(0);
    //            }
    //        }
    //    }
    //}
    //inline bool addTileTypes(const std::vector<tilecomponent>& tiles){
    //    if((tilelistId.size()+tiles.size())>256)return 0;
    //    tilelistId.insert(tilelistId.begin(),tiles.begin(),tiles.end());
    //    return true;
    //}
    //inline bool addTileTypes(const std::vector<tilecomponent>::const_iterator awal,const std::vector<tilecomponent>::const_iterator akhir){
    //    if((tilelistId.size()+(akhir-awal))>256)return 0;
    //    tilelistId.insert(tilelistId.begin(),awal,akhir);
    //    return true;
    }*/
//inline bool addTileType(const tilecomponent& tile){
    //    if(tilelistId.size()==256)return 0;
    //    tilelistId.push_back(tile);
    //    return true;
    //}
    //inline const tilecomponent& getTileType(unsigned char indeks){
    //    return tilelistId[indeks];
    //}