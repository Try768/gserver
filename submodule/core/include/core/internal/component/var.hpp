#pragma once
#include "Var_Table.hpp"
class Var_component{
    protected:
    using cct=Var_component_type::Type;
    template<Var_component_type::Type T> using inval =Var_component_type::inval<T>::type;
    template<Var_component_type::Type T> using params =Var_component_type::Params<T>::type;
    template<Var_component_type::Type T> using cparams =Var_component_type::cParams<T>::type;
    cct type=cct::None;
    explicit Var_component(cct Type=cct::None):type(Type){}
    public:
    //using constrParam=Var_component_type::Params<Var_component_type::Type::None>::type;
    cct get_type()const{return type;}
    template<cct T>
    typename Var_component_type::internal<T>::type* getCast(){
        if (T!=get_type())return nullptr;
        return static_cast<Var_component_type::internal<T>::type*>(this);
    }
    Var_component(const Var_component&)=delete;
    Var_component& operator=(const Var_component& other)=delete;
    Var_component(Var_component&&)=default;
    Var_component& operator=(Var_component&& other)=default;
    virtual ~Var_component()=default;
};
class Var_component_Number final:public Var_component{
    private:
    typename inval<cct::Number> value;
    public:
    const inval<cct::Number>& get()const{return value;}
    inval<cct::Number>& get(){return value;}
    explicit Var_component_Number(params<cct::Number> value):value(value),Var_component(cct::Number){}
    inline const static cct type=cct::Number;
};
class Var_component_Desimal final:public Var_component{
    private:
    public:
    using constrParam=Var_component_type::Params<Var_component_type::Type::Desimal>::type;
    my_property_(inval<cct::Desimal>,value,&get)
    explicit Var_component_Desimal(constrParam value):value(value),Var_component(cct::Desimal){}
};
class Var_component_String final:public Var_component{
    private:
    public:
    using constrParam=Var_component_type::Params<Var_component_type::Type::String>::type;
    std::string value;
    const std::string& get()const{return value;}
    std::string get(){return value;}
    explicit Var_component_String(constrParam value):value(value),Var_component(cct::String){}
};
class Var_component_Bool final:public Var_component{
    private:
    my_property_(bool,value,get)
    public:

    using constrParam=Var_component_type::Params<Var_component_type::Type::Bool>::type;
    explicit Var_component_Bool(constrParam value):value(value),Var_component(cct::Bool){}
};
//static void _erasingComponent(Var_component* itc);

class Var_component_Object final:public Var_component{
    private:
    std::unordered_map<std::string,std::unique_ptr<Var_component>>value;
    public:
    using constrParam=Var_component_type::inval<Var_component_type::Type::Object>::type;
    using InternalType=Var_component_type::internal<Var_component_type::Type::Object>::type;
    template<class T>
    using opt=zt::Internal::util::optionalRef<T>;
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
    opt<Var_component> get(const std::string& key){
        auto it=value.find(key);
        if(it!=value.end())return opt<Var_component>();
        if(it->second)return opt<Var_component>();
        return opt<Var_component>(it->second.get());
    }
    const opt<const Var_component> get(const std::string& key)const{
        auto it=value.find(key);
        if(it!=value.end())return opt<const Var_component>();
        if(it->second)return opt<const Var_component>();
        return opt<const Var_component>(it->second.get());
    }
    typename const opt<const Var_component> get(std::string&& key)const{return get(key);}
    typename opt<Var_component> get(std::string&& key){return get(key);}
    const_iterator find(std::string key)const noexcept{return value.find(key);}
    iterator find(std::string key)noexcept{return value.find(key);}
    //return false if key already exist and replaced
    template<Var_component_type::Type T>
    bool recreate(const std::string& key,Var_component_type::internal<T>::type&& value) {
        auto it = value.find(key);
        if (it != value.end()) {
            value[key] =std::make_unique<Var_component_type::internal<T>::type>(std::move(v));
            return false;
        };
        value[key] = std::make_unique<Var_component_type::internal<T>::type>(std::move(v));
        return true;
    }
    inline static Var_component_type::Type deduce_type(const_iterator it){
        return it->second->get_type();
    }
    private:
    
    public:
    void erase(const_iterator key){
        value.erase(key);
    }
    class Data{
        friend class Var_component_Object;
        std::string key;
        std::unique_ptr<Var_component> value;
        
        explicit Data(const std::string& k,std::unique_ptr<Var_component>v):key(k),value(std::move(v)){}
        public:
        Data(Data&&other) noexcept :key(std::move(other.key)),value(std::move(other.value)){
        }
        ~Data() =default;
        std::unique_ptr<Var_component> release() noexcept {
            return std::move(value);
        }
        //const std::string& get_key()const{return key;}
        const std::string& get_key()const{return key;}
    };
    template<Var_component_type::Type T>
    static Data createData(typename Var_component_type::Params<T>::type v) {
        return Data(std::make_unique(typename Var_component_type::internal<T>::type(v)));
    }
    explicit Var_component_Object(std::vector<Data> value):Var_component(cct::Object){
        for(auto it=value.begin();it!=value.end();++it){
            auto[ic,res]=this->value.try_emplace(it->get_key(),nullptr);
            if(!res){
                throw std::runtime_error("duplicate key "+it->get_key());
            }
            ic->second.swap(it->release());
        }
    }
    //explicit Var_component_Object(std::unordered_map<std::string,Var_component*> value):value(value),Var_component(cct::Object){}
    explicit Var_component_Object():value(),Var_component(cct::Object){}
};


class Var_component_Array final:public Var_component{
    private:
    std::vector<std::unique_ptr<Var_component>>value;
    public:
    inline const static cct type=cct::Array;
    using iterator = decltype(value)::iterator;
    using const_iterator = decltype(value)::const_iterator;
    iterator begin() noexcept { return value.begin(); }
    iterator end()   noexcept { return value.end(); }
    const_iterator begin() const noexcept { return value.begin(); }
    const_iterator end()   const noexcept { return value.end(); }
    const_iterator cbegin() const noexcept { return value.cbegin(); }
    const_iterator cend()   const noexcept { return value.cend(); }
    using constrParam=Var_component_type::inval<Var_component_type::Type::Array>::type;
    //we will take care of your pointer
    template<Var_component_type::Type T>
    void pushBackCreate( typename Var_component_type::Params<T>::type v) {
        value.push_back(std::make_unique<Var_component_type::internal<T>::type>((v)));
    }
    public:
    inline size_t size()const noexcept{return value.size();}
    //typename const constrParam& get()const{
    //    return value;
    //}
    //typename constrParam& get(){
    //    return value;
    //}
    class Data{
        friend class Var_component_Array;
        std::unique_ptr<Var_component> value;
        Data( std::unique_ptr<Var_component>&& v):value(std::move(v)){}
        public:
        Data(Data&&other) noexcept :value(std::move(other.value)){
        }
        //const std::string& get_key()const{return key;}
        std::unique_ptr<Var_component> release(){
            return std::move(value);
        }
    };
    template<Var_component_type::Type T>
    static Data createData(Var_component_type::Params<T>::type v) {
        return Data(std::make_unique<Var_component_type::internal<T>::type>((v)));
    }
    explicit Var_component_Array(std::vector<Data> value):Var_component(cct::Array){
        for(auto it=value.begin();it!=value.end();++it){
            this->value.push_back(it->release());
        }
    }
    explicit Var_component_Array():value(),Var_component(cct::Array){}
    const Var_component& operator[](const size_t key)const{
        return *value[key];
    }
    Var_component& operator[](const size_t key){
        return *value[key];
    }

};
