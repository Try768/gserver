#pragma once
#include <unordered_map>
#include <unordered_set>
#include "../chunk/chunk.hpp"
namespace zt{
    constexpr short lenOfDomain=16;
    namespace util{
        inline long long floordiv(long long a, long long b){
            return (a >= 0) ? (a / b) : ((a - (b - 1)) / b);
        }

        inline long long minOfDomain(long long domain){
            return domain -(lenOfDomain/2);
        }
        inline long long maxOfDomain(long long domain){
            return domain +(lenOfDomain/2);
        }
        inline long long chunkToDomain(long long chunkcoord){
            return floordiv(chunkcoord, lenOfDomain) * lenOfDomain;
        }
    }
}
struct IndeksArea1d
    {
        std::unordered_set<long long> usedChunkIds;
        void dump_ref(std::vector<unsigned char>& buffer){
            dynamic_to_buffer_bigendian(usedChunkIds.size(),buffer);
            for (auto a:usedChunkIds)
            {
                to_buffer_bigendian<long long>(a,buffer);
            }
        }
        void dump_ref(std::vector<unsigned char>& buffer,Coord<long long>& domain){
            std::vector<unsigned char> tmp;
            debug_print("Dumping UsedArea1d for x domain:"<<domain.x<<" to "<<domain.x+16);
            long long min=zt::util::minOfDomain(domain.x);
            long long max=zt::util::maxOfDomain(domain.x);
            dynamic_to_buffer_bigendian(usedChunkIds.size(),buffer);
              for (long long x = min; x < max; x++)
                {
                 if(usedChunkIds.count(x)){
                    debug_print("Dumping chunk x="<<x);
                      to_buffer_bigendian<long long>(x,buffer);
                 }
                }
        }
        //just checking
        static bool is_buffer_valid(const std::vector<unsigned char>& buffer,size_t& offset){
            using namespace zt::Internal;
            unsigned long long size;
            unsigned char csize;
            debug_print("masuk");
            if(!parse::checkDynamicBigendian(buffer,offset,csize))return false; 
            offset--;
            buffer_bigendian_to_dynamic(buffer,offset,size);
            debug_print("used area 1 debug: offset:"<<offset<<"len:"<<size);
            for (size_t i = 0; i < size; i++)
            {
                if(!parse::checkPrimitiveBigendian<long long>(buffer,offset))return false;
            }
            return true;
        }
        //this may throw error
        //this can be additional parse
        void parse(const std::vector<unsigned char>& buffer,size_t& offset){
            using namespace zt::Internal;
            unsigned long long size;
            buffer_bigendian_to_dynamic(buffer,offset,size);
            for (size_t i = 0; i < size; i++)
            {
                long long temp;
                buffer_bigendian_to<long long>(buffer,offset,temp);
                debug_print("key x:"<<temp);
                usedChunkIds.insert(temp);
            }
        }
        IndeksArea1d(const std::vector<unsigned char>& buffer,size_t& offset){
            parse(buffer,offset);
        }
        IndeksArea1d()=default;
    };
struct IndeksArea2d{
    
    std::unordered_map<long long,IndeksArea1d> usedChunkIds;
    bool is_found(long x,long y)const{
        auto temp=usedChunkIds.find(y);
        if(temp!=usedChunkIds.end())if(temp->second.usedChunkIds.count(x)>0)return true;
        return false;
    }
    void dump_ref(std::vector<unsigned char>& buffer){
        //std::vector<unsigned char> tmp;
        dynamic_to_buffer_bigendian(usedChunkIds.size(),buffer);
        for(auto& id:usedChunkIds){
            dynamic_to_buffer_bigendian(id.first,buffer);
            id.second.dump_ref(buffer);
        }
    }
    IndeksArea2d trimed(Coord<long long>& domain)const{
        IndeksArea2d keluaran;
        auto minY = zt::util::minOfDomain(domain.y);
        auto maxY = zt::util::maxOfDomain(domain.y);
        auto minX = zt::util::minOfDomain(domain.x);
        auto maxX = zt::util::maxOfDomain(domain.x);

        for (long long i = minY; i < maxY; i++)
        {
            auto& itc =usedChunkIds.find(i);
            if(itc !=usedChunkIds.end()){
                for(long long j = minX; j < maxX; j++){
                    if(itc->second.usedChunkIds.count(j)){
                        keluaran.usedChunkIds[itc->first].usedChunkIds.insert(j);
                        debug_print("trimed x:"<<j<<",y:"<<itc->first);
                    }
                }
            }
        }
        
        return keluaran;
    }
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
    }
    IndeksArea2d(const std::vector<unsigned char>& buffer,size_t& offset){
        parse(buffer,offset);
    }
    IndeksArea2d()=default;
};
