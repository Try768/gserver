#pragma once
#include "core/internal/datatype.hpp"
#include "core/internal/component/var.hpp"
#include <queue>
class ActionNode{
    public:
    enum Action:unsigned char{
        walk,
        run,
        jump,
    };
    Action aksi;
    bool bringMomentum;
    unsigned short atenttion_span;
    Coord<int> value;
    ActionNode* next;
};
void deleteActionNode(ActionNode* node){
    auto* it=node;
    ActionNode* tmp;
    while(it!=nullptr){
        tmp=it;
        it=it->next;
        delete tmp;
    }
}
struct InternalEntityMoveNode{
    double time;
    Coordinat _akhir;
    Coordinat _awal;
    unsigned int anim_id;
    unsigned int permutation;
};
class EntityMoveNode{
    private:
    std::queue<InternalEntityMoveNode> internal;
    public:
    void addMove(double time,Coordinat _akhir,
    Coordinat _awal,unsigned int anim_id,unsigned int permutation){
        internal.emplace(InternalEntityMoveNode{time,_akhir,_awal,anim_id,permutation});
    }
    const std::queue<InternalEntityMoveNode>& getQueue()const{return internal;}
};
