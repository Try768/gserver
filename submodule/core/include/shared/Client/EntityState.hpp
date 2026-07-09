struct EntityStateUpdateSnapshot{
    unsigned long long entityTypeId;
    unsigned long long entityDataId;
    unsigned long long msgType;
    void* msg;
    size_t msglen;
};
namespace Client
{
    void unloadEntity(EntityStateUpdateSnapshot);
    void loadEntity(EntityStateUpdateSnapshot);
    void changeState(unsigned long long entityDataId,EntityStateUpdateSnapshot);
    EntityStateUpdateSnapshot getEntityState(unsigned long long entityDataId);
} // namespace Client
