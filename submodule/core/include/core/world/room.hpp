#pragma once
#include "dimension.hpp"
class Room{
    World& world;
    template<class T>
    using opt=zt::Internal::util::optional<T>;
    PlayerManager playermanager;
    std::unordered_map<std::string,std::unique_ptr<Dimension>> dimensions;
    bool is_admin;
    public:
    Player getPlayer(unsigned long long);
    explicit Room(std::vector<std::pair<std::string,std::unique_ptr<Dimension>>> dimensions,
        World& world,bool is_admin=false);

    opt<Dimension> getDimension(const std::string& dimensionName){
        auto it =dimensions.find(dimensionName);
        if(it!=dimensions.end())return opt<Dimension>();
        return opt<Dimension>(it->second.get());
    }
    const opt<Dimension> getDimension(const std::string& dimensionName)const{
        auto it =dimensions.find(dimensionName);
        if(it!=dimensions.end())return opt<Dimension>();
        return opt<Dimension>(it->second.get());
    }
};