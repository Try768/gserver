#pragma once
#include "eventType.hpp"
#include <unordered_map>
#include <map>
#include "internal/idmaker.hpp"
#include <stdexcept>
#include "entity/entity.hpp"
#include "entity/player/player.hpp"
#include "tile/tile.hpp"
#include <tuple>
namespace zt::callback
{
    using etype=zt::event::entity::Type;
    template<zt::event::entity::Type T>
    class beforeEventEntity{
        public:
        static using ID=unsigned long long;
        static using func=void(*)(zt::event::entity::params<T>&,bool&);
        private:
        static IDMaker<ID> maker;
        static std::vector<std::pair<ID,func>> queueOfEvent;
        //std::unordered_map<ID,void(*)(zt::event::entity::params<T>&,bool&)> EventbyIterator;
        public:
        static ID EventSubscribe(void(*eventRes)(zt::event::entity::params<T>&,bool&)){
            ID id;
            if(!maker.getID(id))throw std::logic_error("what the ... how many event that you made?dude!");
            queueOfEvent.emplace_back(id,eventRes);
            return id;
        }
        static bool EventUnsubscribe(ID eventId){
            for(auto it=queueOfEvent.begin();it!=queueOfEvent.end();){
                if(it->first ==eventId){
                    queueOfEvent.erase(it);
                    maker.destroyID(eventId);
                    return true;
                }else ++it;
            }return false;
        }
        static void emit(zt::event::entity::params<T>& params,bool& emitdefault){
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
    class afterEventEntity{
        public:
        static using ID=unsigned long long;
        private:
        static IDMaker<ID> maker;
        static std::vector<std::pair<ID,void(*)(zt::event::entity::params<T>&)>> queueOfEvent;
        //std::unordered_map<ID,)> EventbyIterator;
        public:
        static ID EventSubscribe(void(*eventRes)(zt::event::entity::params<T>&)){
            ID id;
            if(!maker.getID(id))throw std::logic_error("what the ... how many event that you made?dude!");
            queueOfEvent.emplace_back(id,eventRes);
            return id;
        }
        static bool EventUnsubscribe(ID eventId){
            for(auto it=queueOfEvent.begin();it!=queueOfEvent.end();){
                if(it->first ==eventId){
                    queueOfEvent.erase(it);//intented
                    maker.destroyID(eventId);
                    return true;
                }else ++it;
            }
            return false;
        }
        static void emit(zt::event::entity::params<T>& params){
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
        using Index=size_t;
        static Index index;
        static std::unordered_map<std::string,Index> ComponentID;
        static std::vector<void(*)(zt::event::entity::params<T>&)> ComponentByIndeks;
        public:
        static bool registerComponent(std::string ComponentName,void(*eventRes)(zt::event::entity::params<T>&)){
            if(ComponentID.count(ComponentName))return false;
            if(ComponentByIndeks.size()==std::numeric_limits<size_t>::max)throw std::exception("dude how many Component that you wanted to add?!");
            auto [itc,stat]=ComponentID.try_emplace(ComponentName,ComponentByIndeks.size());
            if(stat){ComponentByIndeks.push_back(eventRes);}else return false;
            return true;
        }
        static void connect(EntityComponent& component){
            component.runComponent[T].clear();
            for(const auto& itc :ComponentID){
                auto iter =component.getInternal().const_component.find(itc.first);
                if(iter!=component.getInternal().const_component.end()){
                    component.runComponent[T].push_back(itc.second);
                }
            }
        }
        static void emit(zt::event::entity::params<T>& params,const EntityComponent& component){
            const auto& itc =component.runComponent[T];
            for(auto ind:itc){
                assert(ind<ComponentByIndeks.size());
                //if(ind>=ComponentByIndeks.size())throw std::logic_error("hmm i wonder where it come from");
                ComponentByIndeks[ind](params);
            }
        }
    };
    template<zt::event::entity::Type T>
    void entityEmit(zt::event::entity::params<T>&& params,EntityComponent component){
        bool emitdefault=true;
        beforeEventEntity<T>::emit(params,emitdefault);
        if(emitdefault)
        ComponentRegisterofEntity<T>::emit(params,component);
        afterEventEntity<T>::emit(params);
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
namespace zt::EventBus{
    template<zt::event::entity::Type T>
    class Bus{
        using IDparam=zt::event::entity::queueID<T>;
        static std::vector<IDparam> queueOfEvent;
        public:
        static size_t size(){
            return queueOfEvent.size();
        }
        static const IDparam& operator[](size_t it) const{
            return queueOfEvent[it];
        }
        static void clear(){
            queueOfEvent.clear();
        }
        static void eventPush(const IDparam& idparam){
            queueOfEvent.push_back(idparam);
        }
        static void eventPush(IDparam&& idparam){
            queueOfEvent.push_back(std::move(idparam));
        }
    };
    //NO tick
    template<>
    class Bus<zt::event::entity::Type::Tick>{
    };
    //no COUNT
    template<>
    class Bus<zt::event::entity::Type::COUNT>{
    };
    //not adding event tick
}