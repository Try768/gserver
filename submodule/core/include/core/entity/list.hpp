#pragma once
#include "../common.hpp"
#include "../internal/datatype.hpp"
#include "component.hpp"
#include "core/node/node.hpp"
#include "../internal/component/var.hpp"
class Entity;
class EntityData:public Coord_manager{
    private:
        friend class Entity;
        friend class SimulatedEntity;
        //unsigned long long id;
        velo2 velocity;
        IndeksEntityComponent indeks;
        Var_component_Object runtime_property;
        //std::string name;
        Typein::Component dynamic_property;
        ActionNode* nodeAksi;
        static enum status_flag:unsigned char{
            is_valid=0b01,
            is_loaded=0b10
        };
        unsigned char flag;
    public:
        inline const Var_component_Object& getRuntimeproperty()const{return runtime_property;};
        void swap(const IndeksEntityComponent& indeks){
            this->indeks=indeks;
        }
        //const std::string& getname()const noexcept{return name;}
        void ref_dump(std::vector<unsigned char>& keluaran)const{
            this->localdump(keluaran);
            dynamic_property.dump(keluaran);
        }
        static bool is_buffer_valid(const std::vector<unsigned char>& buffer,size_t offset){
            using namespace zt::Internal;
            //if(!parse::checkPrimitiveBigendian<unsigned long long>(buffer,offset))return false;
            if(!Coord_manager_local::is_local_coor_buffer_valid(buffer,offset))return false;
            //a=parse::checkPrimitiveBigendian<unsigned long long>(buffer,offset)&&a;
            if(!Typein::Component::is_buffer_valid(buffer,offset))return false;
            return true;
        }
        //this may throw errors
        void parse(const std::vector<unsigned char>& buffer,size_t& offset){
            //buffer_bigendian_to<unsigned long long>(buffer,offset,indeks.entityType);
            this->localCoorParse(buffer,offset);
            debug_print("normal parsing is completed at offset:"<<offset);
            dynamic_property.parse(buffer,offset);
        }
        EntityData(const std::vector<unsigned char>& buffer,size_t& offset){
            parse(buffer,offset);
        }
        EntityData(Typein::Component dynamic_property,const Coord<long long>& chunkcoord,
            const Coord<int16_t>& lokal,IndeksEntityComponent indeks
            ):dynamic_property(dynamic_property),Coord_manager(chunkcoord,lokal),
            indeks(indeks)
        {}
        EntityData()=default;
        auto& getDynamicProperty(std::string key){
            return dynamic_property[key];
        }
};
 //EntityData(EntityData&& dataentity):Coord_manager(std::move(dataentity)),
        //dynamic_property(std::move(dataentity.dynamic_property))
        //{}
        ////EntityData& operator=(EntityData&&) = default;
        //constexpr EntityData &EntityData::operator=(const EntityData &)=default;
        //EntityData(const EntityData& dataentity):Coord_manager((dataentity)),
        //dynamic_property((dataentity.dynamic_property)){}
        //to_buffer_bigendian<unsigned long long>(indeks.entityType,keluaran);
         //to_buffer_bigendian<unsigned long long>(component.data.size(),keluaran);