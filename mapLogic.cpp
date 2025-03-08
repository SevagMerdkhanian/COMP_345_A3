#include "mapLogic.h"
#include <vector>

MapLogic::MapLogic() {}

MapLogic::MapLogic(int width, int height) : width(width), height(height) {
    map.resize(height, std::vector<Cell>(width, { SCENERY, false, false }));
}

void MapLogic::setCell(int x, int y, CellType type) {
    map[y][x].type = type;
}

Cell MapLogic::getCell(int x, int y) const {
    return map[y][x];
}

void MapLogic::setEntry(int x, int y) {
    map[y][x].type = ENTRY;
    map[y][x].isEntry = true;
    entryX = x;
    entryY = y;
}

void MapLogic::setExit(int x, int y) {
    map[y][x].type = EXIT;
    map[y][x].isExit = true;
    exitX = x;
    exitY = y;
}

int MapLogic::getWidth() const {
    return width;
}

int MapLogic::getHeight() const {
    return height;
}

int MapLogic::getEntryX() const {
    return entryX;
}
int MapLogic::getEntryY() const {
    return entryY;
}
int MapLogic::getExitX() const {
    return exitX;
}
int MapLogic::getExitY() const {
    return exitY;
}

std::vector<std::vector<Cell>> MapLogic::getMap() { return map; }
