#pragma once
#include "etheader.hpp"
//Todo : theres an error down there and dont forget about tile component
namespace zt::callback
{
    using ttype=zt::event::tile::Type;
    template<ttype T>
    class BeforeEventTile{
        friend class Registry;
        public:
        using ID=unsigned long long;
        using func=void(*)(zt::event::tile::params<T>&,bool&);
        private:
        IDMaker<ID> maker;
        std::vector<std::pair<ID,func>> queueOfEvent;
        BeforeEventTile(){}
        //std::unordered_map<ID,void(*)(zt::event::entity::params<T>&,bool&)> EventbyIterator;
        public:
        ID EventSubscribe(void(*eventRes)(zt::event::tile::params<T>&,bool&)){
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
        void emit(zt::event::tile::params<T>& params,bool& emitdefault){
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
    template<ttype T>
    class AfterEventTile{
        friend class Registry;
        public:
        using ID=unsigned long long;
        private:
        IDMaker<ID> maker;
        std::vector<std::pair<ID,void(*)(zt::event::tile::params<T>&)>> queueOfEvent;
        AfterEventTile();
        //std::unordered_map<ID,)> EventbyIterator;
        public:
        ID EventSubscribe(void(*eventRes)(zt::event::tile::params<T>&)){
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
        void emit(zt::event::tile::params<T>& params){
            for (size_t i = 0; i < queueOfEvent.size(); ){
                auto id = queueOfEvent[i].first;
                auto handler = queueOfEvent[i].second;
                handler(params);
                if (i < queueOfEvent.size() && queueOfEvent[i].first == id)
                    ++i;
            }
        }
    };
    template<ttype T>
    class ComponentRegisterofTile{
        using Index=size_t;
        Index index;
        std::unordered_map<std::string,Index> ComponentID;
        std::vector<void(*)(zt::event::tile::params<T>&)> ComponentByIndeks;
        public:
        bool registerComponent(std::string ComponentName,void(*eventRes)(zt::event::tile::params<T>&)){
            if(ComponentID.count(ComponentName))return false;
            if(ComponentByIndeks.size()==std::numeric_limits<size_t>::max)throw std::exception("dude how many Component that you wanted to add?!");
            auto [itc,stat]=ComponentID.try_emplace(ComponentName,ComponentByIndeks.size());
            if(stat){ComponentByIndeks.push_back(eventRes);}else return false;
            return true;
        }
        void connect(TileComponent& component){
            component.runComponent[T].clear();
            for(const auto& itc :ComponentID){
                auto iter =component.getInternal().const_component.find(itc.first);
                if(iter!=component.getInternal().const_component.end()){
                    component.runComponent[T].push_back(itc.second);
                }
            }
        }
        void emit(zt::event::Tile::params<T>& params,const TileComponent& component){
            const auto& itc =component.runComponent[T];
            for(auto ind:itc){
                assert(ind<ComponentByIndeks.size());
                //if(ind>=ComponentByIndeks.size())throw std::logic_error("hmm i wonder where it come from");
                ComponentByIndeks[ind](params);
            }
        }
    };
    
    class TileEventListener{
        private:
        std::tuple<
            AfterEventTile<ttype::EntityStepOn>,
            AfterEventTile<ttype::invalidData>,AfterEventTile<ttype::PlayerInteractWithTile>,
            AfterEventTile<ttype::PlayerStepOn>,AfterEventTile<ttype::Tick>,
            AfterEventTile<ttype::TileLoaded>,AfterEventTile<ttype::TileSwap>
        > afterEvent;
        std::tuple<
            BeforeEventTile<ttype::EntityStepOn>,
            BeforeEventTile<ttype::invalidData>,BeforeEventTile<ttype::PlayerInteractWithTile>,
            BeforeEventTile<ttype::PlayerStepOn>,BeforeEventTile<ttype::Tick>,
            BeforeEventTile<ttype::TileLoaded>,BeforeEventTile<ttype::TileSwap>
        > beforeEvent;
        std::tuple<
            ComponentRegisterofTile<ttype::EntityStepOn>,
            ComponentRegisterofTile<ttype::invalidData>,ComponentRegisterofTile<ttype::PlayerInteractWithTile>,
            ComponentRegisterofTile<ttype::PlayerStepOn>,ComponentRegisterofTile<ttype::Tick>,
            ComponentRegisterofTile<ttype::TileLoaded>,ComponentRegisterofTile<ttype::TileSwap>
        > component;
        public:
        template<etype T> AfterEventEntity<T> getAfterEvent(){std::get<AfterEventEntity<T>>(afterEvent);}
        template<etype T> BeforeEventEntity<T> getBeforeEvent(){std::get<AfterEventEntity<T>>(beforEvent);}
        template<etype T> ComponentRegisterofEntity<T> getComponent(){std::get<ComponentRegisterofEntity<T>>(this->component);}
    };
    template<zt::event::tile::Type T>
    void tileEmit(zt::event::tile::params<T>&& params,const TileComponent& component,TileEventListener& listener){
        bool emitdefault=true;
        listener.getBeforeEvent<T>().emit(params,emitdefault);
        if(emitdefault)listener.getComponent<T>().emit(params,component);
        listener.getComponent<T>().emit(params);
    }
    template<zt::event::tile::Type T>
    void tileEmit(zt::event::tile::params<T>&& params,TileComponent&& component,TileEventListener& listener){
        bool emitdefault=true;
        listener.getBeforeEvent<T>().emit(params,emitdefault);
        if(emitdefault)listener.getComponent<T>().emit(params,component);
        listener.getComponent<T>().emit(params);
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
