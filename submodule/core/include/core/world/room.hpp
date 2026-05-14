#pragma once
#include "dimension.hpp"
class Room{
    World& world;
    friend class World;
    friend class PlayerManager;
    template<class T>
    using opt=zt::Internal::util::OptionalRef<T>;
    template<class T>
    using Copt=zt::Internal::util::ConstOptionalRef<T>;
    PlayerManager playermanager;
    std::unordered_map<std::string,DimensionAccses> dimensions;
    bool is_admin;
    bool loadPlayer(XUID);
    public:
    PlayerManager& getPlayerManager(){return playermanager;}
    const PlayerManager& getPlayerManager()const{return playermanager;}
    explicit Room(std::vector<std::pair<std::string,std::unique_ptr<Dimension>>> dimensions,
        World& world,bool is_admin=false);
    const World& getWorld()const {return world;}
    opt<Dimension> getDimension(const std::string& dimensionName){
        auto it =dimensions.find(dimensionName);
        if(it!=dimensions.end())return opt<Dimension>();
        return it->second.get();
    }
    Copt<Dimension> getDimension(const std::string& dimensionName)const{
        auto itc =dimensions.find(dimensionName);
        if(itc!=dimensions.end())return Copt<Dimension>();
        return Copt<Dimension>(itc->second.getConst());
    }
    using time_point=std::chrono::steady_clock::time_point;
    //dont call it in other simulate function
    void simulate(Registry& reg,time_point time_pivot){
        playermanager.simulate(reg,time_pivot);
        for(auto& dimensi:dimensions){
            dimensi.second.getInternalAcsess().simulate(reg,time_pivot);
        }
    }
};