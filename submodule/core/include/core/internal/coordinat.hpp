#pragma once
#include <vector>
#include <unordered_map>
#include <string>
#include "util.hpp"
#include "shared/Client/EntityMove.hpp"
template<typename T>
struct Coord
{
    static_assert(std::is_arithmetic_v<T>,
              "Coord<T> only supports primitive arithmetic types");

    T x;
    T y;
    Coord(T x=0,T y=0):x(x),y(y){}
    Coord operator+(const Coord& other)const{
        return {x+other.x,y+other.y};
    }
    Coord& operator+=(const Coord& other)const{
        this->x+other.x;
        this->y+other.y;
        return *this;
    }
    bool operator==(const Coord<T>& other)const{return (other.x==this->x) &&(this->y==this->y);}
    bool operator!=(const Coord<T>& other)const{return !(*this == other);}
};
template<typename T>
inline bool operator==(Coord<T> const& a, Coord<T> const& b) noexcept {
    return a.x == b.x && a.y == b.y;
}

namespace std {
template<class T>
struct hash<Coord<T>> {
    size_t operator()(Coord<T> const& p) const noexcept {
        size_t h = 1469598103934665603ULL;
        h ^= std::hash<T>{}(p.x); h *= 1099511628211ULL;
        h ^= std::hash<T>{}(p.y); h *= 1099511628211ULL;
        return h;
    }
};
}
//using Relative =Coord<long long>;
//using ChunkGlobal =Coord<long long>;
//prefix 2 desimal
class InChunkCoord{
    private:
    static constexpr int makslokal =((16/2)*100)-1;
    static constexpr int minlokal =-(16/2)*100;
    static constexpr int lenlokal =1600;
    Coord<short>  _data;
    public:
    InChunkCoord(short x,short y):_data(x,y){}
    InChunkCoord(Coord<short> data):_data(data){}
    Coord<int> add(const Coord<long long>& coord){
        int ox=zt::util::floordiv(coord.x,lenlokal);
        int oy=zt::util::floordiv(coord.y,lenlokal);
        int tempx=((int)(coord.x-lenlokal*ox))+_data.x;
        int tempy=((int)(coord.y-lenlokal*oy))+_data.y;
        short gsx =zt::util::floordiv(tempx,lenlokal);
        short gsy=zt::util::floordiv(tempy,lenlokal);
        _data.x=tempx-gsx*lenlokal;
        _data.y=tempy-gsy*lenlokal;
        return{ox+gsx,oy+gsy};
    }
    Coord<int> add(const Coord<short>& coord){
        int tempx=(int)coord.x+_data.x;
        int tempy=(int)coord.y+_data.y;
        short gsx =zt::util::floordiv(tempx,lenlokal);
        short gsy=zt::util::floordiv(tempy,lenlokal);
        _data.x=tempx-gsx*lenlokal;
        _data.y=tempy-gsy*lenlokal;
        return{gsx,gsy};
    }
    Coord<int> add(const Coord<double>& coord){
        long long dx=(long long)floor(coord.x*100);
        long long dy=(long long)floor(coord.y*100);
        int ox=zt::util::floordiv(dx,lenlokal);
        int oy=zt::util::floordiv(dy,lenlokal);
        int tempx=((int)(dx-lenlokal*ox))+_data.x;
        int tempy=((int)(dy-lenlokal*oy))+_data.y;
        short gsx =zt::util::floordiv(tempx,lenlokal);
        short gsy=zt::util::floordiv(tempy,lenlokal);
        _data.x=tempx-gsx*lenlokal;
        _data.y=tempy-gsy*lenlokal;
        return{ox+gsy,oy+gsx};
    }
    inline const Coord<short> get()const{
        return _data;
    }
    Coord<double> getDouble()const{
        double x=_data.x,y=_data.y;
        return Coord{x/100,y/100};
    }
};
class Coordinat{
    private:
    InChunkCoord lokal;
    Coord<long long> global;
    public:
    const Coord<int16_t> getLokal()const noexcept{return lokal.get();}
    const Coord<long long> getGlobal()const noexcept{return global;}
    Coordinat(Coord<int16_t> lokal,Coord<long long> global):lokal(lokal),global(global){}
    Coordinat operator+(const Coordinat& other){
        InChunkCoord tmp(lokal);
        auto gs=tmp.add(other.lokal.get());
        auto gl=global+other.global;
        gl.x+=gs.x;
        gl.y+=gs.y;
        return Coordinat(tmp.get(),gl);
    }
    Coordinat operator+(Coordinat&& other){
        InChunkCoord tmp(lokal);
        auto gs=tmp.add(other.lokal.get());
        auto gl=global+other.global;
        gl.x+=gs.x;
        gl.y+=gs.y;
        return Coordinat(tmp.get(),gl);
    }
    unsigned char getTileX()const{
        return zt::util::floordiv(lokal.get().x,100)+(16/2);
    }
    unsigned char getTileY()const{
        return zt::util::floordiv(lokal.get().y,100)+(16/2);
    }
    bool isXgreaterthan(const Coordinat& other){
        if(this->global.x>other.global.x)return true;
        if(this->global.x<other.global.x)return true;
        if(this->lokal.get().x>other.lokal.get().x)true;
        return false;
    }
    bool isXequal(const Coordinat& other){
        if(this->global.x==other.global.x)return true;
        if(this->lokal.get().x==other.lokal.get().x)true;
        return false;
    }
    bool isYequal(const Coordinat& other){
        if(this->global.y==other.global.y)return true;
        if(this->lokal.get().y==other.lokal.get().y)true;
        return false;
    }
    bool isYgreaterthan(const Coordinat& other){
        if(this->global.y>other.global.y)return true;
        if(this->global.y<other.global.y)return true;
        if(this->lokal.get().y>other.lokal.get().y)true;
        return false;
    }
    inline std::string toStringGL(
        const Coord<long long>& global,
        const Coord<int16_t>& lokal
    ) {
        std::ostringstream oss;
        oss << "G(" << global.x << ", " << global.y << ") "
            << "| L(" << lokal.x << ", " << lokal.y << ")";
        return oss.str();
    }
    inline std::string toStringWorldPretty(
        const Coord<long long>& g,
        const Coord<int16_t>& l
    ) {
        std::ostringstream oss;
        oss << "W("
            << g.x << std::setw(4) << std::setfill('0') << l.x
            << ", "
            << g.y << std::setw(4) << std::setfill('0') << l.y
            << ")";
        return oss.str();
    }
    Coordinat& operator+=(const Coord<double>& other) {
        Coordinat& out = *this;
        Coord<int> gs= out.lokal.add(other);
        out.global.x+=gs.x;
        out.global.y+=gs.y;
        return out;
    }
    Coordinat& operator+=(const Coord<long long>& other) {
        Coordinat& out = *this;
        Coord<int> gs= out.lokal.add(other);
        out.global.x+=gs.x;
        out.global.y+=gs.y;
        return out;
    }
    Coordinat operator+(const Coord<long long>& other) const {
        Coordinat out = *this;
        Coord<int> gs=out.lokal.add(other);
        out.global.x+=gs.x;
        out.global.y+=gs.y;
        return out;
    }
    inline std::string toStringWorldLazy(
        const Coord<long long>& g,
        const Coord<int16_t>& l
    ) {
        return "W(" +
            std::to_string(g.x) + ":" +
            std::to_string(l.x) + ", " +
            std::to_string(g.y) + ":" +
            std::to_string(l.y) +
        ")";
    }
    inline std::string toStringLong(){
        std::string x=zt::util::addStr(std::to_string(this->global.x),
        zt::util::multiplyStr(std::to_string(this->lokal.get().x),1600));
        std::string y=zt::util::addStr(std::to_string(this->global.y),
        zt::util::multiplyStr(std::to_string(this->lokal.get().y),1600));
        return x+y;
    }
    Coordinat operator+(const Coord<double>& other) const {
        Coordinat out = *this;
        Coord<int> gs=out.lokal.add(other);
        out.global.x+=gs.x;
        out.global.y+=gs.y;
        return out;
    }

};
enum TileSide:unsigned char{
    None,Vertical,Horizontal
};
void reflectVelocity(double& vx, double& vy,
                            TileSide side,
                            double restitution = 1.0)
{
    switch (side) {
    
    case TileSide::Vertical:
        vx = -vx * restitution;
        break;
    case TileSide::Horizontal:
        vy = -vy * restitution;
        break;

    default:
        break;
    }
}
bool clampDisplacementWithRemainder(
    double velocity,
    double maxDisp,
    double& outRemainingTime
) {
    double disp = velocity * outRemainingTime;

    if (std::abs(disp) < maxDisp) {
        outRemainingTime = 0.0;
        return false;
    }

    // waktu sampai mencapai batas
    double t_hit = maxDisp / std::abs(velocity);

    // sisa waktu
    outRemainingTime -= t_hit;
    return true;
}
double normVec2(const Coord<double>& vec) {
    return std::sqrt(vec.x * vec.x + vec.y * vec.y);
}
struct velo2{
    private:
    double x, y;
    double friction;
    
