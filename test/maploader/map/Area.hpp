#pragma once
#include <unordered_map>
#include <unordered_set>
#include "../chunk/chunk.hpp"
//todo: bikin fungsi destroy untuk Area (1 dan 2) D untuk di pakai di parse()

struct UsedArea1d
    {
        std::unordered_set<long long> usedChunkIds;
        void dump_ref(std::vector<unsigned char>& buffer){
            dynamic_to_buffer_bigendian(usedChunkIds.size(),buffer);
            for (auto a:usedChunkIds)
            {
                to_buffer_bigendian<unsigned long long>(a,buffer);
            }
        }
        void dump_ref(std::vector<unsigned char>& buffer,Coord<long long>& domain){
            std::vector<unsigned char> tmp;
            dynamic_to_buffer_bigendian(usedChunkIds.size(),buffer);
              for (long long x = domain.x; x < domain.x+16; x++)
                {
                 if(usedChunkIds.count(x)){
                      to_buffer_bigendian<unsigned long long>(x,buffer);
                 }
                }
        }
        //just checking
        static bool is_buffer_valid(const std::vector<unsigned char>& buffer,size_t& offset){
            using namespace zt::Internal;
            unsigned long long size;
            if(!parse::checkPrimitiveBigendian<unsigned long long>(buffer,offset))return false;
            buffer_bigendian_to<unsigned long long>(buffer,offset,size);
            for (size_t i = 0; i < size; i++)
            {
                if(!parse::checkPrimitiveBigendian<unsigned long long>(buffer,offset))return false;
            }
            return true;
        }
        //this may throw error
        //this can be additional parse
        void parse(const std::vector<unsigned char>& buffer,size_t& offset){
            using namespace zt::Internal;
            unsigned long long size;
            buffer_bigendian_to<unsigned long long>(buffer,offset,size);
            for (size_t i = 0; i < size; i++)
            {
                unsigned long long temp;
                buffer_bigendian_to<unsigned long long>(buffer,offset,temp);
                usedChunkIds.insert(temp);
            }
        }
        UsedArea1d(const std::vector<unsigned char>& buffer,size_t& offset){
            parse(buffer,offset);
        }
        UsedArea1d()=default;
    };
struct UsedArea2d{
    
