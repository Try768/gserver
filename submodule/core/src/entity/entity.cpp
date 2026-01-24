#include <entity/entity.hpp>
#include <register/register.hpp>
const EntityComponent& Entity::getEntityComponent(){
        return registry::getEntities().at(indeks.get_id());
}