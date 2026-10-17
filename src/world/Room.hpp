#pragma once
// FEATURE LIBRARY: F005 Room
// Full behavior/dependencies: docs/FEATURE_LIBRARY.md; metadata: core/FeatureIndex.hpp


#include <array>
#include <string>
#include <vector>

namespace doldrum {
constexpr int kRoomWidth = 20; // legacy default for new room templates
constexpr int kRoomHeight = 12; // legacy default for new room templates
constexpr int kTileSize = 16;

enum class Direction { North = 0, East = 1, South = 2, West = 3 };

struct GridPoint { int x = 0; int y = 0; };

struct Room {
    std::string id;
    int width = kRoomWidth;
    int height = kRoomHeight;
    std::vector<std::string> tiles;
    std::array<bool, 4> doors{};
    GridPoint spawn{10, 6};
    std::vector<GridPoint> walkable;

    [[nodiscard]] bool inBounds(int x, int y) const;
    [[nodiscard]] const std::string& tileAt(int x, int y) const;
    [[nodiscard]] bool isSolid(int x, int y) const;
    [[nodiscard]] bool isDoor(int x, int y) const;
    [[nodiscard]] std::vector<GridPoint> freeTiles(const std::vector<GridPoint>& occupied) const;
};

// Throws std::runtime_error with line/context if the TXT room is invalid.
[[nodiscard]] Room loadRoom(const std::string& path);
} // namespace doldrum
