#include "register/register.hpp"
#include "internal/entity/signed_component.hpp"
std::unordered_map<std::string,playerlist> registry::players={};
std::unordered_map<std::string,tilecomponent::Internal> registry::tiles={};
std::unordered_map<std::string,EntityComponent::Internal> registry::entities={};
bool registry::addEntityCustomComponent(bool(*componentFunction)(EntityData&,const Const_component&) ,const std::string& componentName){
    auto itc=this->customComponents.find(componentName);
    if(itc==customComponents.end()){
        customComponents[componentName]=componentFunction;
        return 1;
    }
    return 0;
}
bool registry::callEntityCustomComponent(const std::string& componentName,EntityData& data,const Const_component& const_component){
    auto itc=this->customComponents.find(componentName);
    if(itc==customComponents.end())return 0;
    return itc->second(data,const_component);
}
bool registry::entityregister(const std::string& idname,const Const_component_Object& CCO){
        registry::entities.try_emplace(idname,CCO);
    }
void registry::dumping::players(std::vector<unsigned char>& keluaran){
    dynamic_to_buffer_bigendian(registry::players.size(),keluaran);
    for (auto& player:registry::players)
    {
        player.second.dump(keluaran);
    }
}
//void registry::dumping::tiles(std::vector<unsigned char>& keluaran){
//    to_buffer_bigendian<unsigned long long>(registry::tiles.size(),keluaran);
//    for(auto& tile:registry::tiles){
//        tile.second.ref_dump(keluaran);
//    }
//}
//void registry::dumping::entity(std::vector<unsigned char>& keluaran){
//    to_buffer_bigendian<unsigned long long>(registry::entities.size(),keluaran);
//    for (auto& ent:registry::entities)
//    {
//        dynamic_to_buffer_bigendian(ent.first,keluaran);
//        ent.second.ref_dump(keluaran);
//    }
//}
void registry::dump(std::vector<unsigned char>& keluaran){
    dumping::players(keluaran);
}

