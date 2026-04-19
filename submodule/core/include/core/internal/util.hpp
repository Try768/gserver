#pragma once
#include <tuple>
#include <utility>
template<typename Map, typename Key, typename... Args>
auto zt_emplace(Map& m, Key&& key, Args&&... args)
{
    using Value = typename Map::mapped_type;

    return m.emplace(
        std::piecewise_construct,
        std::forward_as_tuple(std::forward<Key>(key)),
        std::forward_as_tuple(std::forward<Args>(args)...)
    );
}
namespace zt::Internal::util{
        template<unsigned char len> struct getType;
        template<> struct getType<1>{using type=unsigned char;};
        template<> struct getType<2>{using type=unsigned short;};
        template<> struct getType<4>{using type=unsigned int;};
        template<> struct getType<8>{using type=unsigned long long;};
        template<class T>
        class optionalRef{
            private:
            T* data;
            public:
            const bool is_valid()const{
                if(data==nullptr)return false;
                return true;
            }
            optionalRef(T& data){
                this->data=&data;
            }
            optionalRef(T* data){
                this->data=data;
            }
            optionalRef(){
                this->data=nullptr;
            }
            template<class U=T>
            typename std::enable_if_t<!std::is_const<U>::value,T*>
             operator->(){
                return data;
            }
            const T* operator->()const{
                return data;
            }
            //may throw error or ub if isnt valid
            template<class U=T>
            typename std::enable_if_t<!std::is_const<U>::value,T&>
            get(){return *data;}
            //may throw error or ub if isnt valid
            const T& getConst()const{return *data;}
        };
    }
struct XUID{
        unsigned long long id1;
        unsigned long long id2;
        unsigned long long id3;
        bool operator==(const XUID& u)const{
            return (id1==u.id1)&&(id2==u.id2)&&(id3==u.id3);
        }
};
namespace std
{
    template<>
    struct hash<XUID>{
        size_t operator()(const XUID& u){
            size_t a=u.id1^0xab83f21678ffaa12ULL;
            size_t b=u.id2^0xaa83f2;
            size_t c=u.id3^0xab43f2;
            return (((a>>8)^(b<<8))^(c<<16));
        }
    };
} // namespace std
