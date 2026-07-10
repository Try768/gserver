
#include "core/entity/player/player.hpp"
#include "core/entity/entity.hpp"
#include "core/world/world.hpp"
void Player::teleport(Coordinat pos)
{
    this->data.setchunkcoord(pos.getGlobal());
    this->data.setlocalcoord(pos.getLokal());
}
void Player::clearVelocity(){
    this->data.velocity.clearForce();
}
void Player::teleport(Coordinat coor,Dimension& dim){
    this->data.origin.setDimension(dim);
    this->data.setchunkcoord(coor.getGlobal());
    this->data.setlocalcoord(coor.getLokal());
}
void Player::teleport(Coordinat coor){
    this->teleport(coor,this->data.origin.getDimensionRef());
}   
void SimulatedPlayer::damageOther(Entity &other, unsigned int damage)
{
    const auto &reg = this->data.origin.getWorld().getRegister();
    const auto &pel = reg.getPEL();
    constexpr auto eventytpe = zt::event::player::Type::PlayerHitEntity;
    bool emitdefault = true;
    pel.getBeforeEvent<eventytpe>().emit({*this, other,damage}, emitdefault);
    if (emitdefault)
    {
        pel.getComponent<eventytpe>().emit({*this, other,damage}, reg.getPlayerComponent());
    }
    pel.getAfterEvent<eventytpe>().emit({*this, other,damage});
}
void SimulatedPlayer::damageOther(Player &other, unsigned int damage)
{
    const auto &reg = this->data.origin.getWorld().getRegister();
    const auto &pel = reg.getPEL();
    constexpr auto eventytpe = zt::event::player::Type::PlayerHitPlayer;
    bool emitdefault = true;
    pel.getBeforeEvent<eventytpe>().emit({*this, other,damage}, emitdefault);
    if (emitdefault)
    {
        pel.getComponent<eventytpe>().emit({*this, other,damage}, reg.getPlayerComponent());
    }
    pel.getAfterEvent<eventytpe>().emit({*this, other,damage});
}
void SimulatedPlayer::PlayerInteractWithPlayer(Player& target){
    const auto &reg = this->data.origin.getWorld().getRegister();
    const auto &pel = reg.getPEL();
    constexpr auto eventytpe = zt::event::player::Type::PlayerInteractWithPlayer;
    bool emitdefault = true;
    pel.getBeforeEvent<eventytpe>().emit({*this, target}, emitdefault);
    if (emitdefault)
    {
        pel.getComponent<eventytpe>().emit({*this, target}, reg.getPlayerComponent());
    }
    pel.getAfterEvent<eventytpe>().emit({*this, target});
}
void SimulatedPlayer::hurt(unsigned int damage,const std::string& reason){
    const auto &reg = this->data.origin.getWorld().getRegister();
    const auto &pel = reg.getPEL();
    constexpr auto eventytpe = zt::event::player::Type::PlayerHurt;
    bool emitdefault = true;
    pel.getBeforeEvent<eventytpe>().emit({*this,damage,reason}, emitdefault);
    if (emitdefault)
    {
        pel.getComponent<eventytpe>().emit({*this,damage,reason}, reg.getPlayerComponent());
    }
    pel.getAfterEvent<eventytpe>().emit({*this,damage,reason});
}
