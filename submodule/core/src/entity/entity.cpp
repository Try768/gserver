#include "core/entity/entity.hpp"
#include "core/register/register.hpp"
#include "core/map/loader.hpp"
#include "core/eventListener/eventTable.hpp"
EntityComponent Entity::getEntityComponent(){
        return EntityComponent(this->data.indeks.get_name(),*(this->data.indeks.entityType));
}
bool Entity::applyImpuls(Coord<double> impuls){
        this->data.velocity.addForce(impuls);
}
void Entity::clearVelocity(){
        this->data.velocity.clearForce();
}
Coord<int> Entity::getVelocity(){
        auto vel=this->data.velocity.getforce();
        return Coord<int>((int)vel.x,(int)vel.y);
}
void Entity::teleport(Coordinat destination){
        this->data.setchunkcoord(destination.getGlobal());
        this->data.setlocalcoord(destination.getLokal());
}
void Entity::jump(double power){
        this->data.velocity.addForce(Coord<double>(0.0,power));
}
void SimulatedEntity::damageOther(Entity& other,unsigned int damage){
        zt::callback::entityEmit<zt::event::entity::Type::EntityHit>(
                {*this,other,damage},this->data.indeks.getComponent()
        );
}
void SimulatedEntity::damageOther(Player& other,unsigned int damage){
        zt::callback::entityEmit<zt::event::entity::Type::EntityHitPlayer>(
                {*this,other,damage},this->data.indeks.getComponent()
        );
}
void SimulatedEntity::hurt(unsigned int damage,std::string&& reason){
        zt::callback::entityEmit<zt::event::entity::Type::EntityHurt>(
                {*this,damage,reason},this->data.indeks.getComponent()
        );
}

void Entity::walk(bool direction,double speed){
        this->data.velocity.addForce(Coord<double>(direction?speed:-speed,0));
}