    public:
    void addForce(Coord<double>&& force){
        x+=force.x;
        y+=force.y;
    }
    void addForce(const Coord<double>& force){
        x+=force.x;
        y+=force.y;
    }
    const Coord<double> getforce()const{
        return Coord<double>(x,y);
    }
    void clearForce(){
        x=0;y=0;
    }
    void setFriction(double friction){
        if(friction<0)friction=0;
        this->friction=friction;
    }
    inline void apply(Coordinat& pos,double deltatime){
        pos+=Coord(x*deltatime,y*deltatime);
        x-=std::clamp(x,-(friction*deltatime),friction*deltatime);
        y-=std::clamp(y,-(friction*deltatime),friction*deltatime);
    }
    //status and remaining dt
    inline void apply(Coordinat& pos,
               double& remaintime,
               TileSide reflect_side,
               double xMaks,
               double yMaks,float restitution=1.0,EntityMoveSnapshot& snapshot)
    {
        double crt=remaintime;
        // integrate position
        bool habis=clampDisplacementWithRemainder(normVec2(Coord<double>(x, y)),normVec2(Coord<double>(xMaks, yMaks)),remaintime);
        auto deltapos= Coord(
            std::clamp(x * (crt-remaintime), -xMaks, xMaks),
            std::clamp(y * (crt-remaintime), -yMaks, yMaks)
        );
        snapshot.x=deltapos.x;
        snapshot.y=deltapos.y;
        snapshot.time=(crt-remaintime);
        pos+=deltapos;
        const double f = friction * (crt-remaintime);
        x -= std::clamp(x, -f, f);
        y -= std::clamp(y, -f, f);
        if(remaintime<=0)return;
        // reflect velocity if collision
        if (reflect_side != TileSide::None && !habis) {
            reflectVelocity(x, y, reflect_side,restitution); // 1.0 = elastic
        }
        
        // friction (velocity space)
    }
    void apply(Coordinat& pos,
        double& remaintime,
        double xMaks,
        double yMaks,float restitution=1.0)
        {
            TileSide reflect_side=TileSide::None;
            // integrate position
            auto crt=remaintime;
            bool end=clampDisplacementWithRemainder(normVec2(Coord<double>(x, y)),normVec2(Coord<double>(xMaks, yMaks)),remaintime);
            
            pos += Coord(
                std::clamp(x * crt-remaintime, -xMaks, xMaks),
                std::clamp(y * crt-remaintime, -yMaks, yMaks)
            );
            // friction (velocity space)
            const double f = friction * (crt-remaintime);
            x -= std::clamp(x, -f, f);
            y -= std::clamp(y, -f, f);
            // reflect velocity if collision
            const long long err=round(100*(abs(xMaks)-abs(yMaks)))/100;
            if(err>0)reflect_side=TileSide::Horizontal;
            if(err<0)reflect_side=TileSide::Vertical;
            if (reflect_side != TileSide::None && !end) {
                reflectVelocity(x, y, reflect_side,restitution); // 1.0 = elastic
            }
        }
        
