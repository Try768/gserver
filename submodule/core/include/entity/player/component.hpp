#include "../../common.hpp"
#include "../../internal/datatype.hpp"
class playercomponent
{
private:
    public:
    inline static const std::string idname="player";
    inline static const std::vector<unsigned char> componentType={t_type_id::string};
    inline static const std::vector<std::string> componentName={"nama"};
    inline static const unsigned long long id=1;
    playercomponent(){}
};
