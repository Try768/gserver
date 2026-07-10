#include "core/entity/player/component.hpp"
#include "core/entity/player/list.hpp"
#include "core/eventListener/playerInput.hpp"
class PlayerManager;
class Player
{
private:
    friend class PlayerManager;
    friend class Registry;
    protected:
    PlayerData& data;
    unsigned long long id;
    
    template<class T>
    using opt=zt::Internal::util::OptionalRef<T>;
    explicit inline Player(PlayerData& data,unsigned long long id):data(data),id(id){}
    public:
    inline bool is_valid(){return data.getFlag(PlayerData::status_flag::is_valid);}
    inline const std::string& getname()const{return data.name;}
    void teleport(Coordinat pos);
    void teleport(Coordinat pos,Dimension& dim);
    const velo2& getVelocity();
    void clearVelocity();
    void applyImpuls(Coord<double> impuls);
    
    inline void setDynamicProperty(const std::string& key,const MultiValue& value){
        data.dynamic_property.data[key]=value;
    }
    inline bool clearDynamicProperty(const std::string& key){
        auto it=data.dynamic_property.data.find(key);
        if(it==data.dynamic_property.data.end()){
            return false;
        }
        data.dynamic_property.data.erase(it);
        return true;
    }
    inline const opt<MultiValue> getDynamicProperty(const std::string& key)const{
        auto it=data.dynamic_property.data.find(key);
        if(it==data.dynamic_property.data.end()){
            return opt<MultiValue>();
        }
        return opt<MultiValue>(it->second);
    }
    inline const opt<const Var_component> getRuntimeProperty(const std::string& key)const{
        return data.getRuntimeproperty().get(key);
    }
    inline bool clearRuntimeProperty(const std::string& key){
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
    inline void hurt(unsigned int damage){
        hurt(damage,"");
    }
    //calling an event
    void hurt(unsigned int damage,const std::string& reason);
    //calling an event
    void PlayerInteractWithPlayer(Player& target);
    //calling an event
    inline Player& getPlayer(){
        return *this;
    }
    inline const Player& getPlayer()const{
        return *this;
    }
};