#include "core/register/register.hpp"

Registry::InEntityComponent Registry::getentitybyname(std::string name){
    namespace utility=zt::Internal::util;
    auto itc = entities.find(name);
    if(itc==entities.end())return InEntityComponent();
    return InEntityComponent(EntityComponent(name,itc->second));
}
Registry::InTileComponent Registry::getTilebyname(std::string name)
{
    namespace utility=zt::Internal::util;
    auto itc = tiles.find(name);
    if(itc==tiles.end())return utility::OptionalRef<TileComponent>();
    return utility::OptionalRef<TileComponent>(TileComponent(itc->second));
}
