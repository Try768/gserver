#pragma once

#include "indeksArea.hpp"
struct Area2d{
    private:
    std::unordered_map<Coord<long long>,chunkmap*> chunks;
    public:
    using optChunkmap =zt::Internal::util::optional<chunkmap>;
    const optChunkmap& get_const(const Coord<long long>& chunkcoord)const{
        auto temp=chunks.find(chunkcoord);
        if(temp!=chunks.end()){
            return optChunkmap(temp->second);
        }
        return optChunkmap();
    }
    optChunkmap& get(const Coord<long long>& chunkcoord){
        auto temp=chunks.find(chunkcoord);
        if(temp!=chunks.end()){
            return optChunkmap(temp->second);
        }
        return optChunkmap();
    }
    chunkmap* get_ptr(const Coord<long long>& chunkcoord){
        auto temp=chunks.find(chunkcoord);
        if(temp!=chunks.end()){
            return temp->second;
        }
        return nullptr;
    }
    inline const bool has_chunk(const Coord<long long>& chunkcoord)const{
        auto temp=chunks.find(chunkcoord);
        if(temp!=chunks.end()){
            return true;
        }
        return false;
    }
    
    inline void create(const Coord<long long>& chunkcoord,const chunkmap& chunk){
        auto [itc,stat]=chunks.try_emplace(chunkcoord,nullptr);
        if(stat)itc->second=new chunkmap(chunk);
    }
    
    inline bool delete_chunk(const Coord<long long>& chunkcoord){
        auto& temp=chunks.find(chunkcoord);
        if(temp!=chunks.end()){
            delete temp->second;
            chunks.erase(chunkcoord);
            return true;
        }
        return false;
    }
    Area2d(){
        chunks.reserve(1024);
    }
    void clear(){
        debug_print("clear called, chunks size:"<< chunks.size());
        for(auto& a:chunks){
            if(a.second){
                debug_print("delete y:"<<a.first.x);
                delete a.second;
                a.second=nullptr;
            }
        }
        chunks.clear();
    }
    ~Area2d(){
        clear();
    }/*
    void dump_ref(std::vector<unsigned char>& buffer,IndeksArea2d& areaget){
        std::vector<long long> tmp;
        dynamic_to_buffer_bigendian(areaget.usedChunkIds.size(),buffer);
        debug_print("Dumping Area2d with "<<areaget.usedChunkIds.size()<<" rows");
        for(auto& xc:areaget.usedChunkIds){
            //dynamic_to_buffer_bigendian(plus.first,buffer);
            to_buffer_bigendian<long long>(xc.first,buffer);
            auto& itc=chunks.find(xc.first);
            if(itc==chunks.end()){
                //throw std::runtime_error("Area2d::dump_ref: missing Area1d for y="+std::to_string(xc.first));
                tmp.push_back(xc.first);
                continue;
            }
            debug_print("xc:"<<xc.first);
            chunks.at(xc.first)->dump_ref(buffer,xc.second);
        }
        for(auto tpi:tmp){
            debug_print("invalid used chunk y:"<<tpi);
            areaget.usedChunkIds.erase(tpi);
        }
    }
    static bool is_buffer_valid(const std::vector<unsigned char>& buffer,size_t& offset){
        using namespace zt::Internal;
        unsigned char sc;
        unsigned long long size;
        if(!parse::checkDynamicBigendian(buffer,offset,sc))return false;
        offset--;
        buffer_bigendian_to_dynamic(buffer,offset,size);
        debug_print("area2d checking size:"<<size);
        if(size>256*2)throw std::exception("kegedean cik satu file chunk lebih dari semestinya");
        for (size_t i = 0; i < size; i++)
        {
            if(!parse::checkPrimitiveBigendian<long long>(buffer,offset));
            if(!Area1d::is_buffer_valid(buffer,offset))return false;
        }
        return true;
    }
    /**
     * this can throw error
     * there is no built in clear
     *//*
    void parse(const std::vector<unsigned char>& buffer,size_t& offset,IndeksArea2d& areaget){
        using namespace zt::Internal;
        unsigned long long size;
        buffer_bigendian_to_dynamic(buffer,offset,size);
        if(size>256*2)throw std::exception("kegedean cik satu file chunk lebih dari semestinya");
        for (size_t i = 0; i < size; i++)
        {
            long long tempkey;
            buffer_bigendian_to<long long>(buffer,offset,tempkey);
            Area1d* temparea=new Area1d(buffer,offset,areaget.usedChunkIds.at(tempkey));
            chunks[tempkey]=temparea;
        }
    }
    void parse_additional(const std::vector<unsigned char>& buffer,size_t& offset,IndeksArea2d& areaget){
        using namespace zt::Internal;
        unsigned long long size;
        buffer_bigendian_to_dynamic(buffer,offset,size);
        debug_print("size parse y:"<<size)
        if(size>256*4)throw std::exception("kegedean cik satu file chunk lebih dari semestinya");
        for (size_t i = 0; i < size; i++)
        {
            long long tempkey;
            buffer_bigendian_to<long long>(buffer,offset,tempkey);
            debug_print("parse key y:"<<tempkey);
            auto itc=chunks.find(tempkey);
            if(itc!=chunks.end()){
                itc->second->parse_additional(buffer,offset,areaget.usedChunkIds[tempkey]);
                continue;
            }
            chunks[tempkey]=new Area1d(buffer,offset,areaget.usedChunkIds[tempkey]);
        }
    }
    Area2d(const std::vector<unsigned char>& buffer,size_t& offset,IndeksArea2d& areaget){
        parse(buffer,offset,areaget);
    }
    */
    //Area2d()=default;
};
/**
 * inline void set_ptr(long long x,long long y,chunkmap* chunk){
        if(chunk==nullptr)return;
        std::cout<<"Setting chunk at ("<<x<<","<<y<<")\n";
        auto itc=chunks.find(y);
        if(itc == chunks.end()){
            std::cout<<"not Found existing Area1d for y="<<y<<"\n";
            chunks[y]=new Area1d();
            itc=chunks.find(y);
        }
        itc->second->set_ptr(x,chunk);
    }
 */