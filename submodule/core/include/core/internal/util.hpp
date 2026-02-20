#pragma once
namespace zt::Internal::util{
        template<class T>
        class optional{
            private:
            T* data;
            public:
            const bool is_valid()const{
                if(data==nullptr)return false;
                return true;
            }
            optional(T& data){
                this->data=&data;
            }
            optional(T* data){
                this->data=data;
            }
            optional(){
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