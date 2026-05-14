#include "core/entity/entity.hpp"
#include "core/register/register.hpp"
#include "core/map/loader.hpp"
#include "core/eventListener/eventTable.hpp"
#include "core/world/world.hpp"
EntityComponent Entity::getEntityComponent(){
        return EntityComponent(this->data.indeks.get_name(),*(this->data.indeks.entityType));
}
bool Entity::applyImpuls(Coord<double> impuls){
        this->data.velocity.addForce(impuls);
}
void Entity::clearVelocity(){
        this->data.velocity.clearForce();
}
Coord<long long> Entity::getVelocityDiskrit()const{
        auto vel=this->data.velocity.getforce();
        return Coord<long long>((long long)ceil(vel.x),(long long)ceil(vel.y));
}
void Entity::teleport(Coordinat destination){
        this->data.setchunkcoord(destination.getGlobal());
        this->data.setlocalcoord(destination.getLokal());
}
void Entity::jump(double power){
        this->data.velocity.addForce(Coord<double>(0.0,power));
}

void SimulatedEntity::damageOther(Entity& other,unsigned int damage){
        const auto& eel=dimension.getRoom().getWorld().getRegister().getEEL();
        constexpr auto eventytpe=zt::event::entity::Type::EntityHit;
        bool emitdefault=true;
        eel.getBeforeEvent<eventytpe>().emit({*this,other,damage},emitdefault);
        if(emitdefault)eel.getComponent<eventytpe>().emit({*this,other,damage},this->getEntityComponent());
        eel.getAfterEvent<eventytpe>().emit({*this,other,damage});
}
void SimulatedEntity::damageOther(Player& other,unsigned int damage){
        const auto& eel=dimension.getRoom().getWorld().getRegister().getEEL();
        constexpr auto eventytpe=zt::event::entity::Type::EntityHitPlayer;
        bool emitdefault=true;
        eel.getBeforeEvent<eventytpe>().emit({*this,other,damage},emitdefault);
        if(emitdefault)eel.getComponent<eventytpe>().emit({*this,other,damage},this->getEntityComponent());
        eel.getAfterEvent<eventytpe>().emit({*this,other,damage});
}
void SimulatedEntity::hurt(unsigned int damage,std::string&& reason){
        const auto& eel=dimension.getRoom().getWorld().getRegister().getEEL();
        constexpr auto eventytpe=zt::event::entity::Type::EntityHurt;
        bool emitdefault=true;
        eel.getBeforeEvent<eventytpe>().emit({*this,damage,reason},emitdefault);
        if(emitdefault)eel.getComponent<eventytpe>().emit({*this,damage,reason},this->getEntityComponent());
        eel.getAfterEvent<eventytpe>().emit({*this,damage,reason});
}

void Entity::walk(bool direction,double speed){
        this->data.velocity.addForce(Coord<double>(direction?speed:-speed,0));
}