#pragma once
namespace zt::input
{
    enum PlayerInput:int{
        invalid=-1,zero,one,two,three,four,five,six,seven,eight,nine,
        A,B,C,D,E,F,G,H,I,J,K,L,M,N,O,P,Q,R,S,T,U,V,W,X,Y,Z,
        ctrl,spacebar,rightclick,shift,leftclick,tab,koma,point,slash,reverse_slash,enter
    };
    class CursorPos{
        Coord<float> pos;
        public:
        float getX()const noexcept{return pos.x;}
        float getY()const noexcept{return pos.y;}
        private:
        void setX(float x){
            if(x<0)x=0;
            else if(x>1)x=1;
            this->pos.x=x;
        }
        void setY(float y){
            if(y<0)y=0;
            else if(y>1)y=1;
            this->pos.y=y;
        }
    };
    CursorPos* getCursor(void* param,PlayerInput input){
        if(input==PlayerInput::rightclick||input==PlayerInput::leftclick){
            return (CursorPos*)param;
        }return nullptr;
    }
} // namespace zt::input