#include "../../common.hpp"
#include "../../internal/datatype.hpp"
#include <string_view>
#include "core/eventListener/eventType.hpp"
#include "forward.hpp"
#include "internal/component/var.hpp"
class ComponentRegisterofPlayer;
class PlayerComponent
{
private:
    friend class Registry;
    friend class ComponentRegisterofPlayer;
    Var_component_Object const_component;
    public:
    using ID=size_t;
    using RunComponent=std::array<std::vector<ID>,(size_t)zt::event::player::Type::COUNT>;
    using optionalRC = zt::Internal::util::optional<RunComponent>;
    private:
    RunComponent runComponent;
    inline static const unsigned long long id=1;
    inline static const std::string_view idname="player";
    template<class T>
    using optional=zt::Internal::util::optional<T>;
    const optional<const Var_component> get_component(const std::string& key)const{
        return optional<const Var_component>(const_component.get(key));
    }
    optional<Var_component> get_component(const std::string& key){
        return optional<Var_component>(const_component.get(key));
    }
    const Var_component_Object& get_internal_component(){return const_component;}
    PlayerComponent(){}
};
