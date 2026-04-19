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
    struct Internal{
        friend class Registry;
        friend class PlayerComponent;
        friend class ComponentRegisterofPlayer;
        private:
        using ID=size_t;
        using RunComponent=std::array<std::vector<ID>,(size_t)zt::event::player::Type::COUNT>;
        using optionalRC = zt::Internal::util::optionalRef<RunComponent>;
        Internal(Var_component_Object&& cco):const_component(std::move(cco)){}
        Var_component_Object const_component;
        RunComponent runComponent;
        public:
        const auto& getConstComponent()const {return const_component;}
    };
    private:
    Internal* internal;
    explicit PlayerComponent(const std::string&,Internal& internal):internal(&internal){}
    
    public:
    inline static const unsigned long long id=1;
    inline static const std::string_view idname="player";
    template<class T>
    using optional=zt::Internal::util::optionalRef<T>;
    const optional<const Var_component> get_component(const std::string& key)const{
        return optional<const Var_component>(internal->getConstComponent().get(key));
    }
    const Var_component_Object& get_internal_component(){return internal->getConstComponent();}
};
