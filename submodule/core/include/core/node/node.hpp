#pragma once
#include "core/internal/datatype.hpp"
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
