#ifndef MAPLOGIC_H
#define MAPLOGIC_H

#include <vector>
#include "ObservableVec.h"

enum CellType {
    PATH,
    SCENERY,
    ENTRY,
    EXIT
};

struct Cell {
    CellType type;
    bool isEntry;
    bool isExit;
};

class MapLogic: public ObservableVec {
public:
    MapLogic();
    MapLogic(int width, int height);

    void setCell(int x, int y, CellType type);
    Cell getCell(int x, int y) const;
    std::vector<std::vector<Cell>> getMap();

    void setEntry(int x, int y);
    void setExit(int x, int y);

    int getWidth() const;
    int getHeight() const;
    int getEntryX() const;
    int getEntryY() const;
    int getExitX() const;
    int getExitY() const;
    void notifyObservers();

private:
    int width;
    int height;
    std::vector<std::vector<Cell>> map;
    int entryX, entryY;
    int exitX, exitY;
};

#endif // MAPLOGIC_H
