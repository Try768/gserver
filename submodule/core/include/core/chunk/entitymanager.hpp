#pragma once
#include "../register/register.hpp"
#include "../tile/list.hpp"
#include "forward.hpp"
//#include "../entity/list.hpp"
#include <deque>
#include "../entity/entity.hpp"

class EntityManager{
        private:
        friend class chunkmap;
        Room& room;
        using ID=unsigned long long;
        static IDMaker<unsigned long long> entityid;
        static std::unordered_map<ID,EntityData*> entitybyID;
        public:
        struct ChunkEntity{
            private:
            friend class EntityManager;
            std::unordered_set<EntityManager::ID> entityInChunk;
            bool dirty=true;
            bool loaded=true;
            public:
            ChunkEntity()=default;
            ChunkEntity(ID id){entityInChunk.emplace(id);}
            void markDirty(){dirty=true;}
            inline void cleanup(){
                for (auto it = entityInChunk.begin(); it != entityInChunk.end(); ) {
                    if (EntityManager::findEntityId(*it) == EntityManager::end()) {
                        it = entityInChunk.erase(it);
                    } else {
                         ++it;
                    }
                }
                dirty=false;
            }
            std::vector<EntityManager::ID> getentityIDinchunk(){
                if(dirty)cleanup();
                return std::vector<ID>(entityInChunk.begin(),entityInChunk.end());
            }
        };
        private:
        static std::unordered_map<Coord<long long>,ChunkEntity> chunks;
        static void cleanUpEvent(Entity& data,const Coord<long long> &chunk);
        public:
        static ID createEntity(const EntityData& data);
        //may throw error
        inline static const std::pair<Entity,bool> getEntity(ID entityid){
            auto& lentity=entitybyID.find(entityid);
            if(lentity==entitybyID.end())return std::pair(Entity(lentity->second,entityid),false);
            return std::pair(Entity(lentity->second,entityid),true);
        }
        inline static decltype(entitybyID)::const_iterator end(){
            return entitybyID.end();
        }
        inline static decltype(entitybyID)::const_iterator findEntityId(ID identity){
            return entitybyID.find(identity);
        }
        static bool delEntity(ID id);
        static void simulate();

    };
//inline static bool setEntity(decltype(entitybyID)::const_iterator itc,Entity&& entity){
        //    if(itc==entitybyID.end())return false;
        //    auto it= entitybyID.find(itc->first);
        //    it->second.first=entity.get_name();
        //    it->second.second=&entity.data;
        //    return true;
        //};
        //std::forward_as_tuple std::piecewise_construct
 //ID duplicateEntity(EntityManager::ID);
        //ID duplicateEntity(const EntityData& entity){return duplicateEntity(EntityData(entity));}
 //inline static bool setEntity(decltype(entitybyID)::const_iterator itc,const Entity& entity){
        //    return setEntity(itc,Entity(entity));
        //}