//bool registry::check::entities(const std::vector<unsigned char>& data,size_t& offset){
//    using namespace zt::Internal;
//    unsigned char len;
//    unsigned long long arrsize;
//    if(!parse::checkPrimitiveBigendian<unsigned long long>(data,offset))return false;
//    offset-=sizeof(unsigned long long);
//    buffer_bigendian_to<unsigned long long>(data,offset,arrsize);
//    debug_print("entitycomponent count check:"<<arrsize);
//    for (size_t i = 0; i < arrsize; i++)
//    {
//        if(!parse::checkDynamicBigendian(data,offset,len))return false;
//        offset--;
//        unsigned long long id;
//        buffer_bigendian_to_dynamic(data,offset,id);
//        if(!EntityComponent::is_buffer_valid(data,offset))return false;
//    }
//    debug_print("passed entitycomponent check");
//}
//bool registry::check::tiles(const std::vector<unsigned char>& data,size_t& offset){
//    using namespace zt::Internal;
//    unsigned char len;
//    unsigned long long arrsize;
//    if(!parse::checkPrimitiveBigendian<unsigned long long>(data,offset))return false;
//    offset-=sizeof(unsigned long long);
//    buffer_bigendian_to<unsigned long long>(data,offset,arrsize);
//    for (size_t i = 0; i < arrsize; i++)
//    {
//        if(!tilecomponent::is_buffer_valid(data,offset))return false;
//    }
//    debug_print("passed tilecomponent check");
//}
bool registry::check::players(const std::vector<unsigned char>& data,size_t& offset){
    using namespace zt::Internal;
    unsigned char len;
    unsigned long long arrsize;
    if(!parse::checkDynamicBigendian(data,offset,len))return false;
    offset--;buffer_bigendian_to_dynamic(data,offset,arrsize);
    debug_print("passed dynamic size check:"<<offset);
    for (size_t i = 0; i < arrsize; i++)
    {
        if(!playerlist::is_buffer_valid(data,offset))return false;
    }
    debug_print("passed playerlist check");
}
void registry::parsing::players(const std::vector<unsigned char>& data,size_t& offset){
    using namespace zt::Internal;
    unsigned char len;
    unsigned long long arrsize;
    registry::players.clear();
    buffer_bigendian_to_dynamic(data,offset,arrsize);
    debug_print("parsing playerlist size:"<<arrsize);
    if(arrsize>maxplayers)throw std::exception("Registry playerlist size exceeded");
    debug_print("parsing playerlist count:"<<arrsize);
    for (size_t i = 0; i < arrsize; i++)
    {
        playerlist temp(data,offset);
        registry::players[temp.getname()]=temp;
    }
    debug_print("finished parsing playerlist");
}
//void registry::parsing::tiles(const std::vector<unsigned char>& data,size_t& offset){
//    using namespace zt::Internal;
//    unsigned char len;
//    unsigned long long arrsize;
//    registry::tiles.clear();
//    buffer_bigendian_to<unsigned long long>(data,offset,arrsize);
//    debug_print("parsing tilecomponent size:"<<arrsize);
//    if(arrsize>maxtiles)throw std::exception("Registry tilecomponent size exceeded");
//    debug_print("parsing tilecomponent count:"<<arrsize);
//    tilecomponent temp;
//    for (size_t i = 0; i < arrsize; i++)
//    {
//        temp.parse(data,offset);
//        nametiles[temp.name]=idcountt;
//        registry::tiles[idcountt]=temp;
//        idcountt++;
//    }
//}
//void registry::parsing::entities(const std::vector<unsigned char>& data,size_t& offset){
//    using namespace zt::Internal;
//    unsigned char len;
//    unsigned long long arrsize;
//    registry::entities.clear();
//    buffer_bigendian_to<unsigned long long>(data,offset,arrsize);
//    if(arrsize>maxentities)throw std::exception("Registry entitycomponent size exceeded");
//    debug_print("parsing entitycomponent count:"<<arrsize);
//    for (size_t i = 0; i < arrsize; i++)
//    {
//        unsigned long long id;
//        buffer_bigendian_to_dynamic(data,offset,id);
//        EntityComponent temp(data,offset);
//        registry::entities[id]=temp;
//    }
//}
void registry::fullparse(const std::vector<unsigned char>& data,size_t offset){
    using namespace zt::Internal;
    unsigned long long sum;
    unsigned long long arrsize;
    for (size_t i = 0; i < sign.size(); i++)
    {
        if(data[offset++]!=sign[i])throw std::runtime_error("Registry sign invalid");
    }
    buffer_bigendian_to<unsigned long long>(data,offset,sum);
    std::vector<unsigned char> temp;
    buffer_bigendian_to_array(data,offset,temp);
    debug_print("extracted array size:"<<temp.size()<<"checksum:"<<sum<<","<<offset);
    size_t tempoffset=0;
    if(!checksumparent::verifychecksum(temp.begin(),temp.end(),sum))throw std::exception("Registry checksum invalid");
    debug_print("passed checksum verification");
    parse(temp,tempoffset);
}
bool registry::is_fullbuffer_valid(const std::vector<unsigned char>& data,size_t& offset){
    using namespace zt::Internal;
    unsigned long long sum;
    for (size_t i = 0; i < sign.size(); i++)
    {
        if(data[offset++]!=sign[i])return false;
    }
    debug_print("passed sign check:"<<offset);
    if(!parse::checkPrimitiveBigendian<unsigned long long>(data,offset))return false;
    offset-=sizeof(unsigned long long);
    buffer_bigendian_to<unsigned long long>(data,offset,sum);
    std::vector<unsigned char> temp;
    size_t leng;
    unsigned char btl;
    if(!parse::checkArrayBigendian(data,offset,leng,btl))return false;
    offset-=(btl+2);
    buffer_bigendian_to_array(data,offset,temp);
    debug_print("passed array extraction:"<<offset<<","<<leng);
    if(!checksumparent::verifychecksum(temp.begin(),temp.end(),sum))return false;
    debug_print("passed checksum verification:"<<offset);
    size_t tempoffset2=0;
    if(!is_buffer_valid(temp,tempoffset2))return false;
    return true;
}
void registry::fulldump(std::vector<unsigned char>& buffer){ 
    buffer.insert(buffer.end(),sign.begin(),sign.end());
    {
        std::vector<unsigned char> temp;
        dump(temp);
        debug_print("dumped registry size:"<<temp.size());
        CheckSumForStatic ast(temp);
        to_buffer_bigendian<unsigned long long>(ast.getchecksum(),buffer);
        array_to_buffer_bigendian(temp,buffer);
    }
    
}