#pragma once
#include <unordered_map>
#include <unordered_set>
#include "../chunk/chunk.hpp"

struct IndeksArea2d{
    
    std::unordered_set<Coord<long long>> indeks;
    bool is_found(Coord<long long> coord)const{
        auto temp=indeks.find(coord);
        if(temp!=indeks.end())return true;
        return false;
    }
    //void dump_ref(std::vector<unsigned char>& buffer){
    //    //std::vector<unsigned char> tmp;
    //    dynamic_to_buffer_bigendian(usedChunkIds.size(),buffer);
    //    for(auto& id:usedChunkIds){
    //        dynamic_to_buffer_bigendian(id.first,buffer);
    //        id.second.dump_ref(buffer);
    //    }
    //}

    //min max
    static std::pair<Coord<long long>,Coord<long long>> trimed(Coord<long long>& domain){
        auto minY = zt::util::minOfDomain(domain.y);
        auto maxY = zt::util::maxOfDomain(domain.y);
        auto minX = zt::util::minOfDomain(domain.x);
        auto maxX = zt::util::maxOfDomain(domain.x);
        return std::pair<Coord<long long>,Coord<long long>>(Coord<long long>(minX,minY),Coord<long long>(maxX,maxY));
    }
    
    IndeksArea2d(const std::vector<unsigned char>& buffer,size_t& offset){
        //parse(buffer,offset);
    }
    
    IndeksArea2d()=default;
};
/*
void dump_ref(std::vector<unsigned char>&buffer,Coord<long long>& domain){
        
        dynamic_to_buffer_bigendian(usedChunkIds.size(),buffer);
        long long min=zt::util::minOfDomain(domain.y);
        long long max=zt::util::maxOfDomain(domain.y);
       for (long long i = min; i < max; i++)
       {
        auto& temp = usedChunkIds.find(i);
        if(temp != usedChunkIds.end()){
            debug_print("Dumping UsedArea1d for y="<<i);
            //dynamic_to_buffer_bigendian(i,buffer);
            to_buffer_bigendian<long long>(i,buffer);
            temp->second.dump_ref(buffer,domain);
        }
       }
       
    }
    static bool is_buffer_valid(const std::vector<unsigned char>& buffer,size_t& offset){
        using namespace zt::Internal;
        unsigned long long size;
        unsigned char csize;
        if(!parse::checkDynamicBigendian(buffer,offset,csize))return false;
        debug_print("check point used area is valid :"<<offset);
        offset--;
        buffer_bigendian_to_dynamic(buffer,offset,size);
        debug_print("ukuran:"<<size);
        for (size_t i = 0; i < size; i++)
        {
            if(!parse::checkPrimitiveBigendian<long long>(buffer,offset))return false;
            debug_print("data y valid,offset:"<<offset<<","<<i);
            if(!IndeksArea1d::is_buffer_valid(buffer,offset))return false;
        }
        return true;
    }
    void parse(const std::vector<unsigned char>& buffer,size_t& offset){
        using namespace zt::Internal;
        unsigned long long size;
        buffer_bigendian_to_dynamic(buffer,offset,size);
        long long tempkey;
        for (size_t i = 0; i < size; i++)
        {
            buffer_bigendian_to<long long>(buffer,offset,tempkey);
            debug_print("y:"<<tempkey);
            IndeksArea1d tempvalue(buffer,offset);
            usedChunkIds[(tempkey)]=tempvalue;
        }
    }
    void parse_additional(const std::vector<unsigned char>& buffer,size_t& offset){
        using namespace zt::Internal;
        unsigned long long size;
        buffer_bigendian_to<unsigned long long>(buffer,offset,size);
        for (size_t i = 0; i < size; i++)
        {
            long long tempkey;
            buffer_bigendian_to<long long>(buffer,offset,tempkey);
            debug_print("key y:"<<tempkey);
            auto itc=usedChunkIds.find(tempkey);
            if(itc!=usedChunkIds.end()){
                itc->second.parse(buffer,offset);
                continue;
            }
            IndeksArea1d tempvalue(buffer,offset);
            usedChunkIds[tempkey]=tempvalue;
        }
    }*/