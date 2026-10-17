#include "world/Room.hpp"
#include <cassert>
#include <iostream>
int main(int argc,char** argv){
    assert(argc == 2);
    const std::string root=argv[1];
    auto large=doldrum::loadRoom(root+"/room_large.txt");
    auto boss=doldrum::loadRoom(root+"/room_boss.txt");
    assert(large.width==44 && large.height==28 && boss.width==44 && boss.height==28);
    assert(large.isDoor(43,13) && large.isDoor(43,14));
    assert(boss.isDoor(0,13) && boss.isDoor(0,14));
    assert(!boss.isSolid(1,13) && !large.isSolid(42,13));
    std::cout<<"rooms and doors valid\n";
}
