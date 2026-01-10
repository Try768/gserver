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
    std::vector<tilecomponent> tilelistId;
    //std::vector<entitycomponent> entityTypeList;
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
        if(y<=16||x<=16)return;
        tilesBuffer[(x)+(y*sizex)]=newTile;
    }
    inline bool addTileType(const tilecomponent& tile){
        if(tilelistId.size()==256)return 0;
        tilelistId.push_back(tile);
        return true;
    }
    inline const tilecomponent& getTileType(unsigned char indeks){
        return tilelistId[indeks];
    }
    inline size_t getTileTypeCount()const{
        return tilelistId.size();
    }
    inline bool delTileType(unsigned char indeks){
        if(tilelistId.size()<indeks)return 0;
        tilelistId.erase(tilelistId.begin()+indeks);
        for(auto& tilebuff:tilesBuffer){
            if(tilebuff.getIdinChunk()>indeks){
                tilebuff.setIdinChunk(tilebuff.getIdinChunk()-1);
            }else if(tilebuff.getIdinChunk()==indeks){
                tilebuff.setIdinChunk(0);
            }
        }
        return 1;
    }
    //it will sort all indeks
    inline bool delTileTypes(std::vector<unsigned char>& multiIndeks){
        for(auto indeks:multiIndeks){
            if(tilelistId.size()<indeks)return false;
        }
        for(auto indeks:multiIndeks){
            tilelistId.erase(tilelistId.begin()+indeks);
        }
        cssortchar(multiIndeks);
        for(auto& tilebuff:tilesBuffer){
            for(auto& indeks:multiIndeks){
                if(tilebuff.getIdinChunk()>indeks)
                tilebuff.setIdinChunk(tilebuff.getIdinChunk()-1);
                else if(tilebuff.getIdinChunk()==indeks){
                    tilebuff.setIdinChunk(0);
                }
            }
        }
    }
    inline bool addTileTypes(const std::vector<tilecomponent>& tiles){
        if((tilelistId.size()+tiles.size())>256)return 0;
        tilelistId.insert(tilelistId.begin(),tiles.begin(),tiles.end());
        return true;
    }
    inline bool addTileTypes(const std::vector<tilecomponent>::const_iterator awal,const std::vector<tilecomponent>::const_iterator akhir){
        if((tilelistId.size()+(akhir-awal))>256)return 0;
        tilelistId.insert(tilelistId.begin(),awal,akhir);
        return true;
    }
    std::vector<unsigned char> dump()const{
        std::vector<unsigned char> buff;
        buff.insert(buff.end(),chunkSignature.begin(),chunkSignature.end());
        std::vector<unsigned char> tmp;tmp.reserve(256*2+16);
        buffer_refdump(tmp);
        to_buffer_bigendian<unsigned long long>(getchecksum(tmp),buff);
        return buff;
    }
    void dump_ref(std::vector<unsigned char>& buff)const{
        buff.insert(buff.end(),chunkSignature.begin(),chunkSignature.end());
        std::vector<unsigned char> tmp;tmp.reserve(256*2+16);
        buffer_refdump(tmp);
        to_buffer_bigendian<unsigned long long>(getchecksum(tmp),buff);
        array_to_buffer_bigendian(tmp,buff);
    }
    std::vector<unsigned char> bufferdump()override{
        std::vector<unsigned char> keluaran;
        buffer_refdump(keluaran);
        return keluaran;
    }
    void buffer_refdump(std::vector<unsigned char>& buffer)const{
        to_buffer_bigendian<unsigned short>(tilelistId.size(),buffer);
        for (size_t i = 0; i < tilelistId.size(); i++)
        {
           tilelistId[i].ref_dump(buffer);
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
    static bool is_Databuffer_valid(const std::vector<unsigned char>& buffer,size_t& offset){
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
    static bool is_buffer_valid(const std::vector<unsigned char>& buffer,size_t& offset){
        using namespace zt::Internal;
        if(buffer.size()<offset+chunkSignature.size()+sizeof(unsigned long long))return false;
        for (size_t i = 0; i < chunkSignature.size(); i++)
        {
            if(buffer[offset++]!=chunkSignature[i])return false;
        }
        debug_print("chunkmap valid signature");
        //offset+=chunkSignature.size();
        unsigned long long checksum;
        if(!parse::checkPrimitiveBigendian<unsigned long long>(buffer,offset))return false;
        offset-=sizeof(unsigned long long);
        buffer_bigendian_to<unsigned long long>(buffer,offset,checksum);
        std::vector<unsigned char> temp;
        size_t arrlength;unsigned char btl;
        if(!parse::checkArrayBigendian(buffer,offset,arrlength,btl))return false;
        offset-=(2+btl);buffer_bigendian_to_array(buffer,offset,temp);
        //if(buffer.size()<offset+arrlength)return false;
        if(!verifychecksum(temp.begin(),temp.end(),checksum))return false;
        size_t templen=0;
        debug_print("chunkmap checksum");
        is_Databuffer_valid(temp,templen);
        return true;
    }
    void parse_buffer(const std::vector<unsigned char>& buffer,size_t& offset){
        unsigned short arrlengthc;
        buffer_bigendian_to<unsigned short>(buffer,offset,arrlengthc);
        tilelistId.clear();tilelistId.reserve(arrlengthc);
        {
            
            for (size_t i = 0; i < arrlengthc; i++)
            {
                tilelistId.emplace_back(buffer,offset);
            }
        }
        debug_print("tile id completed at offset:"<<offset);
        buffer_bigendian_to(buffer,offset,arrlengthc);
        if(arrlengthc!=sizex*sizey) throw std::exception("error:jumlah tile buffer tak sesuai");
        for (size_t i = 0; i < arrlengthc; i++)
        {
            tilesBuffer[i].parse(buffer,offset);
        }
        debug_print("tile buffer completed at offset:"<<offset);
        unsigned long long arlen;
        buffer_bigendian_to_dynamic(buffer,offset,arlen);
        entitiesBuffer.clear();entitiesBuffer.reserve(arlen);
        for (size_t i = 0; i < arlen; i++)
        {
            entitiesBuffer.emplace_back(buffer,offset);
        }
        debug_print("entity buffer completed at offset:"<<offset);
    }
    void parse(const std::vector<unsigned char>& buffer,size_t& offset){
        if(buffer.size()<offset+chunkSignature.size()+sizeof(unsigned long long))throw std::exception("error:chunk parsing invalid size");
        for (size_t i = 0; i < chunkSignature.size(); i++)
        {
            if(buffer[offset+i]!=chunkSignature[i])throw std::exception("error:chunk parsing invalid signature");
        }
        offset+=chunkSignature.size();
        unsigned long long checksum;
        size_t arrlength=0;
        std::vector<unsigned char> tarr;
        buffer_bigendian_to<unsigned long long>(buffer,offset,checksum);
        buffer_bigendian_to_array(buffer,offset,tarr);
        debug_print("checksum had been extracted");
        if(!verifychecksum(tarr.begin(),tarr.end(),checksum))throw std::exception("error:chunk parsing invalid checksum");
        parse_buffer(tarr,arrlength);
    }
    chunkmap(const std::vector<unsigned char>& buffer,size_t& offset){
        parse(buffer,offset);
    }
    chunkmap(){
        std::cout<<"creating a new chunk\n";
        dirty=true;
    };
};
