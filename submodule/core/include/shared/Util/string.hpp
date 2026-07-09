#pragma once
namespace zt
{
    size_t strlen(const char* str){
        size_t len=0;
        while(str[len]!='\0') len++;
        return len;
    }
    void memcpy(char* dest,const char* src,size_t n){
        for(size_t i=0;i<n;i++){
            dest[i]=src[i];
        }
    }
    class string{
        char* data;
        size_t length;
        public:
        string(const char* str){
            length=strlen(str);
            data=new char[length+1];
            memcpy(data,str,length+1);
        }
        string(const string& other){
            length=other.length;
            data=new char[length+1];
            memcpy(data,other.data,length+1);
        }
        string(string&& other) noexcept : data(other.data), length(other.length) {
            other.data = nullptr;
            other.length = 0;
        }
        string& operator=(const string& other){
            if(this==&other)return *this;
            delete[] data;
            length=other.length;
            data=new char[length+1];
            memcpy(data,other.data,length+1);
            return *this;
        }
        string& operator+=(const string& other){
            char* newData=new char[length+other.length+1];
            memcpy(newData,data,length);
            memcpy(newData+length,other.data,other.length+1);
            delete[] data;
            data=newData;
            length+=other.length;
            return *this;
        }
        string operator+(const string& other) const {
            string result(*this);
            result += other;
            return result;
        }
        ~string(){
            delete[] data;
        }
        const char* c_str()const{
            return data;
        }
        size_t size()const{
            return length;
        }
    };
    class string_view{
        const char* data;
        size_t length;
        public:
        string_view(const char* str):data(str),length(strlen(str)){}
        string_view(const string& str):data(str.c_str()),length(str.size()){}
        const char* c_str()const{
            return data;
        }
        size_t size()const{
            return length;
        }
    };
} // namespace zt
