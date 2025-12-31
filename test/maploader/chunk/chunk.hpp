#pragma once
#include "../tile/list.hpp"
#include "../register/register.hpp"
#include "../entity/list.hpp"
class chunkmap:public checksumparent
{
    public:
    static constexpr unsigned int sizex=16;
    static constexpr unsigned int sizey=16;
    private:
    std::vector<tilecomponent> listId;
    std::array<tilelist,sizex*sizey> tilesBuffer;
    public:
    bool dirty;
    std::vector<entitylist> entitiesBuffer;
    inline static const std::array<unsigned char,8> chunkSignature ={3,5,7,255,12,34,56,78};
    //can throw error
    inline const tilelist& getTile(size_t x,size_t y)const{
        return tilesBuffer[(x)+(y*sizex)];
    }
    inline void setTile(size_t x,size_t y,const tilelist& newTile){
        if(y>=16||x>=16)return;
        tilesBuffer[(x)+(y*sizex)]=newTile;
    }
    inline bool addTileType(const tilecomponent& tile){
        if(listId.size()==256)return 0;
        listId.push_back(tile);
        return true;
    }
    inline tilecomponent getTileType(unsigned char indeks){
        return listId[indeks];
    }
    inline bool delTileType(unsigned char indeks){
        if(listId.size()<indeks)return 0;
        listId.erase(listId.begin()+indeks);
        for(auto& tilebuff:tilesBuffer){
            if(tilebuff.getIdinChunk()>indeks){
                tilebuff.setIdinChunk(tilebuff.getIdinChunk()-1);
            }
        }
        return 1;
    }
    //it will sort all indeks
    inline bool delTileTypes(std::vector<unsigned char>& multiIndeks){
        for(auto indeks:multiIndeks){
            if(listId.size()<indeks)return false;
        }
        for(auto indeks:multiIndeks){
            listId.erase(listId.begin()+indeks);
        }
        cssortchar(multiIndeks);
        for(auto& tilebuff:tilesBuffer){
            for(auto& indeks:multiIndeks){
                if(tilebuff.getIdinChunk()<indeks)break;
                tilebuff.setIdinChunk(tilebuff.getIdinChunk()-1);
            }
        }
    }
    inline bool addTileTypes(const std::vector<tilecomponent>& tiles){
        if((listId.size()+tiles.size())>256)return 0;
        listId.insert(listId.begin(),tiles.begin(),tiles.end());
        return true;
    }
    inline bool addTileTypes(const std::vector<tilecomponent>::const_iterator awal,const std::vector<tilecomponent>::const_iterator akhir){
        if((listId.size()+(akhir-awal))>256)return 0;
        listId.insert(listId.begin(),awal,akhir);
        return true;
    }
    std::vector<unsigned char> dump()const{
        std::vector<unsigned char> buff;
        buff.insert(buff.end(),chunkSignature.begin(),chunkSignature.end());
        std::vector<unsigned char> tmp;tmp.reserve(256*2+16);
        buffer_refdump(tmp);
        to_buffer_bigendian<unsigned long long>(getceksum(tmp),buff);
        return buff;
    }
    void dump_ref(std::vector<unsigned char>& buff)const{
        buff.insert(buff.end(),chunkSignature.begin(),chunkSignature.end());
        std::vector<unsigned char> tmp;tmp.reserve(256*2+16);
        buffer_refdump(tmp);
        to_buffer_bigendian<unsigned long long>(getceksum(tmp),buff);
        array_to_buffer_bigendian(tmp,buff);
    }
    std::vector<unsigned char> bufferdump()override{
        std::vector<unsigned char> keluaran;
        buffer_refdump(keluaran);
        return keluaran;
    }
    void buffer_refdump(std::vector<unsigned char>& buffer)const{
        
        to_buffer_bigendian<unsigned short>(listId.size(),buffer);
        for (size_t i = 0; i < listId.size(); i++)
        {
           listId[i].ref_dump(buffer);
        }
        to_buffer_bigendian<unsigned short>(0x100,buffer);
        for (size_t i = 0; i < sizex*sizey; i++)
        {
            tilesBuffer[i].ref_dump(buffer);
        }
        dynamic_to_buffer_bigendian(entitiesBuffer.size(),buffer);
        for(auto bufentity:entitiesBuffer){
            bufentity.ref_dump(buffer);
        }
        
    }
    static bool is_Databuffer_valid(const std::vector<unsigned char>& buffer,size_t offset){
        using namespace zt::Internal;
        unsigned short listIdSize;
        unsigned char tmpc;
        if(!parse::checkPrimitiveBigendian<unsigned short>(buffer,offset))return false;
        buffer_bigendian_to<unsigned short>(buffer,offset,listIdSize);
        {
            for (size_t i = 0; i < listIdSize; i++)
            {
                if(!tilecomponent::is_buffer_valid(buffer,offset))return false;
            }
        }
        unsigned short tileSize;
        if(!parse::checkPrimitiveBigendian<unsigned short>(buffer,offset))return false;
        buffer_bigendian_to<unsigned short>(buffer,offset,tileSize);
        if(tileSize!=0x100)return false;
        {
            for (size_t i = 0; i < tileSize; i++)
            {
                if(!tilecomponent::is_buffer_valid(buffer,offset))return false;
            }
        }
        unsigned long long entitySize;
        if(!parse::checkDynamicBigendian(buffer,offset,tmpc))return false;
        offset--;
        buffer_bigendian_to_dynamic(buffer,offset,entitySize);
        if(entitySize>1024)return false;
        {
            for (size_t i = 0; i < entitySize; i++)
            {
                if(!entitylist::is_buffer_valid(buffer,offset))return false;
            }
        }
        return true;
    }
    static bool is_buffer_valid(const std::vector<unsigned char>& buffer,size_t offset){
        using namespace zt::Internal;
        if(buffer.size()<offset+chunkSignature.size()+sizeof(unsigned long long))return false;
        for (size_t i = 0; i < chunkSignature.size(); i++)
        {
            if(buffer[offset+i]!=chunkSignature[i])return false;
        }
        offset+=chunkSignature.size();
        unsigned long long checksum;
        buffer_bigendian_to<unsigned long long>(buffer,offset,checksum);
        std::vector<unsigned char> temp;
        unsigned long long arrlength;
        parse::checkArrayBigendian(buffer,offset,arrlength);
        if(buffer.size()<offset+arrlength)return false;
        if(!verifycheksum(buffer.begin()+offset,buffer.begin()+offset+arrlength,checksum))return false;
        is_Databuffer_valid(buffer,offset);
        return true;
    }
    void parse(const std::vector<unsigned char>& buffer,size_t& offset){
        if(buffer.size()<offset+chunkSignature.size()+sizeof(unsigned long long))throw std::exception("error:chunk parsing invalid size");
        for (size_t i = 0; i < chunkSignature.size(); i++)
        {
            if(buffer[offset+i]!=chunkSignature[i])throw std::exception("error:chunk parsing invalid signature");
        }
        offset+=chunkSignature.size();
        unsigned long long checksum;
        unsigned long long arrlength;
        unsigned short arrlengthc;
        buffer_bigendian_to<unsigned long long>(buffer,offset,checksum);
        zt::Internal::parse::checkArrayBigendian(buffer,offset,arrlength);
        if(!verifycheksum(buffer.begin()+offset,buffer.begin()+offset+arrlength,checksum))throw std::exception("error:chunk parsing invalid checksum");
        buffer_bigendian_to<unsigned short>(buffer,offset,arrlengthc);
        listId.clear();listId.reserve(arrlengthc);
        {
            
            for (size_t i = 0; i < arrlengthc; i++)
            {
                listId.emplace_back(buffer,offset);
            }
        }
        buffer_bigendian_to(buffer,offset,arrlengthc);
        if(arrlength!=sizex*sizey) throw std::exception("error:jumlah tile buffer tak sesuai");
        for (size_t i = 0; i < arrlengthc; i++)
        {
            tilesBuffer[i].parse(buffer,offset);
        }
        buffer_bigendian_to_dynamic(buffer,offset,arrlength);
        entitiesBuffer.clear();entitiesBuffer.reserve(arrlength);
            for (size_t i = 0; i < arrlength; i++)
            {
                entitiesBuffer.emplace_back(buffer,offset);
            }
    }
    chunkmap(const std::vector<unsigned char>& buffer,size_t& offset){
        parse(buffer,offset);
    }
    chunkmap(){
        dirty=true;
    };
};
