#pragma once
#include "forward.hpp"
#include "core/internal/util.hpp"
#include <string>
#include "playerInput.hpp"
class EntityComponent;
class TileComponent;
class PlayerComponent;
class EntityData;
class TileData;
class PlayerData;
class SimulatedEntity;
namespace zt
{
    namespace event
    {
        namespace entity
        {
            enum Type:unsigned int{
                //register
                
                //load data
                InvalidData,
                //simulatedEntity
                EntityWalk,
                EntityJump,
                EntityHit,
                EntityHitPlayer,
                EntityHurt,
                //simulatedPlayer
                PlayerInteractWithEntity,
                //EntityManager
                EntityDied,
                EntitySpawn,
                Tick,
                COUNT
            };
            template<Type eventType> struct params;
            template<> struct params<Type::PlayerInteractWithEntity>{Player& source;Entity& target;};
            template<> struct params<Type::EntityHit>{Entity& source;Entity& target;unsigned int damage;};
            template<> struct params<Type::EntityHitPlayer>{Entity& source;Player& target;unsigned int damage;};
            template<> struct params<Type::EntityHurt>{Entity& target;unsigned int damage;const std::string& reason;};
            template<> struct params<Type::Tick>{SimulatedEntity& entity;};
            template<> struct params<Type::PlayerInteractWithEntity>{Player& source;Entity& entity;};
            template<> struct params<Type::EntityWalk>{Entity& entity;};
            template<> struct params<Type::EntityJump>{Entity& entity;};
            //template<> struct params<Type::Register>{EntityComponent& entityComponent;};
            template<> struct params<Type::InvalidData>{EntityData& entity;};
            template<> struct params<Type::COUNT>{};
            using playerID=unsigned long long;
            using entityID=unsigned long long;
            template<Type eventType> struct queueID;
            template<> struct queueID<Type::PlayerInteractWithEntity>{playerID source;entityID target;};
            template<> struct queueID<Type::EntityHit>{entityID source;entityID target;unsigned int damage;};
            template<> struct queueID<Type::EntityHitPlayer>{entityID source;playerID target;unsigned int damage;};
            template<> struct queueID<Type::EntityHurt>{entityID target;std::string reason;};
            template<> struct queueID<Type::Tick>{};
            template<> struct queueID<Type::InvalidData>{};
            //template<> struct queueID<Type::Register>{};
            template<> struct queueID<Type::COUNT>{};
        } // namespace Entity
        namespace tile
        {
            enum Type:unsigned int{
                //register,
                //Register,
                //load data,
                InvalidData,
                //simulatedPlayer,
                PlayerInteractWithTile,
                PlayerStepOn,
                //simulatedEntity,
                EntityStepOn,
                //TileManager,
                TileSwap,
                TileLoaded,
                Tick,
                COUNT
            };
            template<Type eventType> struct params;
            template<> struct params<Type::TileSwap>{Tile& prev;Tile& next;};
            template<> struct params<Type::PlayerInteractWithTile>{Player& source;Tile& target;};
            template<> struct params<Type::EntityStepOn>{Entity& source;Tile& target;};
            template<> struct params<Type::PlayerStepOn>{Player& source;Tile& target;};
            template<> struct params<Type::Tick>{Tile& target;};
            template<> struct params<Type::InvalidData>{TileData& data;const Registry& reg;};
            template<> struct params<Type::COUNT>{};
        } // namespace Tile
        namespace player{
            enum Type:unsigned int{
                //register
                //Register,
                //load data
                invalidData,
                //simulatedPlayer
                PlayerHitEntity,
                PlayerHurt,
                PlayerInteractWithPlayer,
                //PlayerManager
                PlayerInput,
                PlayerDisconnect,
                PlayerSpawn,
                PlayerDespawn,
                PlayerDied,
                PlayerConnect,
                Tick,
                COUNT
            };
            template<Type eventType> struct params;
            template<> struct params<Type::PlayerHitEntity>{Player& source;Entity& target;};
            template<> struct params<Type::PlayerHurt>{Player& target;};
            template<> struct params<Type::PlayerInteractWithPlayer>{Player& source;Player& target;};
            template<> struct params<Type::PlayerInput>{SimulatedPlayer& source;zt::input::PlayerInput& input;};
            template<> struct params<Type::PlayerDisconnect>{const XUID& xuid;};
            template<> struct params<Type::PlayerSpawn>{Player& source;};
            template<> struct params<Type::PlayerDied>{Player& source;};
            template<> struct params<Type::PlayerConnect>{bool& aprove;const XUID& xuid;};
            template<> struct params<Type::COUNT>{};
        } // namespace player
        
        
        
    } // namespace event
    
} // namespace zt
