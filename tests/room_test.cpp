#include "world/Room.hpp"
#include <cassert>
#include <iostream>
int main(int argc, char** argv) {
    assert(argc == 3);
    auto first = doldrum::loadRoom(argv[1]);
    auto second = doldrum::loadRoom(argv[2]);
    assert(first.id == "room_start" && second.id == "room_east");
    assert(first.isSolid(0,0));
    assert(!first.isSolid(10,6));
    assert(first.isDoor(19,5));
    assert(second.isDoor(0,5));
    assert(first.doors[1] && second.doors[3]);
    auto available = first.freeTiles({{10,6}});
    for(auto p: available) assert(p.x != 10 || p.y != 6);
    std::cout << "Room parsing checks passed\n";
}
