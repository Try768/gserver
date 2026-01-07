#pragma once
#include "../common.hpp"
#include "../internal/datatype.hpp"
struct Internal_c{
       unsigned char Type;
        std::string Name;
    };
class entitycomponent
{
private:
    /* data */
    unsigned long long id;
    std::string idname;
    std::vector<Internal_c> inComponent;
    public:
    //inline static std::unordered_set<unsigned long long> usedId={1};
    //entitycomponent(/* args */){}
    struct C_entityData{
        const unsigned long long& id;
        const std::string& idname;
        const std::vector<Internal_c>& inComponent;
    };
    C_entityData getData()const{
        return{id,idname,inComponent};
    }
    std::vector<unsigned char> dump()const{
        std::vector<unsigned char> keluaran;
        ref_dump(keluaran);
        return keluaran;
    }
    void ref_dump(std::vector<unsigned char>& buffer)const{
         to_buffer_bigendian<unsigned long long>(id,buffer);
        string_short_to_buffer_bigendian(idname,buffer);
        to_buffer_bigendian<unsigned short>((unsigned short)inComponent.size(),buffer);
        for (size_t i = 0; i < inComponent.size(); i++)
        {
            to_buffer_bigendian<unsigned char>(inComponent[i].Type,buffer);
            string_short_to_buffer_bigendian(inComponent[i].Name,buffer);
        }
    }
    static bool is_buffer_valid(const std::vector<unsigned char>& data,size_t& offset){
        using namespace zt::Internal;
        size_t len;
        debug_print("checking entitycomponent buffer validity at offset:"<<offset);
        if(!parse::checkPrimitiveBigendian<unsigned long long>(data,offset))return false;
        if(!parse::checkStringBigendian(data,offset,len))return false;
        offset+=len;
        unsigned short compSize;
        if(!parse::checkPrimitiveBigendian<unsigned short>(data,offset))return false;
        offset-=sizeof(unsigned short);
        buffer_bigendian_to<unsigned short>(data,offset,compSize);
        for (size_t i = 0; i < compSize; i++)
        {
            if(!parse::checkPrimitiveBigendian<unsigned char>(data,offset))return false;
            offset-=sizeof(unsigned char);
            unsigned char type;
            buffer_bigendian_to<unsigned char>(data,offset,type);
            size_t length;
            if(!parse::checkStringBigendian(data,offset,length))return false;
            offset+=length;
        }
        return true;
    }
    void parse(const std::vector<unsigned char>& data,size_t& offset){
        buffer_bigendian_to(data,offset,id);
        buffer_bigendian_to_string_short(data,offset,idname);
        unsigned short compSize;
        buffer_bigendian_to<unsigned short>(data,offset,compSize);
        inComponent.clear();
        inComponent.reserve(compSize);
        for (size_t i = 0; i < compSize; i++)
        {
            Internal_c temp;
            buffer_bigendian_to<unsigned char>(data,offset,temp.Type);
            buffer_bigendian_to_string_short(data,offset,temp.Name);
            inComponent.push_back(temp);
        }
    }
    entitycomponent(const std::vector<unsigned char>& data,size_t& offset){
        parse(data,offset);
    }
    Typein::Component getDefaultComponent()const{
        Typein::Component keluaran;
        for (size_t i = 0; i < inComponent.size(); i++)
        {
            switch (inComponent[i].Type)
            {
            case t_type_id::integer:
                keluaran[inComponent[i].Name]=static_cast<unsigned int>(0);
                break;
            case t_type_id::string:
                keluaran[inComponent[i].Name]=std::string("");
                break;
            case t_type_id::boolean:
                keluaran[inComponent[i].Name]=false;
                break;
            case t_type_id::loong:
                keluaran[inComponent[i].Name]=static_cast<unsigned long long>(0);
                break;
            case t_type_id::character:
                keluaran[inComponent[i].Name]=static_cast<unsigned char>(0);
                break;
            default:
                break;
            }
        }
        return keluaran;
    }
    entitycomponent()=default;
    entitycomponent(unsigned long long id,
        std::string idname,std::vector<Internal_c> components):id(id),idname(idname),inComponent(components)
    {}
};
