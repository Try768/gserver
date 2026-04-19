#pragma once
#include "core/forward.hpp"
#include "core/internal/internal.hpp"
#include "core/entity/entity.hpp"
#include <deque>

class EntityManager{
        private:
        friend class chunkmap;
        Dimension& dimension;
        using ID=unsigned long long;
        IDMaker<unsigned long long> entityid;
        std::unordered_map<ID,EntityData*> entitybyID;
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
            inline void cleanup(EntityManager& em){
                for (auto it = entityInChunk.begin(); it != entityInChunk.end(); ) {
                    if (em.findEntityId(*it) == em.end()) {
                        it = entityInChunk.erase(it);
                    } else {
                         ++it;
                    }
                }
                dirty=false;
            }
            std::vector<EntityManager::ID> getentityIDinchunk(EntityManager& em){
                if(dirty)cleanup(em);
                return std::vector<ID>(entityInChunk.begin(),entityInChunk.end());
            }
        };

        private:
         std::unordered_map<Coord<long long>,ChunkEntity> chunks;
         void cleanUpEvent(Entity& data,const Coord<long long> &chunk);
        public:
         ID createEntity(const EntityData& data);
        //may throw error
        inline  const std::pair<Entity,bool> getEntity(ID entityid){
            auto& lentity=entitybyID.find(entityid);
            if(lentity==entitybyID.end())return std::pair(Entity(lentity->second,entityid,dimension),false);
            return std::pair(Entity(lentity->second,entityid,dimension),true);
        }
        inline decltype(entitybyID)::const_iterator end(){
            return entitybyID.end();
        }
        inline decltype(entitybyID)::const_iterator findEntityId(ID identity){
            return entitybyID.find(identity);
        }
        EntityManager(Dimension& dimension):dimension(dimension){}
         bool delEntity(ID id);
         using time_point=std::chrono::steady_clock::time_point;
         void simulate(Registry& reg,time_point time_pivot);

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