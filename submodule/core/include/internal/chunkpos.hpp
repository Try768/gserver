#include <unordered_map>
struct ChunkPos {
    long long x;
    long long y;
};
inline bool operator==(ChunkPos const& a, ChunkPos const& b) noexcept {
    return a.x == b.x && a.y == b.y;
}

namespace std {
template<>
struct hash<ChunkPos> {
    size_t operator()(ChunkPos const& p) const noexcept {
        size_t h = 1469598103934665603ULL;
        h ^= static_cast<size_t>(p.x); h *= 1099511628211ULL;
        h ^= static_cast<size_t>(p.y); h *= 1099511628211ULL;
        return h;
    }
};
}
