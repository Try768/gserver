#include "core/world/dimension.hpp"
#include "core/world/world.hpp"
explicit Dimension::Dimension(const std::string& dimensionName,
        Room& room,World& world,const std::string& dimensionDir):room(room),
    entityManager(*this,world.getRegister(),std::string(dimensionDir+"entityData/")),name(dimensionName){
    }