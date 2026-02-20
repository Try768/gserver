#include "entity/player/component.hpp"
#include "entity/player/list.hpp"
#include "eventListener/playerInput.hpp"
class PlayerManager;
class Player
{
private:
    friend class PlayerManager;
    friend class Registry;
    PlayerData& data;
    std::string name;
    template<class T>
    using opt=zt::Internal::util::optional<T>;
    explicit Player(PlayerData& data):data(data){}
    public:
    bool is_valid(){return data.getFlag(PlayerData::status_flag::is_valid);}
    inline const std::string& getname()const{return name;}
    void teleport(Coordinat pos);
    bool tryTeleport(Coordinat pos);
    const velo2& getVelocity();
    void clearVelocity();
    void applyImpuls(Coord<double> impuls);
    
    void setDynamicProperty(const std::string& key,const MultiValue& value){
        data.dynamic_property.data[key]=value;
    }
    bool clearDynamicProperty(const std::string& key){
        auto it=data.dynamic_property.data.find(key);
        if(it==data.dynamic_property.data.end()){
            return false;
        }
        data.dynamic_property.data.erase(it);
        return true;
    }
    const opt<MultiValue> getDynamicProperty(const std::string& key)const{
        auto it=data.dynamic_property.data.find(key);
        if(it==data.dynamic_property.data.end()){
            return opt<MultiValue>();
        }
        return opt<MultiValue>(it->second);
    }
    const opt<const Var_component> getRuntimeProperty(const std::string& key)const{
        return data.getRuntimeproperty().get(key);
    }
    bool clearRuntimeProperty(const std::string& key){
        auto it=data.runtime_property.find(key);
        if(it==data.runtime_property.end()){
            return false;
        }
        data.runtime_property.erase(it);
        return true;
    }
    template<Var_component_type::Type T>
    bool setRuntimeProperty(const std::string& key,Var_component_type::internal<T>::type&& v){
        data.runtime_property.recreate<T>(key,v)
    }
    
};
/* Simulated Player is a special kind of player that is controlled by the server
   It can be used to simulate player actions for testing or automation purposes
*/
class SimulatedPlayer:public Player{
    using Player::Player;
    public:
    //calling an event
    void damageOther(Entity& other,unsigned int damage);
    //calling an event
    void damageOther(Player& other,unsigned int damage);
    //calling an event
    void breakTile(Tile& tile);
    //calling an event
    void hurt(unsigned int damage);
    //calling an event
    void PlayerInteractWithPlayer(Player& target);
    //calling an event
    void PlayerInteractWithEntity(Entity& target);
    //calling an event
    void PlayerInteractWithTile(Tile& target);
    Player& getPlayer(){
        return *this;
    }
    const Player& getPlayer()const{
        return *this;
    }
};