    void apply(Coordinat& pos,double& remaintime,double xMaks,double yMaks){
            double crt=remaintime;
        clampDisplacementWithRemainder(normVec2(Coord<double>(x, y)),normVec2(Coord<double>(xMaks, yMaks)),remaintime);
        pos+=Coord(std::clamp(x*(crt-remaintime),-xMaks,xMaks),std::clamp(y*(crt-remaintime),-yMaks,yMaks));
        x-=std::clamp(x,-(friction*(crt-remaintime)),friction*(crt-remaintime));
        y-=std::clamp(y,-(friction*(crt-remaintime)),friction*(crt-remaintime));
    }
    
};

class Coord_manager_local{
    protected:
    Coord<int16_t> lokal;
    Coord_manager_local()=default;
    void localdump(std::vector<unsigned char>& keluaran)const{
        to_buffer_bigendian<int16_t>(lokal.x,keluaran);
        to_buffer_bigendian<int16_t>(lokal.y,keluaran);
    }
    static bool is_local_coor_buffer_valid(const std::vector<unsigned char>& buffer,size_t& offset){
        using namespace zt::Internal;
        bool a=zt::Internal::parse::checkPrimitiveBigendian<int16_t>(buffer,offset);
        return parse::checkPrimitiveBigendian<int16_t>(buffer,offset)&&a;
    }
    void localCoorParse(const std::vector<unsigned char>& buffer,size_t& offset){
        buffer_bigendian_to<int16_t>(buffer,offset,lokal.x);
        buffer_bigendian_to<int16_t>(buffer,offset,lokal.y);
    }
    Coord_manager_local(const Coord<int16_t>& lokal):lokal(lokal){}
    public:
    inline void setlocalcoord(const Coord<int16_t>& lokal){
            this->lokal=lokal;
    }
    inline Coord<int16_t> getlocalcoord()const{
            return lokal;
    }
    inline const Coord<int16_t>& getreflocal() const{
         return lokal;
    }
};

