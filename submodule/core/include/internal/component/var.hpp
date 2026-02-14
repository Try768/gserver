#pragma once
#include "Var_Table.hpp"
class Var_component{
    protected:
    using cct=Var_component_type::Type;
    template<Var_component_type::Type T> using inval =Var_component_type::inval<T>::type;
    template<Var_component_type::Type T> using params =Var_component_type::Params<T>::type;
    cct type=cct::None;
    explicit Var_component(cct Type=cct::None):type(Type){}
    public:
    //using constrParam=Var_component_type::Params<Var_component_type::Type::None>::type;
    cct get_type()const{return type;}
    template<cct T>
    typename Var_component_type::internal<T>::type getCast(){
        if (T!=get_type())return nullptr;
        return static_cast<Var_component_type::internal<T>::type>(this);
    }
};
class Var_component_Number final:public Var_component{
    private:
    inval<cct::Number> value;
    public:
    using constrParam=params<cct::Number>;
    const params<cct::Number> get()const{return value;}
    params<cct::Number> get(){return value;}
    explicit Var_component_Number(constrParam value):value(value),Var_component(cct::Number){}
    inline const static cct type=cct::Number;
};
class Var_component_Desimal final:public Var_component{
    private:
    public:
    using constrParam=Var_component_type::Params<Var_component_type::Type::Desimal>::type;
    my_property_(constrParam,value,get)
    explicit Var_component_Desimal(constrParam value):value(value),Var_component(cct::Desimal){}
};
class Var_component_String final:public Var_component{
    private:
    public:
    using constrParam=Var_component_type::Params<Var_component_type::Type::String>::type;
    std::string value;
    const std::string& get()const{return value;}
    explicit Var_component_String(constrParam value):value(value),Var_component(cct::String){}
};
class Var_component_Bool final:public Var_component{
    private:
    my_property_(bool,value,get)
    public:

    using constrParam=Var_component_type::Params<Var_component_type::Type::Bool>::type;
    explicit Var_component_Bool(constrParam value):value(value),Var_component(cct::Bool){}
};
static void _erasingComponent(Var_component* itc);

class Var_component_Object final:public Var_component{
    private:
    std::unordered_map<std::string,Var_component*>value;
    public:
    using constrParam=Var_component_type::Params<Var_component_type::Type::Object>::type;
    using InternalType=Var_component_type::internal<Var_component_type::Type::Object>::type;
    using iterator = decltype(value)::iterator;
    using const_iterator = decltype(value)::const_iterator;
    iterator begin() noexcept { return value.begin(); }
    iterator end()   noexcept { return value.end(); }
    const_iterator begin() const noexcept { return value.begin(); }
    const_iterator end()   const noexcept { return value.end(); }
    const_iterator cbegin() const noexcept { return value.cbegin(); }
    const_iterator cend()   const noexcept { return value.cend(); }
    Var_component_type::Type getDataType(const std::string& key)const{
        auto it = value.find(key);
        if (it == value.end()) return Var_component_type::Type::None;
        return it->second->get_type();
    }
    typename constrParam get()const{
        return value;
    }
    template<Var_component_type::Type T>
    typename zt::Internal::util::optional<typename Var_component_type::inval<T>::type> getAs(const std::string& key){
        auto it = value.find(key);
        if (it == value.end()) return zt::Internal::util::optional<typename Var_component_type::Params<T>::type>();
        if (it->second->get_type() != T) throw std::runtime_error("type mismatch");
        return zt::Internal::util::optional<typename Var_component_type::Params<T>::type>
        (static_cast<typename Var_component_type::Ret<T>::type>(it->second->getCast<T>().get()));
    }
    Var_component* getPtr(const std::string& key) const {
        auto it = value.find(key);
        if (it == value.end()) return nullptr;
        return it->second;
    }
    const_iterator find(std::string key)const noexcept{return value.find(key);}
    //return false if key already exist and replaced
    template<Var_component_type::Type T>
    bool recreate(const std::string& key,Var_component_type::Params<T>::type v) {
        auto it = value.find(key);
        if (it != value.end()) {
            delete it->second;
            value.erase(it);
            value[key] = new Var_component_type::internal<T>::type(v);
            return false;
        };
        value[key] = new Var_component_type::internal<T>::type(v);
        return true;
    }
    inline static Var_component_type::Type deduce_type(const_iterator it){
        return it->second->get_type();
    } 
    private:
    static void _internalErase(const_iterator key){
        auto itc=key->second;
        if(itc==nullptr)return;
            switch(itc->get_type()){
                case cct::Number:delete static_cast<Var_component_Number*>(itc);break;
                case cct::String:delete  static_cast<Var_component_String*>(itc);break;
                case cct::Bool:delete static_cast<Var_component_Bool*>(itc);break;
                case cct::Desimal:delete  static_cast<Var_component_Desimal*>(itc);break;
                case cct::Object:delete  static_cast<Var_component_Object*>(itc);break;
                case cct::Array:delete  static_cast<Var_component_Array*>(itc);break;
                default:delete itc;break;
            }
    }
    public:
    void erase(const_iterator key){
        _internalErase(key);
        value.erase(key);
    }
    //we will take care of your pointer
    class Data{
        friend class Var_component_Object;
        std::string key;
        Var_component* value;
        
