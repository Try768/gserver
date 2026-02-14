#pragma once
#include "forward.hpp"
#include <unordered_set>
template<class IDT>
    class IDMaker{
        private:
        friend class chunkmap;
        friend class EntityManager;
        friend class PlayerManager;
        std::unordered_set<IDT> free_ids;
        IDT next=0;
        IDT back=0;
        IDT count=0;
        public:
        bool getID(IDT& id){
            if(!free_ids.empty()){
                auto itc=free_ids.begin();
                id=*itc;
                free_ids.erase(itc);
                next++;count++;
                return true;
            }
            if(count!=ULLONG_MAX){
                id=next;next++;count++;
                return true;
            }return false;
        }
        void trimed(){
            while (true)
            {
                auto itc =free_ids.find(back);
                if(itc==free_ids.end()){
                    free_ids.erase(itc);
                    back++;
                }else{
                    break;
                }
            }
            
        }
        void destroyID(const IDT id){
            if(count==0)return;
            if(id==back){
                back++;
                count--;
            }else if(back>next){
                if(id>back||id<next){
                    free_ids.insert(id);
                    count--;
                }
            }else{
                if(id>back&&id<next){
                    free_ids.insert(id);
                    count--;
                }
            }
        }
    };