class Coord_manager:public Coord_manager_local{
    protected:
        Coord<long long> global;
        void co_dump(std::vector<unsigned char>& keluaran)const{
            this->localdump(keluaran);
            to_buffer_bigendian<long long>(global.x,keluaran);
            to_buffer_bigendian<long long>(global.y,keluaran);
            //to_buffer_bigendian<unsigned char>((((unsigned char)global.signx)<<1)|((unsigned char)global.signy),keluaran);
        }
        Coord_manager(){
            global={0,0};
        };
        static bool is_co_valid(const std::vector<unsigned char>& buffer,size_t& offset){
            bool a=Coord_manager_local::is_local_coor_buffer_valid(buffer,offset);
            using namespace zt::Internal;
            a=parse::checkPrimitiveBigendian<long long>(buffer,offset)&&a;
            return parse::checkPrimitiveBigendian<long long>(buffer,offset)&&a;
        }
        void co_parse(const std::vector<unsigned char>& buffer,size_t& offset){
            this->localCoorParse(buffer,offset);
            buffer_bigendian_to<long long>(buffer,offset,global.x);
            buffer_bigendian_to<long long>(buffer,offset,global.y);
        }
        Coord_manager& set_Coordinat(const Coordinat& other){
            this->global.x=other.getGlobal().x;
            this->global.y=other.getGlobal().y;
            this->lokal.x=other.getLokal().x;
            this->lokal.y=other.getLokal().y;
        }
        Coord_manager(const Coord<long long>& pos,
            const Coord<int16_t>& lokal):global(pos),Coord_manager_local(lokal)
        {}
    public:
        
        inline const Coord<long long>& getchunkcoord()const{
            return global;
        }
        inline void setchunkcoord(Coord<long long> chunk){
            this->global=chunk;
        }
};
