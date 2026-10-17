#include "world/Room.hpp"
#include "world/Tiles.hpp"

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
    return x >= 0 && x < width && y >= 0 && y < height;
}

const std::string& Room::tileAt(int x, int y) const {
    static const std::string outside = "A0";
    return inBounds(x, y) ? tiles[static_cast<size_t>(y * width + x)] : outside;
}

bool Room::isDoor(int x, int y) const { return tileAt(x, y) == "D0"; }

bool Room::isSolid(int x, int y) const {
    // Unrecognized codes intentionally block movement until their behavior is registered.
    const auto& tile = tileAt(x, y);
    return !tileWalkable(tile);
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
            if (!(stream >> w >> h) || w < 3 || h < 3 || w > 512 || h > 512 || hasSize)
                fail("SIZE must be between 3x3 and 512x512");
            room.width=w; room.height=h; room.tiles.resize(static_cast<size_t>(w*h));
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
            if (row >= room.height) fail("too many tile rows");
            // First token is already the first tile on this row.
            for (int x = 0; x < room.width; ++x) {
                if (x && !(stream >> token)) fail("tile row has too few tokens");
                if (token != Wall::id && token != Floor::id && token != Door::id && token != "P")
                    fail("unknown tile: " + token);
                if (token == "P") {
                    if (hasSpawn) fail("multiple player spawn markers");
                    room.spawn = {x, row};
                    hasSpawn = true;
                    token = "A1";
                }
                room.tiles[static_cast<size_t>(row * room.width + x)] = token;
                if (token == "A1") room.walkable.push_back({x, row});
            }
            if (stream >> token) fail("tile row has too many tokens");
            ++row;
        } else {
            fail("unknown directive: " + token);
        }
    }
    if (!hasId || !hasSize || !readingTiles || row != room.height || !hasSpawn)
        throw std::runtime_error(path + ": missing ROOM, SIZE, TILES, 12 rows, or P spawn");
    for (int i = 0; i < 4; ++i) {
        bool found = false;
        for (int y = 0; y < room.height; ++y) for (int x = 0; x < room.width; ++x) {
            if (!room.isDoor(x, y)) continue;
            if ((i == 0 && y == 0) || (i == 1 && x == room.width - 1) ||
                (i == 2 && y == room.height - 1) || (i == 3 && x == 0)) found = true;
        }
        if (room.doors[static_cast<size_t>(i)] && !found)
            throw std::runtime_error(path + ": DOORS declares a side without D0 border tiles");
    }
    return room;
}
} // namespace doldrum
