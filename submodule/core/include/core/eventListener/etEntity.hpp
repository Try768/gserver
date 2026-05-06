#pragma once
#include "etheader.hpp"
namespace zt::callback
{
    class EntityEventListener;
    using etype=zt::event::entity::Type;
    template<zt::event::entity::Type T>
    class BeforeEventEntity{
        friend class Registry;
        friend class EntityEventListener;
        public:
        using ID=unsigned long long;
        using func=void(*)(zt::event::entity::params<T>&,bool&);
        private:
        IDMaker<ID> maker;
        std::vector<std::pair<ID,func>> queueOfEvent;
        //std::unordered_map<ID,void(*)(zt::event::entity::params<T>&,bool&)> EventbyIterator;
        public:
        BeforeEventEntity(){}
        ID EventSubscribe(void(*eventRes)(zt::event::entity::params<T>&,bool&)){
            ID id;
            if(!maker.getID(id))throw std::logic_error("what the ... how many event that you made?dude!");
            queueOfEvent.emplace_back(id,eventRes);
            return id;
        }
        bool EventUnsubscribe(ID eventId){
            for(auto it=queueOfEvent.begin();it!=queueOfEvent.end();){
                if(it->first ==eventId){
                    queueOfEvent.erase(it);
                    maker.destroyID(eventId);
                    return true;
                }else ++it;
            }return false;
        }
        void emit(zt::event::entity::params<T>&& params,bool& emitdefault)const{
            for (size_t i = 0; i < queueOfEvent.size(); ){
                auto id = queueOfEvent[i].first;
                auto handler = queueOfEvent[i].second;
                handler(params,emitdefault);
                if (i < queueOfEvent.size() && queueOfEvent[i].first == id)
                    ++i;
            }
        }
        void emit(zt::event::entity::params<T>& params,bool& emitdefault)const{
            for (size_t i = 0; i < queueOfEvent.size(); ){
                auto id = queueOfEvent[i].first;
                auto handler = queueOfEvent[i].second;
                handler(params,emitdefault);
                if (i < queueOfEvent.size() && queueOfEvent[i].first == id)
                    ++i;
            }
        }
    };
    //zt::event::entity::Type::Tick
    template<zt::event::entity::Type T>
    class AfterEventEntity{
        friend class Registry;
        friend class EntityEventListener;
        public:
        using ID=unsigned long long;
        private:
        IDMaker<ID> maker;
        std::vector<std::pair<ID,void(*)(zt::event::entity::params<T>&)>> queueOfEvent;
        //std::unordered_map<ID,)> EventbyIterator;
        public:
        AfterEventEntity()=default;
        ID EventSubscribe(void(*eventRes)(zt::event::entity::params<T>&)){
            ID id;
            if(!maker.getID(id))throw std::logic_error("what the ... how many event that you made?dude!");
            queueOfEvent.emplace_back(id,eventRes);
            return id;
        }
        bool EventUnsubscribe(ID eventId){
            for(auto it=queueOfEvent.begin();it!=queueOfEvent.end();){
                if(it->first ==eventId){
                    queueOfEvent.erase(it);//intented
                    maker.destroyID(eventId);
                    return true;
                }else ++it;
            }
            return false;
        }
        void emit(zt::event::entity::params<T>&& params)const{
            for (size_t i = 0; i < queueOfEvent.size(); ){
                auto id = queueOfEvent[i].first;
                auto handler = queueOfEvent[i].second;
                handler(params);
                if (i < queueOfEvent.size() && queueOfEvent[i].first == id)
                    ++i;
            }
        }
        void emit(zt::event::entity::params<T>& params)const{
            for (size_t i = 0; i < queueOfEvent.size(); ){
                auto id = queueOfEvent[i].first;
                auto handler = queueOfEvent[i].second;
                handler(params);
                if (i < queueOfEvent.size() && queueOfEvent[i].first == id)
                    ++i;
            }
        }
    };
    template<zt::event::entity::Type T>
    class ComponentRegisterofEntity{
        friend class EntityEventListener;
        using Index=size_t;
        Index index;
        std::unordered_map<std::string,Index> ComponentID;
        std::vector<void(*)(zt::event::entity::params<T>&)> ComponentByIndeks;
        public:
        ComponentRegisterofEntity(){}
        bool registerComponent(std::string ComponentName,void(*eventRes)(zt::event::entity::params<T>&)){
            if(ComponentID.count(ComponentName))return false;
            if(ComponentByIndeks.size()==std::numeric_limits<size_t>::max)throw std::exception("dude how many Component that you wanted to add?!");
            auto [itc,stat]=ComponentID.try_emplace(ComponentName,ComponentByIndeks.size());
            if(stat){ComponentByIndeks.push_back(eventRes);}else return false;
            return true;
        }
        void connect(EntityComponent& component){
            component.runComponent[T].clear();
            for(const auto& itc :ComponentID){
                auto iter =component.getInternal().const_component.find(itc.first);
                if(iter!=component.getInternal().const_component.end()){
                    component.internal.runComponent[T].push_back(itc.second);
                }
            }
        }
        void emit(zt::event::entity::params<T>&& params,const EntityComponent& component)const{
            const auto& itc =component.internal.runComponent[T];
            for(auto ind:itc){
                assert(ind<ComponentByIndeks.size());
                //if(ind>=ComponentByIndeks.size())throw std::logic_error("hmm i wonder where it come from");
                (*ComponentByIndeks[ind])(params);
            }
        }
        void emit(zt::event::entity::params<T>& params,const EntityComponent& component)const{
            const auto& itc =component.internal.runComponent[T];
            for(auto ind:itc){
                assert(ind<ComponentByIndeks.size());
                //if(ind>=ComponentByIndeks.size())throw std::logic_error("hmm i wonder where it come from");
                (*ComponentByIndeks[ind])(params);
            }
        }
    };
    
