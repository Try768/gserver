#include "../../common.hpp"
#include "../../internal/datatype.hpp"
#include <string_view>
#include "forward.hpp"
#include "internal/component/var.hpp"
class PlayerComponent
{
private:
    friend class registry;
    static Var_component_Object const_component;
    public:
    inline static const unsigned long long id=1;
    inline static const std::string_view idname="player";
    template<class T>
    using optional=zt::Internal::util::optional<T>;
    
    static optional<Var_component> get_component(const std::string& key){
        return optional<Var_component>(const_component.get(key));
    }
    PlayerComponent(){}
};