        explicit Data(const std::string& k,Var_component* v):key(k),value(v){}
        public:
        Data(Data&&other) noexcept :key(std::move(other.key)),value(other.value){
            other.value=nullptr;
        }
        ~Data(){
            _erasingComponent(value);
        }
        Var_component* release() noexcept {
            Var_component* tmp = value;
            value = nullptr;
            return tmp;
        }
        //const std::string& get_key()const{return key;}
        const std::string& get_key()const{return key;}
    };
    template<Var_component_type::Type T>
    static Data createData(typename Var_component_type::Params<T>::type v) {
        return Data(new typename Var_component_type::internal<T>::type(v));
    }
    explicit Var_component_Object(std::vector<Data> value):Var_component(cct::Object){
        for(auto it=value.begin();it!=value.end();++it){
            auto[ic,res]=this->value.try_emplace(it->get_key(),nullptr);
            if(!res){
                throw std::runtime_error("duplicate key "+it->get_key());
            }
            ic->second=it->release();
        }
    }
    //explicit Var_component_Object(std::unordered_map<std::string,Var_component*> value):value(value),Var_component(cct::Object){}
    explicit Var_component_Object():value(),Var_component(cct::Object){}
    ~Var_component_Object(){
        for(auto it=value.begin();it!=value.end();++it){
            _internalErase(it);
        }
        value.clear();
    }
    const Var_component*& operator[](const std::string key){
        return value[key];
    }
};
class Var_component_Array final:public Var_component{
    private:
    std::vector<Var_component*>value;
    public:
    inline const static cct type=cct::Object;
    using iterator = decltype(value)::iterator;
    using const_iterator = decltype(value)::const_iterator;
    iterator begin() noexcept { return value.begin(); }
    iterator end()   noexcept { return value.end(); }
    const_iterator begin() const noexcept { return value.begin(); }
    const_iterator end()   const noexcept { return value.end(); }
    const_iterator cbegin() const noexcept { return value.cbegin(); }
    const_iterator cend()   const noexcept { return value.cend(); }
    Var_component* get(const size_t key) const {
        if(key>value.size())return nullptr;
        return value[key];
    }
    using constrParam=Var_component_type::Params<Var_component_type::Type::Array>::type;
    //we will take care of your pointer
    template<Var_component_type::Type T>
    void pushBackCreate( typename Var_component_type::Params<T>::type v) {
        value.push_back(new Var_component_type::internal<T>(v));
    }
    private:
    inline void _internalErase(const_iterator key){
        auto itc=*key;
        _erasingComponent(itc);
    }
    public:
    void erase(const_iterator key){
        _internalErase(key);
        value.erase(key);
    }
    typename constrParam get()const{
        return value;
    }
    class Data{
        friend class Var_component_Array;
        Var_component* value;
        Data( Var_component* v):value(v){}
        public:
        Data(Data&&other) noexcept :value(other.value){
            other.value=nullptr;
        }
        ~Data(){
           _erasingComponent(value);
        }
        //const std::string& get_key()const{return key;}
        Var_component* release(){
            Var_component* tmp=value;
            value=nullptr;
            return tmp;
        }
    };
    template<Var_component_type::Type T>
    static Data createData(Var_component_type::Params<T> v) {
        return Data(new Var_component_type::internal<T>(v));
    }
    explicit Var_component_Array(std::vector<Data> value):Var_component(cct::Array){
        for(auto it=value.begin();it!=value.end();++it){
            this->value.push_back(it->release());
        }
    }
    explicit Var_component_Array():value(),Var_component(cct::Array){}
    const Var_component*& operator[](const size_t key){
        return value[key];
    }
    ~Var_component_Array(){
        for(auto it=value.begin();it!=value.end();++it){
            _internalErase(it);
        }
        value.clear();
    }

};
void _erasingComponent(Var_component* itc){
    if(itc==nullptr)return;
        switch(itc->get_type()){
            case Var_component_type::Type::Number:delete static_cast<Var_component_Number*>(itc);break;
            case Var_component_type::Type::String:delete  static_cast<Var_component_String*>(itc);break;
            case Var_component_type::Type::Bool:delete static_cast<Var_component_Bool*>(itc);break;
            case Var_component_type::Type::Desimal:delete  static_cast<Var_component_Desimal*>(itc);break;
            case Var_component_type::Type::Object:delete  static_cast<Var_component_Object*>(itc);break;
            case Var_component_type::Type::Array:delete  static_cast<Var_component_Array*>(itc);break;
            default:delete itc;break;
    }
}