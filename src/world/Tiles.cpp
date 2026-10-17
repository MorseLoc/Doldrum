#include "world/Tiles.hpp"
namespace doldrum {
bool tileWalkable(std::string_view id){return id==Floor::id || id==Door::id;}
}
