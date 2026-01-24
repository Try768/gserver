#pragma once
#include "forward.hpp"
namespace zt
{
    namespace event
    {
        enum class Type:int{
            EntityHit,
            PlayerHitEntity,
            TileBreak,
            EntityHurt,
            PlayerHurt,
            TilePlaced,
            PlayerInteractWithTile,
            PlayerInteractWithEntity,
            PlayerInteractWithPlayer,
            PlayerInput,
            PlayerDisconnect,
            PlayerSpawn,
            PlayerDied,
            PlayerConnect,
            PlayerMove
        };
        template<Type eventType> struct params;
        template<> struct params<Type::EntityHit>{Entity& source;Entity& target;};
        template<> struct params<Type::PlayerHitEntity>{Player& source;Entity& target;};
        template<> struct params<Type::TileBreak>{Entity& source;Entity& target;};
        template<> struct params<Type::EntityHurt>{Entity& source;Entity& target;};
        template<> struct params<Type::PlayerHurt>{Entity& source;Entity& target;};
        
    } // namespace event
    
} // namespace zt
