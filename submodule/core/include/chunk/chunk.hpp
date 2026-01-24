#pragma once
#include "../register/register.hpp"
#include "../tile/list.hpp"
//#include "../entity/list.hpp"
#include <deque>
#include "../entity/entity.hpp"
class chunkmap:public checksumparent
{
    public:
    static constexpr unsigned int sizex=16;
    static constexpr unsigned int sizey=16;
    
    private:
    unsigned char perm_len;
    using ID=unsigned long long;
    using enc=unsigned long long;
    template<class IDT>
    class IDMaker{
        private:
        friend class chunkmap;
        std::unordered_set<IDT> free_ids;
        IDT next=0;
        IDT back=0;
        IDT count=0;
        bool getID(IDT& id){
            if(!free_ids.empty()){
                auto itc=free_ids.begin();
                id=*itc;
                free_ids.erase(itc);
                next++;count++;
                return true;
            }
            if(count!=ULLONG_MAX){
                id=next;next++;count++;
                return true;
            }return false;
        }
        void trimed(){
            while (true)
            {
                auto itc =free_ids.find(back);
                if(itc==free_ids.end()){
                    free_ids.erase(itc);
                    back++;
                }else{
                    break;
                }
            }
            
        }
        void destroyID(const IDT id){
            if(count==0)return;
            if(id==back){
                back++;
                count--;
            }else if(back>next){
                if(id>back||id<next){
                    free_ids.insert(id);
                    count--;
                }
            }else{
                if(id>back&&id<next){
                    free_ids.insert(id);
                    count--;
                }
            }
        }
    };
    //indeks and count
    IDMaker<unsigned char> tileinchunk;
    std::unordered_map<std::string,std::pair<unsigned char,unsigned short>> tileCounterId;
    std::unordered_map<unsigned char,std::string> tileId;
    void internal_swap_Tile(const std::string& before,const std::string& after){
        auto itc =tileCounterId.find(before);
        if(itc!=tileCounterId.end()){
            if(itc->second.second==0){
                tileinchunk.destroyID(itc->second.first);
                tileId.erase(itc->second.first);
                tileCounterId.erase(before);
            }else{
                itc->second.second--;
            }
        }
        itc=tileCounterId.find(after);
        if(itc!=tileCounterId.end()){
            itc->second.second++;
        }else{
            unsigned char temid;
            if(!tileinchunk.getID(temid))throw std::exception("some math mistake on tile id");
            tileCounterId[after]=std::pair<unsigned char,unsigned short>(temid,0);
            tileId[temid]=after;
        }
    }
    //indeks and count
    //static std::unordered_map<std::string,std::unordered_map<ID,EntityData*>> entitybystring;
    std::array<std::pair<TileData,unsigned char>,sizex*sizey> tilesBuffer;
    public:
    class EntityManager{
        private:
        static IDMaker<unsigned long long> entityid;
        static std::unordered_map<ID,Entity> entitybyID;
        public:
        ID addEntity(Entity&& entity){
            ID tempid;
            if(!entityid.getID(tempid))throw std::exception("alright alright thats enough entities");
            const std::string& entityType=entity.get_name();
            entitybyID.try_emplace(tempid,entity);
            return tempid;
        }
        
        inline static const Entity& getEntity(ID& entityid){
            return entitybyID.at(entityid);
        }
        inline static decltype(entitybyID)::const_iterator findEntityId(ID identity){
            return entitybyID.find(identity);
        }
        
        inline static bool setEntity(decltype(entitybyID)::const_iterator itc,Entity&& entity){
            if(itc==entitybyID.end())return false;
            auto it= entitybyID.find(itc->first);
            it->second=entity;
            return true;
        };
        inline static bool setEntity(decltype(entitybyID)::const_iterator itc,const Entity& entity){
            return setEntity(itc,entity);
        }
        inline static bool delEntity(ID id){
            if(entitybyID.count(id)){
                entitybyID.erase(id);
                return true;
            }return false;
        }
        inline ID addEntity(const Entity& entity){
            addEntity(Entity(entity));
        }
        inline std::vector<ID> getentityIDinchunk();
    };
    inline void setTile(size_t x,size_t y,const Tile& newTile){
        if(y>=16||x>=16)return;
        auto& tile=tilesBuffer[(x)+(y*sizex)];
        internal_swap_Tile(tileId.at(tile.second),newTile.get_name());
        tile=std::pair<TileData,unsigned char>(newTile.data,tileCounterId.at(newTile.name).first);
    }
    bool dirty;
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