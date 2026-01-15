#pragma once

#include "indeksArea.hpp"
//todo: bikin fungsi destroy untuk Area (1 dan 2) D untuk di pakai di parse()
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
        chunkmap* get_ptr(long long x){
            auto itc=chunks.find(x);
            if(itc!=chunks.end()){
                return itc->second;
            }
            return nullptr;
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
            std::cout<<"Setting chunk at x="<<x<<"\n";
            auto itc=chunks.find(x);
            if(itc!=chunks.end()){
                std::cout<<"Found existing chunk at x="<<x<<"\n";
                if(chunk==itc->second)return;
                    debug_print("Deleting existing chunk at x="<<x<<" at address:"<<(void*)itc->second<<"and replace it with:"<<(void*)chunk);
                delete itc->second;
            }
            chunks[x]=chunk;
            std::cout<<"pointer value: "<<(void*)chunk<<"\n";
        }
        inline bool delete_chunk(long long x,bool& is_empty){
            auto temp=chunks.find(x);
            if(temp!=chunks.end()){
                delete temp->second;
                chunks.erase(temp);
                is_empty=!chunks.size();
                return true;
            }
            is_empty=!chunks.size();
            return false;
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
        void dump_ref(std::vector<unsigned char>& buffer,IndeksArea1d& areaget){
            dynamic_to_buffer_bigendian(areaget.usedChunkIds.size(),buffer);
            std::vector<long long> tmp;
            for (auto a:areaget.usedChunkIds)
            {
               auto itc = chunks.find(a);
               if(itc!=chunks.end()){
                to_buffer_bigendian<long long>(a,buffer);
                itc->second->dump_ref(buffer);
                continue;
               }
               tmp.emplace_back(a);
            }
            for(auto tpi:tmp){
                areaget.usedChunkIds.erase(tpi);
            }
        }
        static bool is_buffer_valid(const std::vector<unsigned char>& buffer,size_t& offset){
            using namespace zt::Internal;
            unsigned long long size;
            unsigned char csize;
            if(!parse::checkDynamicBigendian(buffer,offset,csize))return false;
            offset--;
            buffer_bigendian_to_dynamic(buffer,offset,size);
            debug_print("Area1d::is_buffer_valid: found size="<<size);
            for (size_t i = 0; i < size; i++)
            {
                if(!parse::checkPrimitiveBigendian<long long>(buffer,offset))return 0;
                if(!chunkmap::is_buffer_valid(buffer,offset))return false;
            }
            return true;
        }
        void parse(const std::vector<unsigned char>& buffer,size_t& offset,IndeksArea1d& areaget){
            using namespace zt::Internal;
            unsigned long long size;
            long long key;
            clear();
            buffer_bigendian_to_dynamic(buffer,offset,size);
            debug_print("size x nya:"<<size);
            if(size>(256*4))throw std::exception("hell nah banyak bet cik");
            for (size_t i=0;i<size;i++)
            {
                buffer_bigendian_to<long long>(buffer,offset,key);
                debug_print("coord x nya:"<<key);
                if(!areaget.usedChunkIds.count(key))areaget.usedChunkIds.insert(key);
                chunkmap* temp=new chunkmap(buffer,offset);
                chunks[key]=temp;
            }
        }
        void parse_additional(const std::vector<unsigned char>& buffer,size_t& offset,IndeksArea1d& areaget){
            using namespace zt::Internal;
            unsigned long long size;
            long long key;
            buffer_bigendian_to_dynamic(buffer,offset,size);
            debug_print("size x nya:"<<size);
            if(size>(256*4))throw std::exception("hell nah banyak bet cik");
            for (size_t i = 0; i < size; i++)
            {
                buffer_bigendian_to<long long>(buffer,offset,key);
                debug_print("key x:"<<key)
                if(!areaget.usedChunkIds.count(key))areaget.usedChunkIds.insert(key);
                chunkmap* temp=new chunkmap(buffer,offset);
                auto itc =chunks.find(key);
                if(itc!=chunks.end()){
                    delete itc->second;
                    itc->second=temp;
                }else{
                    chunks[key]=temp;
                }
            }
        }
        Area1d(const std::vector<unsigned char>& buffer,size_t& offset,IndeksArea1d& areaget){
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
    chunkmap* get_ptr(long long x,long long y){
        auto temp=chunks.find(y);
        if(temp!=chunks.end()){
            return temp->second->get_ptr(x);
        }
        return nullptr;
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
    inline bool delete_chunk(long long x,long long y){
        auto& temp=chunks.find(y);
        bool is_empty;
        bool keluaran;
        if(temp!=chunks.end()){
            keluaran=temp->second->delete_chunk(x,is_empty);
            if(is_empty){
                chunks.erase(y);
            }
            return keluaran;
        }
    }
    Area2d(){
        chunks.reserve(1024);
    }
    void clear(){
        debug_print("clear called, chunks size:"<< chunks.size());
        for(auto& a:chunks){
            if(a.second){
                debug_print("delete y:"<<a.first);
                delete a.second;
                a.second=nullptr;
            }
        }
        chunks.clear();
    }
    ~Area2d(){
        clear();
    }
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
     */
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
    //Area2d()=default;
};