    std::unordered_map<long long,UsedArea1d> usedChunkIds;
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
    UsedArea2d trimed(Coord<long long>& domain)const{
        UsedArea2d keluaran;
        for(long long y=domain.y;y<domain.y+16;y++){
            auto temp=usedChunkIds.find(y);
            if(temp!=usedChunkIds.end()){
                UsedArea1d temparea;
                for(long long x=domain.x;x<domain.x+16;x++){
                    if(temp->second.usedChunkIds.count(x)){
                        temparea.usedChunkIds.insert(x);
                    }
                }
                if(temparea.usedChunkIds.size()>0){
                    keluaran.usedChunkIds[y]=temparea;
                }
            }
        }
        return keluaran;
    }
    void dump_ref(std::vector<unsigned char>&buffer,Coord<long long>& domain){
        
        dynamic_to_buffer_bigendian(usedChunkIds.size(),buffer);
       for (size_t i = domain.y; i < domain.y+16; i++)
       {
        auto temp = usedChunkIds.find(i);
        if(temp != usedChunkIds.end()){
            dynamic_to_buffer_bigendian(i,buffer);
            temp->second.dump_ref(buffer);
        }
       }
       
    }
    static bool is_buffer_valid(const std::vector<unsigned char>& buffer,size_t& offset){
        using namespace zt::Internal;
        unsigned long long size;
        if(!parse::checkPrimitiveBigendian<unsigned long long>(buffer,offset))return false;
        buffer_bigendian_to<unsigned long long>(buffer,offset,size);
        for (size_t i = 0; i < size; i++)
        {
            if(!parse::checkPrimitiveBigendian<unsigned long long>(buffer,offset))return false;
            if(!UsedArea1d::is_buffer_valid(buffer,offset))return false;
        }
        return true;
    }
    void parse(const std::vector<unsigned char>& buffer,size_t& offset){
        using namespace zt::Internal;
        unsigned long long size;
        buffer_bigendian_to<unsigned long long>(buffer,offset,size);
        for (size_t i = 0; i < size; i++)
        {
            long long tempkey;
            buffer_bigendian_to<long long>(buffer,offset,tempkey);
            UsedArea1d tempvalue(buffer,offset);
            usedChunkIds[tempkey]=tempvalue;
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
            auto itc=usedChunkIds.find(tempkey);
            if(itc!=usedChunkIds.end()){
                itc->second.parse(buffer,offset);
                continue;
            }
            UsedArea1d tempvalue(buffer,offset);
            usedChunkIds[tempkey]=tempvalue;
        }
    }
    UsedArea2d(const std::vector<unsigned char>& buffer,size_t& offset){
        parse(buffer,offset);
    }
    UsedArea2d()=default;
};
struct Area1d
    {
        private:
        std::unordered_map<long long, chunkmap*> chunks;
        public:
        //can throw error
        const chunkmap& get_const(long long x)const{
            return *(chunks.at(x));
        }
        chunkmap& get(long long x){
            return *(chunks.at(x));
        }
        Area1d(){
            chunks.reserve(1024);
        }
        inline const bool has_chunk(long long x)const{
            return chunks.find(x)!=chunks.end();
        }
        inline void set(long long x,const chunkmap& chunk){
            if(chunks[x]){
                delete chunks[x];
            }
            chunkmap* p_chunk=new chunkmap(chunk);
            chunks[x]=p_chunk;
        }

        inline void set_ptr(long long x,chunkmap* chunk){
            //if(chunks.find(x)!=chunks.end())
            if(chunks[x]){
                delete chunks[x];
            }
            chunks[x]=chunk;
        }
        inline bool delete_chunk(long long x){
            auto temp=chunks.find(x);
            if(temp!=chunks.end()){
                delete temp->second;
                chunks.erase(temp);
                return true;
            }
            return false;
        }
        inline void clear(){
            for(auto& a:chunks){
                if(a.second){
                    delete a.second;
                    a.second=nullptr;
                }
            }
        }
        ~Area1d(){
            clear();
        }
        void dump_ref(std::vector<unsigned char>& buffer,UsedArea1d& areaget){
            dynamic_to_buffer_bigendian(areaget.usedChunkIds.size(),buffer);
            std::vector<unsigned char> tmp;
            for (auto a:areaget.usedChunkIds)
            {
                chunks[a]->dump_ref(tmp);
                array_to_buffer_bigendian(tmp,buffer);
                tmp.clear();
            }
        }
        static bool is_buffer_valid(const std::vector<unsigned char>& buffer,size_t& offset){
            using namespace zt::Internal;
            unsigned long long size;
            if(!parse::checkPrimitiveBigendian<unsigned long long>(buffer,offset))return false;
            buffer_bigendian_to<unsigned long long>(buffer,offset,size);
            for (size_t i = 0; i < size; i++)
            {
                if(!chunkmap::is_buffer_valid(buffer,offset))return false;
            }
            return true;
        }
        void parse(const std::vector<unsigned char>& buffer,size_t& offset,UsedArea1d& areaget){
            using namespace zt::Internal;
            unsigned long long size;
            clear();
            buffer_bigendian_to<unsigned long long>(buffer,offset,size);
            for (auto a:areaget.usedChunkIds)
            {
                chunkmap* temp=new chunkmap(buffer,offset);
                chunks[a]=temp;
            }
        }
        void parse_additional(const std::vector<unsigned char>& buffer,size_t& offset,UsedArea1d& areaget){
            using namespace zt::Internal;
            unsigned long long size;
            buffer_bigendian_to<unsigned long long>(buffer,offset,size);
            for (auto a:areaget.usedChunkIds)
            {
                chunkmap* temp=new chunkmap(buffer,offset);
                if(chunks[a]){
                    delete chunks[a];
                }
                chunks[a]=temp;
            }
        }
        Area1d(const std::vector<unsigned char>& buffer,size_t& offset,UsedArea1d& areaget){
            parse(buffer,offset,areaget);
        }
    };
