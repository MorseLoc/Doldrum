// FEATURE LIBRARY: F002 Floor, F003 Wall, F004 Door
// Full behavior/dependencies: docs/FEATURE_LIBRARY.md; metadata: core/FeatureIndex.hpp
#include "world/Tiles.hpp"
namespace doldrum {
bool tileWalkable(std::string_view id){return id==Floor::id || id==Door::id;}
}
