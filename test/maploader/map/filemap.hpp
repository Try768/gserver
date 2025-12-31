#pragma once
#include "Area.hpp"
#include "../internal/datatype.hpp"
#include "option.hpp"
#include "../internal/filemanager.hpp"
class MapFile:public checksumparent
{
    private:
    public:
    std::string mapName;//tidak di dump
    std::string mapDir;//tidak di dump
    std::string idfile;
    Coord<long long> domain;
    constexpr static std::array<unsigned char,8> mapSignature ={1,2,3,255,23,45,67,89};
    Area2d* area;
    //from domain(x,y) to domain(x+16,y+16)
    UsedArea2d* usedAreainfile;
    std::vector<unsigned char> dump(){
        std::vector<unsigned char> keluaran;
        keluaran.insert(keluaran.end(),mapSignature.begin(),mapSignature.end());
        string_short_to_buffer_bigendian(idfile,keluaran); 
        to_buffer_bigendian<unsigned long long>(getcheksum(),keluaran);
        array_to_buffer_bigendian(bufferdump(),keluaran);
        return keluaran;
    }
    std::vector<unsigned char> bufferdump()override{
        std::vector<unsigned char> keluaran;
        bufferdump_ref(keluaran);
        return keluaran;
    }
    void bufferdump_ref(std::vector<unsigned char>& buffer){
        to_buffer_bigendian(domain.x,buffer);
        to_buffer_bigendian(domain.y,buffer);
        registry::fulldump(buffer);
        usedAreainfile->dump_ref(buffer,domain);
        area->dump_ref(buffer,usedAreainfile->trimed(domain));
    }
    static bool is_Data_buffer_valid(const std::vector<unsigned char>& data,size_t& offset){
        using namespace zt::Internal;
        if(!parse::checkPrimitiveBigendian<long long>(data,offset))return false;
        if(!parse::checkPrimitiveBigendian<long long>(data,offset))return false;
        if(!registry::is_buffer_valid(data,offset))return false;
        if(!UsedArea2d::is_buffer_valid(data,offset))return false;
        if(!Area2d::is_buffer_valid(data,offset))return false;
        return true;
    }
    static bool is_buffer_valid(const std::vector<unsigned char>& data,size_t& offset){
        using namespace zt::Internal;
        if(data.size()<mapSignature.size())return false;
        for(size_t i=0;i<mapSignature.size();i++){
            if(data[offset++]!=mapSignature[i])return false;
        }
        unsigned char clen;unsigned long long len;
        if(!parse::checkStringBigendian(data,offset,len))return false;
        offset+=len;
        if(!parse::checkPrimitiveBigendian<unsigned long long>(data,offset))return false;
        offset-=sizeof(unsigned long long);unsigned long long sum;
        buffer_bigendian_to<unsigned long long>(data,offset,sum);
        if(!parse::checkArrayBigendian(data,offset,len))return false;
        verifycheksum(data.begin()+offset,data.begin()+offset+len,sum);
        if(!is_Data_buffer_valid(data,offset))return false;
        return true;
    }
    void dataParse(const std::vector<unsigned char>& data,size_t& offset){
        using namespace zt::Internal;
        buffer_bigendian_to<long long>(data,offset,domain.x);
        buffer_bigendian_to<long long>(data,offset,domain.y);
        registry::fullparse(data,offset);
        usedAreainfile->parse(data,offset);
        area->parse(data,offset,*usedAreainfile);
    }
    //this function may throw errors
    void parse(const std::vector<unsigned char>& buffer,size_t& offset){
        if(buffer.size()<mapSignature.size())throw std::runtime_error("Map signature invalid");
        for(size_t i=0;i<mapSignature.size();i++){
            if(buffer[offset++]!=mapSignature[i])throw std::runtime_error("Map signature invalid");
        }
        buffer_bigendian_to_string_short(buffer,offset,idfile);
        unsigned char clen;unsigned long long len;unsigned long long sum;
        buffer_bigendian_to<unsigned long long>(buffer,offset,sum);
        std::vector<unsigned char> temp;
        buffer_bigendian_to_array(buffer,offset,temp);
        if(!verifycheksum(temp.begin(),temp.end(),sum))throw std::runtime_error("Map checksum invalid");
        dataParse(temp,offset);
    }
    std::vector<unsigned char> startup(){
        std::vector<unsigned char> keluaran;
        keluaran.insert(keluaran.end(),mapSignature.begin(),mapSignature.end());
        string_short_to_buffer_bigendian(idfile,keluaran);
        keluaran.push_back(10);
        dynamic_to_buffer_bigendian(0,keluaran);
        return keluaran;
    }
    MapFile(std::string name,std::string dir,Area2d* area_in):mapName(name),mapDir(dir){
        this->area=area_in;
        if(!fsmanager::file::exists(dir+name+".map")){
            fsmanager::file::writebin(dir+name+".map",startup());
        }
    }
    MapFile(std::string name,std::string dir,Area2d* area_in,const std::vector<unsigned char>& buffer,size_t& offset):mapName(name),mapDir(dir){
        this->area=area_in;
        parse(buffer,offset);
    }
    MapFile(){}
};
class MapFileParent{
    private:
    std::string mapDir;
    MapOption* opsi;
    std::unordered_map<std::string,MapFile*> mapFiles;
    public:
    void undirty_all_chunks(MapFile* mapfile){
        for(auto& a:mapfile->usedAreainfile->usedChunkIds){
            for(auto& b:a.second.usedChunkIds){
                //mark all chunk as saved
                chunkmap& chunk = mapfile->area->get(a.first, b);
                chunk.dirty = false;

            }
        }
    }
    void dump_ref(std::vector<unsigned char>& buffer,size_t& offset){
        registry::fulldump(buffer);
    }
    void save_file(MapFile* mapfile){
        std::vector<unsigned char> dumpdata=mapfile->dump();
        fsmanager::file::writebin(mapfile->mapDir+mapfile->mapName+".map",dumpdata);
        undirty_all_chunks(mapfile);
    }
    void set_option(MapOption& opsi){
        this->opsi=&opsi;
    }
    void save_chunk(long long x,long long y){
        std::string target="sch"+std::to_string(x/64)+std::string("_")+std::to_string(y/64)+".map";
        auto& mapfileptr=mapFiles[target];
        if(!mapfileptr){
            mapfileptr=new MapFile(target,mapDir+"/chunks/",new Area2d());
        }
        if(!mapfileptr->usedAreainfile->is_found(x,y)){
            mapfileptr->usedAreainfile->usedChunkIds[y].usedChunkIds.insert(x);
        }
        save_file(mapfileptr);
    }
    void unload_chunk(long long x,long long y){
        for(auto& mapfilepair:mapFiles){
            auto& mapfileptr=mapfilepair.second;
            if(mapfileptr->usedAreainfile->is_found(x,y)){
                save_file(mapfileptr);
                mapfileptr->area->delete_chunk(x,y);
            }
        }
    }
    bool is_file_valid(const std::string& filename){
        if(!fsmanager::file::exists(mapDir+"/chunks/"+filename))return false;
        std::vector<unsigned char> buffer=fsmanager::file::readbin(mapDir+"/chunks/"+filename);
        size_t offset=0;
        return MapFile::is_buffer_valid(buffer,offset);
    }
    bool find_chunk(long long x,long long y){
        for(auto& mapfilepair:mapFiles){
            auto& mapfileptr=mapfilepair.second;
            if(mapfileptr->usedAreainfile->is_found(x,y)){
                return true;
            }
        }
        return false;
    }
    void parse_file(const std::string& filename){
        std::vector<unsigned char> buffer=fsmanager::file::readbin(mapDir+"/chunks/"+filename);
        size_t offset=0;
        MapFile* mapfileptr=new MapFile(filename,mapDir+"/chunks/",new Area2d(),buffer,offset);
        mapFiles[filename]=mapfileptr;
    }
    
    void parse_dir(const std::string& dir){
        mapDir=dir;
        if(!fsmanager::directory::exists(dir+"/chunks/")){
            fsmanager::directory::create(dir+"/chunks/");
        }
        auto filelist=fsmanager::directory::list_files_in_directory(dir+"/chunks/");
        for(auto& filename:filelist){
            parse_file(filename);
        }
    }
    //op
    MapFileParent(std::string dir,MapOption& opsi_in):mapDir(dir){
        opsi=&opsi_in;
        parse_dir(dir);
    }
    MapFileParent()=default;
};