    class EntityEventListener{
        private:
        friend class Registry;
        EntityEventListener(){}
        std::tuple<
            AfterEventEntity<etype::EntityDied>,AfterEventEntity<etype::EntityHit>,
            AfterEventEntity<etype::EntityHitPlayer>,AfterEventEntity<etype::EntityJump>,
            AfterEventEntity<etype::EntityWalk>,AfterEventEntity<etype::EntityHurt>,
            AfterEventEntity<etype::EntitySpawn>,AfterEventEntity<etype::InvalidData>,
            AfterEventEntity<etype::PlayerInteractWithEntity>,AfterEventEntity<etype::Tick>
        > afterEvent;
        std::tuple<
            BeforeEventEntity<etype::EntityDied>,BeforeEventEntity<etype::EntityHit>,
            BeforeEventEntity<etype::EntityHitPlayer>,BeforeEventEntity<etype::EntityJump>,
            BeforeEventEntity<etype::EntityWalk>,BeforeEventEntity<etype::EntityHurt>,
            BeforeEventEntity<etype::EntitySpawn>,BeforeEventEntity<etype::InvalidData>,
            BeforeEventEntity<etype::Tick>,BeforeEventEntity<etype::PlayerInteractWithEntity>
        > beforeEvent;
        std::tuple<
            ComponentRegisterofEntity<etype::EntityDied>,ComponentRegisterofEntity<etype::EntityHit>,
            ComponentRegisterofEntity<etype::EntityHitPlayer>,ComponentRegisterofEntity<etype::EntityJump>,
            ComponentRegisterofEntity<etype::EntityWalk>,ComponentRegisterofEntity<etype::EntityHurt>,
            ComponentRegisterofEntity<etype::EntitySpawn>,ComponentRegisterofEntity<etype::InvalidData>,
            ComponentRegisterofEntity<etype::Tick>,ComponentRegisterofEntity<etype::PlayerInteractWithEntity>
        > component;
        public:

        template<etype T> const AfterEventEntity<T>& getAfterEvent()const{return std::get<AfterEventEntity<T>>(afterEvent);}
        template<etype T> const BeforeEventEntity<T>& getBeforeEvent()const{return std::get<BeforeEventEntity<T>>(beforeEvent);}
        template<etype T> const ComponentRegisterofEntity<T>& getComponent()const{return std::get<ComponentRegisterofEntity<T>>(this->component);}
    };
    template<zt::event::entity::Type T>
    void entityEmit(zt::event::entity::params<T> params,EntityComponent component,EntityEventListener& listener){
        bool emitdefault=true;
        listener.getBeforeEvent<T>().emit(params,emitdefault);
        if(emitdefault)listener.getComponent<T>().emit(params,component);
        listener.getAfterEvent<T>().emit(params);
    }
} 