#pragma once
#include "playermanager.hpp"
#include "core/tile/tile.hpp"
class chunkmap:public checksumparent
{
    public:
    static constexpr unsigned int sizex=16;
    static constexpr unsigned int sizey=16;
    friend class EntityManager;
    friend class PlayerManager;
    enum Flag:unsigned char{
        load=0b1
    };
    bool is_load()const{return flag&Flag::load;}
    void toggle_load(){flag=flag^Flag::load;}
    private:
    Dimension& dimension;
    Registry& reg;
    unsigned char perm_len;
    using ID=unsigned long long;
    using enc=unsigned long long;
    unsigned char flag; 
    //indeks and count
    IDMaker<unsigned char> tileinchunk;
    std::unordered_map<std::string,std::pair<unsigned char,unsigned short>> tileCounterId;
    std::unordered_map<unsigned char,std::string> tileId;
    void internal_swap_Tile(const std::string& before,const std::string& after);
    //indeks and count
    //static std::unordered_map<std::string,std::unordered_map<ID,EntityData*>> entitybystring;
    struct __Tile{TileData data;unsigned char id;};
    using time_point=std::chrono::steady_clock::time_point;
    class SimulationTile{
        std::unordered_set<unsigned char> data;
        public:
        void add(unsigned char indeksTile){
            data.emplace(indeksTile);
        }
        void remove(unsigned char indeksTile){
            data.erase(indeksTile);
        }
        void simulate(std::array<__Tile,sizex*sizey>& buffer,
            Dimension& dimension,Registry& reg,chunkmap::time_point time_pivot);
    };
    SimulationTile tilesimulator;
    std::array<__Tile,sizex*sizey> tilesBuffer;
    public:
    void simulate(Registry& reg,time_point time_pivot){tilesimulator.simulate(tilesBuffer,dimension,reg,time_pivot);}
    bool dirty;
    
    inline void setTile(size_t x,size_t y,const Tile& newTile){
        if(y>=16||x>=16)return;
        auto& tile=tilesBuffer[(x)+(y*sizex)];
        internal_swap_Tile(tileId.at(tile.id),newTile.get_name());
        tile=__Tile{newTile.data,tileCounterId.at(newTile.name).first};
    }
    inline static const std::array<unsigned char,8> chunkSignature ={3,5,7,255,12,34,56,78};
    //can throw error
    //max 15,min 0
    inline const Tile getTile(unsigned short x,unsigned short y){
        return Tile(tilesBuffer[x+(y*sizex)].data);
    }
    inline const std::unordered_map<unsigned char,std::string>& getAllTypeTile()const{
        return tileId;
    }
    std::vector<unsigned char> dump()const;
    void dump_ref(std::vector<unsigned char>& buff)const;
    std::vector<unsigned char> bufferdump()const;
    void buffer_refdump(std::vector<unsigned char>& buffer)const;
    static size_t is_Databuffer_valid(const std::vector<unsigned char>& buffer,size_t offset);
    static size_t is_buffer_valid(const std::vector<unsigned char>& buffer,size_t offset);
    void parse_buffer(const std::vector<unsigned char>& buffer,size_t& offset);
    void parse(const std::vector<unsigned char>& buffer,size_t& offset);
    chunkmap(const std::vector<unsigned char>& buffer,size_t& offset,Dimension& dimension,Registry& reg):dimension(dimension),reg(reg){
        parse(buffer,offset);
    }
    chunkmap(Dimension& dimension,Registry& reg):dimension(dimension),reg(reg){
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