struct Area2d{
    private:
    std::unordered_map<long long,Area1d*> chunks;
    public:
    const chunkmap& get_const(long long x,long long y)const{
        return (chunks.at(y)->get_const(x));
    }
    chunkmap& get(long long x,long long y){
        return (chunks.at(y)->get(x));
    }
    inline const bool has_chunk(long long x,long long y)const{
        auto temp=chunks.find(y);
        if(temp!=chunks.end()){
            return temp->second->has_chunk(x);
        }
        return false;
    }
    
    inline void set(long long x,long long y,const chunkmap& chunk){
        if(chunks[y]==nullptr){
            chunks[y]=new Area1d();
        }
        chunkmap* p_chunk=new chunkmap(chunk);
        chunks[y]->set_ptr(x,p_chunk);
    }
    inline void set_ptr(long long x,long long y,chunkmap* chunk){
        if(chunks[y]==nullptr){
            chunks[y]=new Area1d();
        }
        chunks[y]->set_ptr(x,chunk);
    }
    inline bool delete_chunk(long long x,long long y){
        auto& temp=chunks.find(y);
        if(temp!=chunks.end()){
            return temp->second->delete_chunk(x);
        }
    }
    Area2d(){
        chunks.reserve(1024);
    }
    void clear(){
        for(auto& a:chunks){
            if(a.second){
                delete a.second;
                a.second=nullptr;
            }
        }
    }
    ~Area2d(){
        clear();
    }
    void dump_ref(std::vector<unsigned char>& buffer,UsedArea2d& areaget){
        std::vector<unsigned char> tmp;
        dynamic_to_buffer_bigendian(areaget.usedChunkIds.size(),buffer);
        for(auto& plus:areaget.usedChunkIds){
            dynamic_to_buffer_bigendian(plus.first,buffer);
            chunks[plus.first]->dump_ref(buffer,plus.second);
        }
    }
    static bool is_buffer_valid(const std::vector<unsigned char>& buffer,size_t& offset){
        using namespace zt::Internal;
        unsigned char sc;
        unsigned long long size;
        if(!parse::checkDynamicBigendian(buffer,offset,sc))return false;
        offset--;
        buffer_bigendian_to_dynamic(buffer,offset,size);
        for (size_t i = 0; i < size; i++)
        {
            if(!parse::checkDynamicBigendian(buffer,offset,sc))return false;
            if(!Area1d::is_buffer_valid(buffer,offset))return false;
        }
        return true;
    }
    /**
     * this can throw error
     * there is no built in clear
     */
    void parse(const std::vector<unsigned char>& buffer,size_t& offset,UsedArea2d& areaget){
        using namespace zt::Internal;
        unsigned long long size;
        buffer_bigendian_to<unsigned long long>(buffer,offset,size);
        for (size_t i = 0; i < size; i++)
        {
            long long tempkey;
            buffer_bigendian_to<long long>(buffer,offset,tempkey);
            Area1d* temparea=new Area1d(buffer,offset,areaget.usedChunkIds.at(tempkey));
            chunks[tempkey]=temparea;
        }
    }
    void parse_additional(const std::vector<unsigned char>& buffer,size_t& offset,UsedArea2d& areaget){
        using namespace zt::Internal;
        unsigned long long size;
        buffer_bigendian_to<unsigned long long>(buffer,offset,size);
        for (size_t i = 0; i < size; i++)
        {
            long long tempkey;
            buffer_bigendian_to<long long>(buffer,offset,tempkey);
            auto itc=chunks.find(tempkey);
            if(itc!=chunks.end()){
                itc->second->parse_additional(buffer,offset,areaget.usedChunkIds.at(tempkey));
                continue;
            }
            chunks[tempkey]=new Area1d(buffer,offset,areaget.usedChunkIds.at(tempkey));
        }
    }
    Area2d(const std::vector<unsigned char>& buffer,size_t& offset,UsedArea2d& areaget){
        parse(buffer,offset,areaget);
    }
    //Area2d()=default;
};