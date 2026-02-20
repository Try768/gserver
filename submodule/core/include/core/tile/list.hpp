#pragma once
#include "../internal/datatype.hpp"
//#include "component.hpp"
#include "../common.hpp"
#include "component.hpp"
class TileData{
    private:
        IndeksTileComponent indeks;
        unsigned int permutation;
    public:
        std::vector<unsigned char> dump(unsigned char permutationlen){
            std::vector<unsigned char>keluaran;
            ref_dump(keluaran,permutationlen);
            return keluaran;
        }
        inline void ref_dump(std::vector<unsigned char>& buffer,unsigned char permutationlen)const{
            //buffer.push_back(idinchunk);
            dynamic_to_buffer_bigendian(permutation,buffer);
        }
        bool is_buffer_valid(const std::vector<unsigned char>& buffer,size_t offset,unsigned char permutationlen){
            using namespace zt::Internal;
            unsigned char len;
           // if(!parse::checkPrimitiveBigendian<unsigned char>(buffer,offset))return false;
            if(!parse::checkDynamicBigendian(buffer,offset,len))return false;
            if(len>4)return false;
            offset+=len;
            return true;
        }
        //this may throw error
        void parse(const std::vector<unsigned char>& buffer,size_t& offset,unsigned char permutationlen){
           //buffer_bigendian_to<unsigned char>(buffer,offset,idinchunk);
           unsigned long long temp;
           buffer_bigendian_to_dynamic(buffer,offset,temp);
           if(temp>std::numeric_limits<unsigned int>::max())throw std::exception("kelebihan permutasi");
           permutation=static_cast<unsigned int>(temp);
        }
        const inline auto getpermutation()const noexcept{
            return permutation;
        }
        const inline void setPermutation(unsigned int permutation){
            this->permutation=permutation;
        }
        inline auto getpermutationlength()const{
            return ceklength(permutation);
        }
        inline const auto getIndeks()const{
            return indeks;
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
        TileData(IndeksTileComponent indeks):indeks(indeks)
        {}
        

        
};