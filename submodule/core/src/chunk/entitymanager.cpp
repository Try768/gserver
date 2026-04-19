#include "core/register/register.hpp"
#include "core/tile/list.hpp"
 #include "core/entity/list.hpp"
 
 #include "core/chunk/entitymanager.hpp"
 #include "core/chunk/chunk.hpp"
 #include "world/world.hpp"
 #include "core/eventListener/eventTable.hpp"
//EntityManager::ID EntityManager::duplicateEntity(EntityData&& data){
//    ID tempid;
//    if(!entityid.getID(tempid))throw std::exception("alright alright thats enough entities");
//    auto [it,inserted]=entitybyID.try_emplace(tempid,(nullptr));
//    if(inserted){it->second=new EntityData(data);}
//    return tempid;
//}

EntityManager::ID EntityManager::createEntity(const EntityData& data){
    ID tempid;
    if(!entityid.getID(tempid))throw std::exception("alright alright thats enough entities");
    auto [it,inserted]=entitybyID.try_emplace(tempid,(nullptr));
    if(inserted){it->second=new EntityData(data);}
    auto itch=chunks.find(data.getchunkcoord());
    if(itch!=chunks.end()){
        itch->second.entityInChunk.emplace(tempid);
    }else{
        chunks.try_emplace(data.getchunkcoord(),tempid);
    }
    return tempid;
}
bool EntityManager::delEntity(ID id){
    auto itc=entitybyID.find(id);
    if(itc==entitybyID.end())return false;
    auto itch=chunks.find(itc->second->getchunkcoord());
    if(itch!=chunks.end()){
        itch->second.entityInChunk.erase(id);
    }
    entitybyID.erase(id);
    return true;
}
void EntityManager::cleanUpEvent(Entity& data,
    const Coord<long long> &chunk){
    //applying impuls
    
        // chunk correction
    if((data.getCoordinat().getGlobal().x!=chunk.x)||(data.getCoordinat().getGlobal().y!=chunk.y)){
        EntityManager::chunks[data.getCoordinat().getGlobal()].entityInChunk.emplace(data.ID);
        auto& tc= EntityManager::chunks[chunk];
        tc.entityInChunk.erase(data.ID);
        if(tc.entityInChunk.empty())EntityManager::chunks.erase(chunk);
    }
}
void EntityManager::simulate(Registry& reg){
    constexpr auto eventtick=zt::event::entity::Type::Tick;
    const auto& emiter=reg.getEEL();
    for(auto& entitiesinchunk:chunks){
        if(!entitiesinchunk.second.loaded)continue;
        for(auto entityid=entitiesinchunk.second.entityInChunk.begin();
        entityid!=entitiesinchunk.second.entityInChunk.end();){
            auto [entity,status]=EntityManager::getEntity(*entityid);
            if(!status){
                entityid=entitiesinchunk.second.entityInChunk.erase(entityid);continue;
            }
            SimulatedEntity simulatedEntity(entity);
            bool emitMainEvent=true;
            //before event
            emiter.getBeforeEvent<eventtick>().emit({simulatedEntity},emitMainEvent);
            //componnent run
            if(emitMainEvent){
                emiter.getComponent<eventtick>().emit(
                    {simulatedEntity},entity.getEntityComponent()
                );
                
            }
            //after event
            emiter.getAfterEvent<eventtick>().emit(
                {simulatedEntity}
            );
            cleanUpEvent(simulatedEntity.getEntity(),entitiesinchunk.first);//wip
            ++entityid;
        }
    }
}