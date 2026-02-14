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
            optional(const T& data){
                this->data=&data;
            }
            optional(const T* data){
                this->data=data;
            }
            optional(){
                this->data=nullptr;
            }
            //may throw error or ub if isnt valid
            T& get(){return *data;}
            //may throw error or ub if isnt valid
            const T& getConst()const{return *data;}
        };
    }