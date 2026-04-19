#pragma once
#include "etheader.hpp"
namespace zt::callback
{
    using ptype=zt::event::player::Type;
    template<ptype T>
    class BeforeEventPlayer{
        friend class Registry;
        public:
        using ID=unsigned long long;
        using func=void(*)(zt::event::player::params<T>&,bool&);
        private:
        IDMaker<ID> maker;
        std::vector<std::pair<ID,func>> queueOfEvent;
        //std::unordered_map<ID,void(*)(zt::event::entity::params<T>&,bool&)> EventbyIterator;
        public:
        BeforeEventPlayer(){}
        ID EventSubscribe(void(*eventRes)(zt::event::player::params<T>&,bool&)){
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
        void emit(zt::event::player::params<T> params,bool& emitdefault)const{
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
    template<ptype T>
    class AfterEventPlayer{
        friend class Registry;
        public:
        using ID=unsigned long long;
        private:
        IDMaker<ID> maker;
        std::vector<std::pair<ID,void(*)(zt::event::player::params<T>&)>> queueOfEvent;
        //std::unordered_map<ID,)> EventbyIterator;
        public:
        AfterEventPlayer(){}
        ID EventSubscribe(void(*eventRes)(zt::event::player::params<T>&)){
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
        void emit(zt::event::player::params<T> params)const{
            for (size_t i = 0; i < queueOfEvent.size(); ){
                auto id = queueOfEvent[i].first;
                auto handler = queueOfEvent[i].second;
                handler(params);
                if (i < queueOfEvent.size() && queueOfEvent[i].first == id)
                    ++i;
            }
        }
    };
    template<ptype T>
    class ComponentRegisterofPlayer{
        using Index=size_t;
        Index index;
        std::unordered_map<std::string,Index> ComponentID;
        std::vector<void(*)(zt::event::player::params<T>&)> ComponentByIndeks;
        public:
        bool registerComponent(std::string ComponentName,void(*eventRes)(zt::event::player::params<T>&)){
            if(ComponentID.count(ComponentName))return false;
            if(ComponentByIndeks.size()==std::numeric_limits<size_t>::max)throw std::exception("dude how many Component that you wanted to add?!");
            auto [itc,stat]=ComponentID.try_emplace(ComponentName,ComponentByIndeks.size());
            if(stat){ComponentByIndeks.push_back(eventRes);}else return false;
            return true;
        }
        void connect(PlayerComponent& component){
            component.runComponent[T].clear();
            for(const auto& itc :ComponentID){
                auto iter =component.get_internal_component().find(itc.first);
                if(iter!=component.get_internal_component().end()){
                    component.internal.runComponent[T].push_back(itc.second);
                }
            }
        }
        void emit(zt::event::player::params<T> params,const PlayerComponent& component)const{
            const auto& itc =component.runComponent[T];
            for(auto ind:itc){
                assert(ind<ComponentByIndeks.size());
                //if(ind>=ComponentByIndeks.size())throw std::logic_error("hmm i wonder where it come from");
                ComponentByIndeks[ind](params);
            }
        }
    };
    
    class PlayerEventListener{
        private:
        friend class Registry;
        PlayerEventListener(){}
        std::tuple<
            AfterEventPlayer<ptype::invalidData>,AfterEventPlayer<ptype::PlayerConnect>,
            AfterEventPlayer<ptype::PlayerDied>,AfterEventPlayer<ptype::PlayerDisconnect>,
            AfterEventPlayer<ptype::PlayerHitEntity>,AfterEventPlayer<ptype::PlayerHurt>,
            AfterEventPlayer<ptype::PlayerInput>,AfterEventPlayer<ptype::PlayerInteractWithPlayer>,
            AfterEventPlayer<ptype::PlayerSpawn>,AfterEventPlayer<ptype::Tick>
        > afterEvent;
        std::tuple<
            BeforeEventPlayer<ptype::invalidData>,BeforeEventPlayer<ptype::PlayerConnect>,
            BeforeEventPlayer<ptype::PlayerDied>,BeforeEventPlayer<ptype::PlayerDisconnect>,
            BeforeEventPlayer<ptype::PlayerHitEntity>,BeforeEventPlayer<ptype::PlayerHurt>,
            BeforeEventPlayer<ptype::PlayerInput>,BeforeEventPlayer<ptype::PlayerInteractWithPlayer>,
            BeforeEventPlayer<ptype::PlayerSpawn>,BeforeEventPlayer<ptype::Tick>
        > beforeEvent;
        std::tuple<
            ComponentRegisterofPlayer<ptype::invalidData>,ComponentRegisterofPlayer<ptype::PlayerConnect>,
            ComponentRegisterofPlayer<ptype::PlayerDied>,ComponentRegisterofPlayer<ptype::PlayerDisconnect>,
            ComponentRegisterofPlayer<ptype::PlayerHitEntity>,ComponentRegisterofPlayer<ptype::PlayerHurt>,
            ComponentRegisterofPlayer<ptype::PlayerInput>,ComponentRegisterofPlayer<ptype::PlayerInteractWithPlayer>,
            ComponentRegisterofPlayer<ptype::PlayerSpawn>,ComponentRegisterofPlayer<ptype::Tick>
        > component;
        public:
        template<ptype T> const AfterEventPlayer<T>& getAfterEvent()const{return std::get<AfterEventPlayer<T>>(afterEvent);}
        template<ptype T> const BeforeEventPlayer<T>& getBeforeEvent()const{return std::get<BeforeEventPlayer<T>>(beforeEvent);}
        template<ptype T> const ComponentRegisterofPlayer<T>& getComponent()const{return std::get<ComponentRegisterofPlayer<T>>(this->component);}
    };
    template<zt::event::player::Type T>
    void playerEmit(zt::event::player::params<T>&& params,const PlayerComponent& component,const zt::callback::PlayerEventListener& listener){
        bool emitdefault=true;
        listener.getBeforeEvent<T>().emit(params,emitdefault);
        if(emitdefault)listener.getComponent<T>().emit(params,component);
        listener.getAfterEvent<T>().emit(params);
    }
    template<zt::event::player::Type T>
    void playerEmit(const zt::event::player::params<T>& params,const PlayerComponent& component,const zt::callback::PlayerEventListener& listener){
        bool emitdefault=true;
        listener.getBeforeEvent<T>().emit(params,emitdefault);
        if(emitdefault)listener.getComponent<T>().emit(params,component);
        listener.getAfterEvent<T>().emit(params);
    }
    void playerConnectEmit(zt::event::player::params<zt::event::player::Type::PlayerConnect> params,
        const PlayerComponent& component,const zt::callback::PlayerEventListener& listener){
        bool emitdefault=true;
        listener.getBeforeEvent<zt::event::player::Type::PlayerConnect>().emit(params,emitdefault);
        if(emitdefault)listener.getComponent<zt::event::player::Type::PlayerConnect>().emit(params,component);
        listener.getAfterEvent<zt::event::player::Type::PlayerConnect>().emit(params);
    }
} // namespace lambda
/**
 * auto id = afterEvent.EventSubscribe(
    [](zt::event::entity::params<T>& p){
        // do something
        p.value += 1;
    }
);

 */
