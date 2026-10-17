#pragma once
// FEATURE LIBRARY: F002 Floor, F003 Wall, F004 Door
// Full behavior/dependencies: docs/FEATURE_LIBRARY.md; metadata: core/FeatureIndex.hpp

#include <string_view>
namespace doldrum {
// FLOOR: walkable ground, used as the eligible placement surface.
struct Floor { static constexpr std::string_view id="A1"; static constexpr bool solid=false; };
// WALL: impassable, blocks movement and projectiles.
struct Wall { static constexpr std::string_view id="A0"; static constexpr bool solid=true; };
// DOOR: traversable tile; room navigation logic decides destination.
struct Door { static constexpr std::string_view id="D0"; static constexpr bool solid=false; };
// New tile classes implement the same ID and collision policy, then can be
// registered in the room parser and renderer without editing player behavior.
[[nodiscard]] bool tileWalkable(std::string_view id);
} // namespace doldrum
