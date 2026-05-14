#include "core/register/register.hpp"
#include "core/tile/list.hpp"
 #include "core/entity/list.hpp"
 
 #include "core/chunk/entitymanager.hpp"
 #include "core/chunk/chunk.hpp"
 #include "world/world.hpp"
 #include "core/eventListener/eventTable.hpp"
//EntityManager::ID EntityManager::duplicateEntity(EntityData&& data){
//    ID tempid;
//    if(!entityid.getID(tempid))throw std::exception("alright alright thats enough entities");
//    auto [it,inserted]=entitybyID.try_emplace(tempid,(nullptr));
//    if(inserted){it->second=new EntityData(data);}
//    return tempid;
//}

EntityManager::ID EntityManager::createEntity(const EntityData& data){
    ID tempid;
    if(!entityid.getID(tempid))throw std::exception("alright alright thats enough entities");
    auto [it,inserted]=entitybyID.try_emplace(tempid,(nullptr));
    if(inserted){it->second=new EntityData(data);}
    auto itch=chunks.find(data.getchunkcoord());
    if(itch!=chunks.end()){
        itch->second.entityInChunk.emplace(tempid);
    }else{
        chunks.try_emplace(data.getchunkcoord(),tempid);
    }
    return tempid;
}
bool EntityManager::delEntity(ID id){
    auto itc=entitybyID.find(id);
    if(itc==entitybyID.end())return false;
    auto itch=chunks.find(itc->second->getchunkcoord());
    if(itch!=chunks.end()){
        itch->second.entityInChunk.erase(id);
    }
    entitybyID.erase(id);
    return true;
}
double calclinear(Coord<double> vec,double x,double maksVal=2){
    if(vec.x==0&&vec.y==0)return 0;
    if(vec.x==0&&vec.y>maksVal)return maksVal;
    if(vec.x==0&&vec.y<-maksVal)return -maksVal;
    if(vec.x==0)return vec.y;
    double gradien=vec.y/vec.x;
    return std::clamp(std::clamp(x*gradien,-vec.y,vec.y), -maksVal, maksVal);
}
std::optional<Coord<double>> getmaksCanGo(Dimension& dim,Coordinat potition,
    int dirX,int dirY,Coord<long long> next,velo2 vel,zt::Collision colision){
    
    auto getTileSolid=[&](Coordinat coord)->bool{
        return dim.getTile(coord).getCollision();
    };
    auto checkCollision=[&](Coord<long long> coord)->int{
        if(colision.width==0||colision.height==0)return 0;
        if(dirX!=0){
             for(int y=-colision.height/2;y<colision.height/2;y++){
                if(getTileSolid(potition+Coord<long long>{((colision.width/2)*dirX),y}))return 1*dirX;
            }
        }
        if(dirY!=0){
             for(int x=-colision.width/2;x<colision.width/2;x++){
                if(getTileSolid(potition+Coord<long long>{x,((colision.height/2)*dirY)}))return 2*dirY;
            }
        }
        return 0;
    };
    int drc=checkCollision(next);
    if(!drc)return std::nullopt;
    double correctionX=(colision.width%100)/100.0*vel.getforce().x*dirX;
    double correctionY=(colision.height%100)/100.0*vel.getforce().y*dirY;
    if(drc==1||drc==-1){
        if(correctionX==0)return std::nullopt;
        double kalkulasiX=calclinear(vel.getforce(),correctionX);
        if(kalkulasiX>1||kalkulasiX<-1)return std::nullopt;
        return Coord<double>{((colision.width/2)*dirX)+correctionX,kalkulasiX};
    }else if(drc==2||drc==-2){
        if(correctionY==0)return std::nullopt;
        double kalkulasiY=calclinear(Coord<double>(vel.getforce().y,vel.getforce().x),correctionY);
        if(kalkulasiY>1||kalkulasiY<-1)return std::nullopt;
        return Coord<double>{kalkulasiY,((colision.height/2)*dirY)+correctionY};
    }
    return std::nullopt;
}
/*
pseduocode for collision checking and correction
Main
moveEntity(entity, velX, velY)
{
    moveX(entity, velX);
    moveY(entity, velY);
}
horizontal
moveX(entity, velX)
{
    step = sign(velX);

    repeat abs(velX)
    {
        nextX = entity.x + step;

        if(collideAt(nextX, entity.y))
        {
            entity.velX = 0;
            break;
        }

        entity.x = nextX;
    }
}
    vertikal
moveX(entity, velX)
{
    step = sign(velX);

    repeat abs(velX)
    {
        nextX = entity.x + step;

        if(collideAt(nextX, entity.y))
        {
            entity.velX = 0;
            break;
        }

        entity.x = nextX;
    }
}
cek sisi
bool collideRight(entity, nextX)
{
    edgeX = nextX + entity.width;

    top = entity.y;
    bottom = entity.y + entity.height;

    for(y = top; y <= bottom; y++)
    {
        tileX = floordiv(edgeX, TILE_SIZE);
        tileY = floordiv(y, TILE_SIZE);

        if(tileSolid(tileX, tileY))
            return true;
    }

    return false;
}
*/
static void impuls(){}
void EntityManager::cleanUpEvent(Entity& data,
    const Coord<long long> &chunk){
    //applying impuls
    auto perpindahan=data.getVelocityDiskrit();
    Coord<long long> last_chunk;
    zt::Collision colision=data.getCollision();
    bool first=true,loaded=false;
    long long bx=0,by=0;
    velo2 vel=data.data.velocity;
    Coord<double> maksgoto{0,0};
    zt::util::workline(0,0,perpindahan.x,perpindahan.y,[&](long long x,long long y){
        Coord<long long> chunkPosi{
            zt::util::floordiv(x,16)+chunk.x,
            zt::util::floordiv(y,16)+chunk.y
        };

        if((chunkPosi != last_chunk)&&(!first)){
            loaded = this->dimension.isChunkLoaded(chunkPosi);
            last_chunk = chunkPosi;
        }
        if(!(loaded)){maksgoto=Coord<double>{x,y};return true;}
        int dirX=(x>bx)-(x<bx);
        int dirY=(y>by)-(y<by);
        //getMaksCanGo();
        if(auto maksCanGo=getmaksCanGo(dimension,data.getCoordinat()+Coord<long long>{x,y},dirX,dirY,Coord<long long>{x,y},vel,colision)){
            maksgoto=*maksCanGo;
            return true;
        }
        vel.addForce(Coord<double>{-dirX,-dirY});
        return false;
    });
    //lets gooooo todo:add remain time and loop to apply velocity until it can be applied no more
    data.data.applyVelocity(0,TileSide::None,0,0);
        // chunk correction
    if((data.getCoordinat().getGlobal().x!=chunk.x)||(data.getCoordinat().getGlobal().y!=chunk.y)){
        EntityManager::chunks[data.getCoordinat().getGlobal()].entityInChunk.emplace(data.ID);
        auto& tc= EntityManager::chunks[chunk];
        tc.entityInChunk.erase(data.ID);
        if(tc.entityInChunk.empty())EntityManager::chunks.erase(chunk);
    }
}
void EntityManager::simulate(time_point time_pivot){
    constexpr auto eventtick=zt::event::entity::Type::Tick;
    const auto& emiter=reg.getEEL();
    for(auto& entitiesinchunk:chunks){
        if(!entitiesinchunk.second.loaded)continue;
        for(auto entityid=entitiesinchunk.second.entityInChunk.begin();
        entityid!=entitiesinchunk.second.entityInChunk.end();){
            auto [entity,status]=EntityManager::getEntity(*entityid);
            if(!status){
                entityid=entitiesinchunk.second.entityInChunk.erase(entityid);continue;
            }
            SimulatedEntity simulatedEntity(entity);
            bool emitMainEvent=true;
            auto now=std::chrono::steady_clock::now();
            double deltatime=std::chrono::duration<double,std::milli>(now-time_pivot).count();
            zt::event::entity::params<eventtick> param{simulatedEntity,deltatime};
            //before event
            emiter.getBeforeEvent<eventtick>().emit(param,emitMainEvent);
            //componnent run
            if(emitMainEvent){
                emiter.getComponent<eventtick>().emit(
                    param,entity.getEntityComponent()
                );
            }
            //after event
            emiter.getAfterEvent<eventtick>().emit(
                param
            );
            cleanUpEvent(simulatedEntity.getEntity(),entitiesinchunk.first);//wip
            ++entityid;
        }
    }
}