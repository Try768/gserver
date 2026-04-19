#pragma once
#include <fstream>
#include <filesystem>
#include <vector>
#include <string>
#include <sstream>
#include <stdexcept>
#include <cstring>
#include "lmdb.h"
namespace fsmanager{
    namespace db{
        class LMDB {
        private:
            MDB_env* env = nullptr;
            MDB_dbi dbi;
            void check(int rc) {
                if (rc != 0)
                    throw std::runtime_error(mdb_strerror(rc));
            }
        
        public:
            LMDB(const std::string& path, size_t mapsize) {
                check(mdb_env_create(&env));
                check(mdb_env_set_maxdbs(env, 1));
                check(mdb_env_set_mapsize(env, mapsize));
                check(mdb_env_open(env, path.c_str(), 0, 0664));
            
                MDB_txn* txn;
                check(mdb_txn_begin(env, nullptr, 0, &txn));
                check(mdb_dbi_open(txn, nullptr, MDB_CREATE, &dbi));
                check(mdb_txn_commit(txn));
            }
        
            ~LMDB() {
                mdb_dbi_close(env, dbi);
                mdb_env_close(env);
            }
            // ---- WRITE ----
            void __put(const std::string& key,
                     const void* data_ptr,
                     size_t size)
            {
                MDB_txn* txn;
                check(mdb_txn_begin(env, nullptr, 0, &txn));
            
                MDB_val k, v;
                k.mv_size = key.size();
                k.mv_data = (void*)key.data();
            
                v.mv_size = size;
                v.mv_data = (void*)data_ptr;
            
                check(mdb_put(txn, dbi, &k, &v, 0));
                check(mdb_txn_commit(txn));
            }
        
            // overload untuk string
            void put(const std::string& key,
                     const std::string& value)
            {
                //if(value.size()+_used>_size){
                //    auto half_size = value.size()-(_size-_used);
                //    mdb_env_set_mapsize(env,(half_size*half_size));
                //}
                __put(key, value.data(), value.size());
            }
            void put(const std::string& key,
                     const std::vector<uint8_t>& value)
            {
                __put(key, value.data(), value.size());
            }
        
            // ---- READ ----
            std::vector<uint8_t> get(const std::string& key) {
                MDB_txn* txn;
                check(mdb_txn_begin(env, nullptr, MDB_RDONLY, &txn));
            
                MDB_val k, v;
                k.mv_size = key.size();
                k.mv_data = (void*)key.data();
            
                int rc = mdb_get(txn, dbi, &k, &v);
                if (rc == MDB_NOTFOUND) {
                    mdb_txn_abort(txn);
                    return {};
                }
            
                check(rc);
            
                std::vector<uint8_t> result(
                    (uint8_t*)v.mv_data,
                    (uint8_t*)v.mv_data + v.mv_size
                );
            
                mdb_txn_abort(txn);
                return result;
            }
        };
    }
    namespace file{
        std::string read(std::string path){
            std::ifstream filer(path);
            if (!filer)
            {
                return "";
            }
            std::string buffer;
            filer.seekg(0, std::ios::end);
            buffer.resize(filer.tellg());
            filer.seekg(0, std::ios::beg);
            filer.read(&buffer[0], buffer.size());
            filer.close();
            return buffer;
        }
        bool write(std::string path, const std::string& data){
            std::ofstream filer(path);
            if (!filer)
            {
                return false;
            }
            filer.write(data.c_str(), data.size());
            filer.close();
            return true;
        }
        std::vector<unsigned char> readbin(std::string path){
            std::vector<unsigned char> buffer;
            std::ifstream filer(path,std::ios::binary);
            if (!filer)
            {
                return buffer;
            }
            filer.seekg(0, std::ios::end);
            buffer.resize(filer.tellg());
            filer.seekg(0, std::ios::beg);
            filer.read(reinterpret_cast<char*>(buffer.data()), buffer.size());
            return buffer;
        }
        bool writebin(std::string path, const std::vector<unsigned char>& data){
            std::ofstream filer(path, std::ios::binary);
            if (!filer)
            {
                return false;
            }
            filer.write(reinterpret_cast<const char*>(data.data()), data.size());
            return true;
        }
        bool exists(std::string path){
            return std::filesystem::exists(path)&& !std::filesystem::is_directory(path);
        }
        bool remove(std::string path){
            return std::filesystem::remove(path);
        }
    }
    namespace directory{
        bool exists(std::string path){
            return std::filesystem::exists(path) && std::filesystem::is_directory(path);
        }
        bool remove(std::string path){
            return std::filesystem::remove_all(path);
        }
        bool create(std::string path){
            return std::filesystem::create_directory(path);
        }
        std::vector<std::string> list_files_in_directory(std::string path){
            std::vector<std::string> filelist;
            for (const auto& entry : std::filesystem::directory_iterator(path))
            {
                if(!std::filesystem::is_directory(entry.path())){
                    filelist.push_back(entry.path().filename().string());
                }
            }
            return filelist;
        }
    }
}