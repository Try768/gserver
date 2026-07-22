#pragma once
#include "core/forward.hpp"
#include "core/internal/internal.hpp"
#include "core/entity/entity.hpp"
#include "core/internal/filemanager.hpp"
#include "core/internal/Permision.hpp"
#include "core/worker/pool.hpp"

#include <deque>
struct EntityEmiter{
    const zt::callback::EntityEventListener &emiter;
    static constexpr zt::event::entity::Type eventtick = zt::event::entity::Tick;
    zt::event::entity::params<zt::event::entity::Tick> param;
    bool emitMainEvent=true;
    Entity entity;
    void operator()();
};
namespace zt::command::entity{
     struct BasePtr {
        EntityData* entityPointer = nullptr;

        BasePtr() = default;
        BasePtr(EntityData* ptr)
            : entityPointer(ptr) {}
    };

    struct BaseID {
        unsigned long long ID = 0;

        BaseID() = default;
        BaseID(unsigned long long id)
            : ID(id) {}
    };

    struct EntityAddVelocity : public BasePtr {
        velo2 vel;

        EntityAddVelocity(EntityData* ptr, const velo2& v)
            : BasePtr(ptr), vel(v) {}
    };

    struct EntityTeleport : public BasePtr {
        Coordinat nPos;

        EntityTeleport(EntityData* ptr, const Coordinat& pos)
            : BasePtr(ptr), nPos(pos) {}
    };

    struct EntityChangeOrigin : public BasePtr ,public BaseID{
        OriginWorld origin;

        EntityChangeOrigin(EntityData* ptr,unsigned long long ID, const OriginWorld& o)
            : BasePtr(ptr),BaseID(ID), origin(o) {}
    };

    struct EntityChangeDynamicProperty : public BasePtr {
        std::string key;
        MultiValue val;

        EntityChangeDynamicProperty(
            EntityData* ptr,
            const std::string& k,
            const MultiValue& v)
            : BasePtr(ptr), key(k), val(v) {}
    };

    struct EntityRemove : public BaseID {

        EntityRemove(unsigned long long id)
            : BaseID(id) {}
    };

    struct EntityAdd : public BasePtr {
       

        EntityAdd(EntityData* ptr)
            : BasePtr(ptr) {}
    };

    struct commandList{
        std::vector<EntityAddVelocity> AddVelocity;
        std::vector<EntityChangeDynamicProperty> propertyChange;
        std::vector<EntityTeleport> teleport;
        std::vector<EntityChangeOrigin> origin;
        std::vector<EntityRemove> removeE;
        std::vector<EntityAdd> addE;
        inline void merge(const commandList& other){
            AddVelocity.insert(AddVelocity.end(),other.AddVelocity.begin(),other.AddVelocity.end());
            propertyChange.insert(propertyChange.end(),other.propertyChange.begin(),other.propertyChange.end());
            teleport.insert(teleport.end(),other.teleport.begin(),other.teleport.end());
            origin.insert(origin.end(),other.origin.begin(),other.origin.end());
            addE.insert(addE.end(),other.addE.begin(),other.addE.end());
            removeE.insert(removeE.end(),other.removeE.begin(),other.removeE.end());
        }
    };
    class commandParent{
        protected:
        friend class EntityManager;
        commandList list;
        public:
        inline void C_addVelocity(EntityData* data,const velo2& vel){
            list.AddVelocity.emplace_back(data,vel);
        }
        inline void C_propertyChange(EntityData* data,const std::string& key,MultiValue val){
            list.propertyChange.emplace_back(data,key,val);
        }
        inline void C_teleport(EntityData* data, Coordinat nPos){
            list.teleport.emplace_back(data,nPos);
        }
        inline void C_changeorigin(EntityData* data,unsigned long long ID, OriginWorld origin){
            list.teleport.emplace_back(data,ID,origin);
        }

        inline void C_removeE(unsigned long long id){
            list.removeE.emplace_back(id);
        }
        inline void C_addE(unsigned long long id, EntityData* data){
            list.addE.emplace_back(data);
        }
        inline void C_merge(const commandParent& other){
            list.merge(other.list);
        }
    };
};
class EntityManager:public zt::command::entity::commandParent{
    private:
        friend class chunkmap;
        OriginWorld origin;
        DimensionPermision dimentionpermit;
        const Registry& reg;
        using ID=unsigned long long;
        static inline IDMaker<unsigned long long> entityid;
        std::unordered_map<ID,EntityData*> entitybyID;
        fsmanager::db::LMDB database;
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
        ID copyEntity(const EntityData& data);
        ID moveEntity(EntityData* data);
        inline ID moveEntity(std::unique_ptr<EntityData> data){
            moveEntity(data.release());
        }
        
        //may throw error
        inline  std::pair<Entity,bool> getEntity(ID entityid){
            auto lentity=entitybyID.find(entityid);
            if(lentity==entitybyID.end())return std::pair<Entity,bool>(Entity(lentity->second,entityid,origin),false);
            return std::pair(Entity(lentity->second,entityid,origin),true);
        }
        inline decltype(entitybyID)::const_iterator end(){
            return entitybyID.end();
        }
        inline decltype(entitybyID)::const_iterator findEntityId(ID identity){
            return entitybyID.find(identity);
        }
        EntityManager(const OriginWorld& origin,DimensionPermision permit,const Registry& reg,
            const std::string& world_dir):
            origin(origin),dimentionpermit(permit),reg(reg),database(world_dir+"/entity_map",1204*128){}
        EntityManager(OriginWorld&& origin ,DimensionPermision&& permit,const Registry& reg,
            std::string&& world_dir):
            origin(origin),dimentionpermit(permit),reg(reg),database(world_dir+"/entity_map",1204*128){}
        bool delEntity(ID id);
        bool releaseEntity(ID id);
        using time_point=std::chrono::steady_clock::time_point;
        void simulate(time_point time_pivot);
        void flush();
        private:
        void addVelocity();
        void propertyChange();
        void ChangeOrigin();
        void Teleport();
        void AddEntity();
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