#pragma once
#include "../common.hpp"
#include "../internal/datatype.hpp"
class entitylist:public Coord_manager_local{
    private:
        unsigned long long id;
        Typein::Component component;
    public:
         void ref_dump(std::vector<unsigned char>& keluaran)const{
            to_buffer_bigendian<unsigned long long>(id,keluaran);
            this->localdump(keluaran);
            //to_buffer_bigendian<unsigned long long>(component.data.size(),keluaran);
            component.dump(keluaran);
        }
        static bool is_buffer_valid(const std::vector<unsigned char>& buffer,size_t offset){
            using namespace zt::Internal;
            if(!parse::checkPrimitiveBigendian<unsigned long long>(buffer,offset))return false;
            if(!Coord_manager_local::is_local_coor_buffer_valid(buffer,offset))return false;
            //a=parse::checkPrimitiveBigendian<unsigned long long>(buffer,offset)&&a;
            if(!Typein::Component::is_buffer_valid(buffer,offset))return false;
            return true;
        }
        //this may throw errors
        void parse(const std::vector<unsigned char>& buffer,size_t& offset){
            buffer_bigendian_to<unsigned long long>(buffer,offset,id);
            this->localCoorParse(buffer,offset);
            debug_print("normal parsing is completed at offset:"<<offset);
            component.parse(buffer,offset);
        }
        entitylist(const std::vector<unsigned char>& buffer,size_t& offset){
            parse(buffer,offset);
        }
        entitylist(unsigned long long id,Typein::Component component,
            Coord<unsigned int>lokal
            ):component(component)
        {
            this->lokal=lokal;
        }
        entitylist()=default;
        auto& getComponent(std::string key){
            return component[key];
        }
};