#include "world/Room.hpp"

#include <algorithm>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <unordered_set>

namespace doldrum {
namespace {
int dirIndex(const std::string& name) {
    if (name == "NORTH") return 0;
    if (name == "EAST") return 1;
    if (name == "SOUTH") return 2;
    if (name == "WEST") return 3;
    return -1;
}

std::string key(int x, int y) { return std::to_string(x) + ":" + std::to_string(y); }
}

bool Room::inBounds(int x, int y) const {
    return x >= 0 && x < kRoomWidth && y >= 0 && y < kRoomHeight;
}

const std::string& Room::tileAt(int x, int y) const {
    static const std::string outside = "A0";
    return inBounds(x, y) ? tiles[static_cast<size_t>(y * kRoomWidth + x)] : outside;
}

bool Room::isDoor(int x, int y) const { return tileAt(x, y) == "D0"; }

bool Room::isSolid(int x, int y) const {
    // Unrecognized codes intentionally block movement until their behavior is registered.
    const auto& tile = tileAt(x, y);
    return tile != "A1" && tile != "D0";
}

std::vector<GridPoint> Room::freeTiles(const std::vector<GridPoint>& occupied) const {
    std::unordered_set<std::string> taken;
    for (const auto& p : occupied) taken.insert(key(p.x, p.y));
    std::vector<GridPoint> result;
    for (const auto& point : walkable) {
        if (!taken.contains(key(point.x, point.y))) result.push_back(point);
    }
    return result;
}

Room loadRoom(const std::string& path) {
    std::ifstream input(path);
    if (!input) throw std::runtime_error("Cannot open room file: " + path);
    Room room;
    bool hasId = false, hasSize = false, hasSpawn = false;
    int row = 0;
    bool readingTiles = false;
    std::string line;
    int lineNumber = 0;
    while (std::getline(input, line)) {
        ++lineNumber;
        const auto comment = line.find('#');
        if (comment != std::string::npos) line.erase(comment);
        std::istringstream stream(line);
        std::string token;
        if (!(stream >> token)) continue;
        auto fail = [&](const std::string& why) {
            throw std::runtime_error(path + ":" + std::to_string(lineNumber) + ": " + why);
        };
        if (token == "ROOM") {
            if (!(stream >> room.id) || hasId) fail("expected a unique ROOM id");
            hasId = true;
        } else if (token == "SIZE") {
            int w = 0, h = 0;
            if (!(stream >> w >> h) || w != kRoomWidth || h != kRoomHeight || hasSize)
                fail("SIZE must be 20 12");
            hasSize = true;
        } else if (token == "DOORS") {
            std::string dir;
            while (stream >> dir) {
                int index = dirIndex(dir);
                if (index < 0) fail("unknown door direction: " + dir);
                room.doors[static_cast<size_t>(index)] = true;
            }
        } else if (token == "TILES") {
            if (readingTiles) fail("duplicate TILES marker");
            readingTiles = true;
        } else if (readingTiles) {
            if (row >= kRoomHeight) fail("too many tile rows");
            // First token is already the first tile on this row.
            for (int x = 0; x < kRoomWidth; ++x) {
                if (x && !(stream >> token)) fail("tile row has fewer than 20 tokens");
                if (token != "A0" && token != "A1" && token != "D0" && token != "P")
                    fail("unknown tile: " + token);
                if (token == "P") {
                    if (hasSpawn) fail("multiple player spawn markers");
                    room.spawn = {x, row};
                    hasSpawn = true;
                    token = "A1";
                }
                room.tiles[static_cast<size_t>(row * kRoomWidth + x)] = token;
                if (token == "A1") room.walkable.push_back({x, row});
            }
            if (stream >> token) fail("tile row has more than 20 tokens");
            ++row;
        } else {
            fail("unknown directive: " + token);
        }
    }
    if (!hasId || !hasSize || !readingTiles || row != kRoomHeight || !hasSpawn)
        throw std::runtime_error(path + ": missing ROOM, SIZE, TILES, 12 rows, or P spawn");
    for (int i = 0; i < 4; ++i) {
        bool found = false;
        for (int y = 0; y < kRoomHeight; ++y) for (int x = 0; x < kRoomWidth; ++x) {
            if (!room.isDoor(x, y)) continue;
            if ((i == 0 && y == 0) || (i == 1 && x == kRoomWidth - 1) ||
                (i == 2 && y == kRoomHeight - 1) || (i == 3 && x == 0)) found = true;
        }
        if (room.doors[static_cast<size_t>(i)] && !found)
            throw std::runtime_error(path + ": DOORS declares a side without D0 border tiles");
    }
    return room;
}
} // namespace doldrum
