#pragma once
#include "internal/internal.hpp"
#include "internal/datatype.hpp"
#include "internal/component/const.hpp"
#include <utility>
namespace InternalEntity
{
    using ctype=Const_component_type::Type;
    //type of variable component
    const std::unordered_map<std::string,unsigned int> tovc={
        {"health",t_type_id::integer},
        {"level",t_type_id::integer},
        {"variant",t_type_id::array}
    };
    using defaultpair=std::pair<ctype,std::string>;
    //variable of component
    const std::unordered_map<std::string,std::pair<Const_component_type::Type,std::vector<std::string>>> voc={
        {"maxHealth",{ctype::Number,{"health"}}},
        {"maxLevel",{ctype::Number,{"level"}}},
        {"variant",{ctype::Bool,{"variant"}}}
    };
    //const component primitif
    const std::unordered_map<std::string,ctype> ccp={
        {"gravity",ctype::Number},
        {"damage",ctype::Number},
        {"attack_player",ctype::Bool},
        {"find_player",ctype::Bool}
    };
} // namespace InternalEntity
