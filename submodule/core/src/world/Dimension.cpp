#include "core/world/dimension.hpp"
#include "core/world/world.hpp"
explicit Dimension::Dimension(const std::string& dimensionName,DimensionPermision permit,
        Room& room,World& world,const std::string& dimensionDir):room(room),
    entityManager(OriginWorld{world,*this,room},permit
    ,world.getRegister(),std::string(dimensionDir+"entityData/")),name(dimensionName){}