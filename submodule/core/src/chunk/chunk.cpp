#include <core/chunk/chunk.hpp>
#include"core/tile/component.hpp"
#include "core/world/world.hpp"
//to do: add simulation to coresponding tile
void chunkmap::internal_swap_Tile(const std::string& before,const std::string& after){
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
void chunkmap::SimulationTile::simulate(std::array<chunkmap::__Tile,sizex*sizey>& buffer,
    Dimension& dimension,Registry& reg,chunkmap::time_point time_pivot){
    for(auto indata:data){
        auto& tile=buffer[indata].data;
        constexpr auto eventTick=zt::event::tile::Type::Tick;
        zt::event::tile::params<eventTick> param{Tile(tile)};
        auto cb=dimension.getRoom().getWorld().getRegister().getTEL();
        bool is_not_cancle=true;
        cb.getBeforeEvent<eventTick>().emit(param,is_not_cancle);
        if(!is_not_cancle)cb.getComponent<eventTick>().emit(param,TileComponent(tile.getInternal()));
        cb.getAfterEvent<eventTick>().emit(param);
    }
}
//some harsh code in here
std::vector<unsigned char> chunkmap::dump()const{
        std::vector<unsigned char> buff;
        buff.insert(buff.end(),chunkSignature.begin(),chunkSignature.end());
        std::vector<unsigned char> tmp;tmp.reserve(256*2+16);
        buffer_refdump(tmp);
        to_buffer_bigendian<unsigned long long>(getchecksum(tmp),buff);
        return buff;
    }
    void chunkmap::dump_ref(std::vector<unsigned char>& buff)const{
        buff.insert(buff.end(),chunkSignature.begin(),chunkSignature.end());
        std::vector<unsigned char> tmp;tmp.reserve(256*2+16);
        buffer_refdump(tmp);
        to_buffer_bigendian<unsigned long long>(getchecksum(tmp),buff);
        array_to_buffer_bigendian(tmp,buff);
    }
    std::vector<unsigned char> chunkmap::bufferdump()const{
        std::vector<unsigned char> keluaran;
        buffer_refdump(keluaran);
        return keluaran;
    }
    void chunkmap::buffer_refdump(std::vector<unsigned char>& buffer)const{
        buffer.reserve(buffer.capacity()+512);//is this append
        to_buffer_bigendian<unsigned char>(tileId.size(),buffer);
        for(const auto & idc: tileId){
            buffer.emplace_back(idc.first);
            string_short_to_buffer_bigendian(idc.second,buffer);
        }
        buffer.emplace_back(this->perm_len);
        switch (perm_len)
        {
        case 0:for(const auto& tile_data:this->tilesBuffer){tile_data.data.ref_dump<0>(buffer,tile_data.id);}
        case 1:for(const auto& tile_data:this->tilesBuffer){tile_data.data.ref_dump<1>(buffer,tile_data.id);}
            break;
        case 2:
            for(const auto& tile_data:this->tilesBuffer){
                tile_data.data.ref_dump<2>(buffer,tile_data.id);}
            break;
        case 4:
            for(const auto& tile_data:this->tilesBuffer){
                tile_data.data.ref_dump<4>(buffer,tile_data.id);}
            break;
        default:
            throw std::runtime_error("err:buffer_ref_dump:chunk:87");
            break;
        }
    }
    size_t chunkmap::is_Databuffer_valid(const std::vector<unsigned char>& buffer,size_t offset){
        using namespace zt::Internal::parse;
        if((checkPrimitiveBigendian<unsigned char>(buffer,offset))==0)return 0;
        unsigned char tileIdlen;
        buffer_bigendian_to<unsigned char>(buffer,offset,tileIdlen);
        for (size_t i = 0; i < tileIdlen; i++)
        {
            if(++offset>buffer.size())return false;
            if((offset=checkStringBigendian(buffer,offset))==0)return 0;
        }
        if(buffer.size()<offset+1)return false;
        unsigned char perm_len=buffer[offset++];
        if (perm_len>4||perm_len==3)return false;
        if(buffer.size()<offset+((perm_len+1)*UCHAR_MAX))return false;
        offset+=((perm_len+1)*UCHAR_MAX);
        return offset;
    }
    size_t chunkmap::is_buffer_valid(const std::vector<unsigned char>& buffer,size_t offset){
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
        buffer_bigendian_to<unsigned long long>(buffer,offset,checksum);
        std::vector<unsigned char> temp;
        size_t arrlength;unsigned char btl;
        if(!parse::checkArrayBigendian(buffer,offset,arrlength,btl))return false;
        buffer_bigendian_to_array(buffer,offset,temp);
        //if(buffer.size()<offset+arrlength)return false;
        if(!verifychecksum(temp.begin(),temp.end(),checksum))return false;
        size_t templen=0;
        debug_print("chunkmap checksum");
        if(!is_Databuffer_valid(temp,templen))return 0;
        return offset;
    }
    void chunkmap::parse_buffer(const std::vector<unsigned char>& buffer,size_t& offset){
        std::unordered_set<std::string> nmcomponent;
        std::unordered_map<unsigned char,const IndeksTileComponent&> component;
        auto& in_reg=this->reg;
        auto& in_tileId=this->tileId;
        unsigned char tmpchar;
        buffer_bigendian_to<unsigned char>(buffer,offset,tmpchar);
        for (size_t i = 0; i < tmpchar; i++)
        {
            unsigned char id=buffer[i];++i;
            std::string name;
            buffer_bigendian_to_string_short(buffer,offset,name);
            in_tileId.emplace(id,name);
            if(nmcomponent.count(name)==0){
                auto ind=reg.getTileComponentByName(name);
                if(ind.is_valid()){
                    auto [itc,stat] =component.emplace(id,ind);
                    if(stat){
                        nmcomponent.emplace(name);
                    }
                }
            }
            tmpchar=buffer[offset];++offset;
            auto inner=[this,&component,&buffer,&offset,&in_reg](auto ic){
                
                //const unsigned char N=1;
                auto& datas=this->tilesBuffer;
                for (size_t i = 0; i < 0xff; i++)
                {
                    TileData data(ic,buffer,offset,datas[i].id);
                    auto itf=component.find(datas[i].id);
                    if(itf==component.end()){
                        constexpr auto eventInvalid=zt::event::tile::Type::InvalidData;
                        zt::event::tile::params<eventInvalid> param{data,in_reg};
                        auto cb=dimension.getRoom().getWorld().getRegister().getTEL();
                        cb.getBeforeEvent<eventInvalid>().emit(param);
                    }else{
                        data.indeks=itf->second;
                    }
                    datas[i].data=data;
                }
                
            };
            switch (tmpchar)
            {
            case 0:inner(TileData::const_ctor<0>{});
                break;
            case 1:inner(TileData::const_ctor<1>{});
                break;
            case 2:inner(TileData::const_ctor<2>{});
                break;
            case 4:inner(TileData::const_ctor<4>{});
                break;
            default:
            throw std::runtime_error("err:185:permutation len unknown");
                break;
            }
            
        }
    }
    void chunkmap::parse(const std::vector<unsigned char>& buffer,size_t& offset){
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