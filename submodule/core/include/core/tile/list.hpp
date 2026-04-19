#pragma once
#include "../internal/datatype.hpp"
//#include "component.hpp"
#include "../common.hpp"
#include "component.hpp"
class TileData{
    private:
        friend class chunkmap;
        IndeksTileComponent indeks;
        unsigned int permutation;
        unsigned char max_permutation;
    public:
        template<unsigned char permLen>
        std::vector<unsigned char> dump(unsigned char permutationlen){
            std::vector<unsigned char>keluaran;
            ref_dump<permLen>(keluaran,permutationlen);
            return keluaran;
        }
        TileData(const TileData& data):indeks(data.indeks),permutation(data.permutation){}
        template<unsigned char permLen>
        inline void ref_dump(std::vector<unsigned char>& buffer,unsigned char localtileID)const{
            buffer.emplace_back(localtileID);
            if(ceklength(permutation)==permLen);
            using tipe=zt::Internal::util::getType<permLen>::type;
            to_buffer_bigendian<tipe>(static_cast<tipe>(permutation),buffer);
        }
        template<>
        inline void ref_dump<0>(std::vector<unsigned char>& buffer,unsigned char localtileID)const{
            buffer.emplace_back(localtileID);
        }
        bool is_valid()const{
            return indeks.is_valid();
        }
        
        //do not use
        static bool is_buffer_valid(const std::vector<unsigned char>& buffer,size_t offset){
            using namespace zt::Internal;
            unsigned char len;
            const unsigned char permutationlen=1;
            if(!parse::checkPrimitiveBigendian<unsigned char>(buffer,offset))return false;
            //if(!parse::checkPrimitiveBigendian<util::getType<permutationlen>>())return false;
            if(len>4)return false;
            offset+=len;
            return true;
        }
        //this may throw error
        template<unsigned char perm_len>
        unsigned char parse(const std::vector<unsigned char>& buffer,size_t& offset){
           //buffer_bigendian_to<unsigned char>(buffer,offset,idinchunk);
            unsigned char id;
            using tipe=zt::Internal::util::getType<perm_len>::type;
            buffer_bigendian_to<unsigned char>(buffer,offset,id);
            tipe temp;
            buffer_bigendian_to<tipe>(buffer,offset,(temp));
            permutation=temp;
            return id;
        }
        template<>
        unsigned char parse<0>(const std::vector<unsigned char>& buffer,size_t& offset){
           //buffer_bigendian_to<unsigned char>(buffer,offset,idinchunk);
            unsigned char id;
            buffer_bigendian_to<unsigned char>(buffer,offset,id);
            return id;
        }
        template<unsigned char T>
        using const_ctor=std::integral_constant<unsigned char,T>;
        //jika ada kesalahan pada data maka data di kirim ke event listener bersangkutan
        //jika data aman maka data di pasang kan dengan component nya
        template<unsigned char Len>
        TileData(const_ctor<Len> Tm,const std::vector<unsigned char>& buffer,
            size_t& offset,unsigned char& id){
            id=parse<Tm>(buffer,offset);
        }
        const inline auto getpermutation()const noexcept{
            return permutation;
        }
        const inline bool setPermutation(unsigned int permutation){
            if(max_permutation==0)return false;
            if(ceklength(permutation)!=(~0ULL)>>(64-(this->max_permutation*8)))return false;
            this->permutation=permutation;
            return true;
        }
        inline bool forceSetPermutation(unsigned int permutation){
            this->permutation=permutation;
        }
        inline auto getPermutationMaxLength()const{
            return max_permutation;
        }
        inline TileComponent::Internal& getInternal(){
            return (indeks.get());
        }
        inline const TileComponent::Internal& getInternal()const{
            return (indeks.get());
        }
        //void setIdinChunk(unsigned char id){
        //    this->idinchunk=id;
        //}
        //void setComponent(std::vector<unsigned char> component){
        //    if(component.size()<this->component.size())return;
        //    this->component.insert(this->component.begin(),component.begin(),component.begin()+component.size());
        //}
        //void setupComponent(unsigned char componentSize){
        //    for (size_t i = 0; i < componentSize;i++)
        //    {
        //        component.emplace_back(0);
        //    }
        //}
        TileData()=default;
        TileData(const TileData&)=default;
        TileData(IndeksTileComponent indeks):indeks(indeks)
        {}
        

        
};