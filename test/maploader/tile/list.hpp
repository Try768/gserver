#pragma once
#include "../internal/datatype.hpp"
#include "../common.hpp"
class tilelist{
    private:
        unsigned char idinchunk;
        std::vector<unsigned char> component;
    public:
        std::vector<unsigned char> dump(){
            std::vector<unsigned char>keluaran;
            ref_dump(keluaran);
            return keluaran;
        }
        inline void ref_dump(std::vector<unsigned char>& buffer)const{
            buffer.push_back(idinchunk);
            array_to_buffer_bigendian(component,buffer);
        }
        bool is_buffer_valid(const std::vector<unsigned char>& buffer,size_t offset){
            using namespace zt::Internal;
            size_t len;
            if(!parse::checkPrimitiveBigendian<unsigned char>(buffer,offset))return false;
            if(!parse::checkArrayBigendian(buffer,offset,len))return false;
            if(len>256)return false;
            offset+=len;
            return true;
        }
        //this may throw error
        void parse(const std::vector<unsigned char>& buffer,size_t& offset){
           buffer_bigendian_to<unsigned char>(buffer,offset,idinchunk);
           buffer_bigendian_to_array(buffer,offset,component);
        }
        const inline auto& getComponent(){
            return component;
        }
        inline auto getIdinChunk()const{
            return idinchunk;
        }
        void setIdinChunk(unsigned char id){
            this->idinchunk=id;
        }
        void setComponent(std::vector<unsigned char> component){
            if(component.size()<this->component.size())return;
            this->component.insert(this->component.begin(),component.begin(),component.begin()+component.size());
        }
        void setupComponent(unsigned char componentSize){
            for (size_t i = 0; i < componentSize;i++)
            {
                component.emplace_back(0);
            }
        }
        tilelist()=default;
        tilelist(unsigned char idinchunk):idinchunk(idinchunk)
        {}
        

        
};