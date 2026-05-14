#pragma once
#include <tuple>
#include <algorithm>
#include <utility>
#include <cmath>
#include <vector>

#define __zt__addComparator__inclass(type,equal,lessequal,moreequal,lessthan,morethan,notequal) \
bool operator==(const type& other){return equal;}\
bool operator<=(const type& other){return lessequal;}\
bool operator>=(const type& other){return moreequal;}\
bool operator<(const type& other){return lessthan;}\
bool operator>(const type& other){return morethan;}\
bool operator!=(const type& other){return notequal;}
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
namespace zt{
    constexpr short lenOfDomain=16;
    namespace util{
        inline long long floordiv(long long a, long long b){
            return (a >= 0) ? (a / b) : -((-a - (b - 1)) / b);
        }
        inline long long minOfDomain(long long domain){
            return domain -(lenOfDomain/2);
        }
        inline long long maxOfDomain(long long domain){
            return domain +(lenOfDomain/2)-1;
        }
        inline long long chunkToDomain(long long chunkcoord){
            return floordiv(chunkcoord, lenOfDomain) * lenOfDomain;
        }
    }
}
namespace zt::util {

    std::vector<std::pair<long long, long long>> getLine(long long x0, long long y0, long long x1, long long y1) {
        std::vector<std::pair<long long, long long>> points;
        long long dx = std::abs(x1 - x0);
        long long dy = std::abs(y1 - y0);
        long long sx = (x0 < x1) ? 1 : -1;
        long long sy = (y0 < y1) ? 1 : -1;
        long long err = dx - dy;
        while (true) {
            points.emplace_back(x0, y0);
            if (x0 == x1 && y0 == y1)
                break;

            int e2 = 2 * err;
            if (e2 > -dy) {
                err -= dy;
                x0 += sx;
            }
            if (e2 < dx) {
                err += dx;
                y0 += sy;
            }
        }
        return points;
    }
    template<class F>
    void workline(long long x0, long long y0, long long x1, long long y1,F func){
        long long dx = std::abs(x1 - x0);
        long long dy = std::abs(y1 - y0);
        long long sx = (x0 < x1) ? 1 : -1;
        long long sy = (y0 < y1) ? 1 : -1;
        long long err = dx - dy;
        while (true) {
            if(func(x0,y0))return;
            if (x0 == x1 && y0 == y1)
                break;

            int e2 = 2 * err;
            if (e2 > -dy) {
                err -= dy;
                x0 += sx;
            }
            if (e2 < dx) {
                err += dx;
                y0 += sy;
            }
        }
    }
    namespace convertion{
        long long toFixed(double v) {
        return (long long)std::floor(v * 100.0);
    }
    double toDouble(long long v) {
        return v / 100.0;
    }
    static constexpr int makslokal =((16/2)*100)-1;
    static constexpr int minlokal =-(16/2)*100;
    static constexpr int lenlokal =1600;
    void toCoordinat(
        long long abs,
        long long& global,
        int& lokal
    ){
        global = zt::util::floordiv(abs, lenlokal);
        long long lx = abs - global * lenlokal;
        lokal = (int)(lx - makslokal);
    }
    long long toFixed(long long global, short lokal) {
        return global * lenlokal + (lokal + makslokal);
    }

    }

std::string binaryToDecimalBig(const std::vector<int>& bits) {
    std::string result = "0";

    for (int b : bits) {
        // result *= 2
        int carry = 0;
        for (int i = result.size() - 1; i >= 0; --i) {
            int num = (result[i] - '0') * 2 + carry;
            result[i] = (num % 10) + '0';
            carry = num / 10;
        }
        if (carry) result.insert(result.begin(), carry + '0');

        // result += b
        int i = result.size() - 1;
        int add = b;
        while (i >= 0 && add) {
            int num = (result[i] - '0') + add;
            result[i] = (num % 10) + '0';
            add = num / 10;
            --i;
        }
        if (add) result.insert(result.begin(), add + '0');
    }

    return result;
    }
    //string hanya boleh memiliki angka/bilangan saja
    std::string multiplyStr(const std::string& num, int mul)
    {
        int carry = 0;
        std::string result;
        for (int i = num.size() - 1; i >= 0; --i){
            int digit = num[i] - '0';
            int temp = digit * mul + carry;
            result.push_back((temp % 10) + '0');
            carry = temp / 10;
        }
        while (carry > 0){
            result.push_back((carry % 10) + '0');
            carry /= 10;
        }
        std::reverse(result.begin(), result.end());
        return result;
    }
    //hanya boleh memiliki angka atau bilangan 
    std::string addStr(std::string a, std::string b)
    {
        std::string result;
        int carry = 0;
        int i = a.size() - 1;
        int j = b.size() - 1;
        while (i >= 0 || j >= 0 || carry)
        {
            int da = 0;
            int db = 0;
            if (i >= 0)
                da = a[i--] - '0';
            if (j >= 0)
                db = b[j--] - '0';
            int sum = da + db + carry;
            result.push_back((sum % 10) + '0');
            carry = sum / 10;
        }
        std::reverse(result.begin(), result.end());
        return result;
    }
}
namespace zt::Internal::util{
        template<unsigned char len> struct getType;
        template<> struct getType<1>{using type=unsigned char;};
        template<> struct getType<2>{using type=unsigned short;};
        template<> struct getType<4>{using type=unsigned int;};
        template<> struct getType<8>{using type=unsigned long long;};
        template<class T>
        class ConstOptionalRef{
            private:
            const T* data;
            public:
            const bool is_valid()const{
                if(data==nullptr)return false;
                return true;
            }
            ConstOptionalRef(T& data){
                this->data=&data;
            }
            ConstOptionalRef(T* data){
                this->data=data;
            }
            ConstOptionalRef(const T& data){
                this->data=&data;
            }
            ConstOptionalRef(const T* data){
                this->data=data;
            }
            ConstOptionalRef(){
                this->data=nullptr;
            }
            //may throw error or ub if isnt valid
            const T* operator->()const{
                return data;
            }
            //may throw error or ub if isnt valid
            const T& getConst()const{return *data;}
        };
        template<class T>
        class OptionalRef{
            private:
            T* data;
            public:
            const bool is_valid()const{
                if(data==nullptr)return false;
                return true;
            }
            OptionalRef(T& data){
                this->data=&data;
            }
            OptionalRef(T* data){
                this->data=data;
            }
            OptionalRef(){
                this->data=nullptr;
            }
            operator->(){
                return data;
            }
            const T* operator->()const{
                return data;
            }
            //may throw error or ub if isnt valid
            get(){return *data;}
            //may throw error or ub if isnt valid
            const T& getConst()const{return *data;}
        };
        //template<class U=T>
        //typename std::enable_if_t<!std::is_const<U>::value,T&>
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
