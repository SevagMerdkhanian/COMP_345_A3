#ifndef MAPUI_H
#define MAPUI_H

#include "critterLogic.h"
#include "towerUI.h"
#include "mapLogic.h"
#include "raylib.h"
#include <string>

class MapUI {
public:
    MapUI(MapLogic& mapLogic);
    void initUI();
    void updateUI();
    void drawUI();
    void drawUIWithTowersCustom(TowerManager& towerManager,TowerUIManager& towerUIManager, CritterManager& critterManager);
    void drawCritters(CritterManager& manager);

private:
    MapLogic& mapLogic;
    int cellSize;
    CellType selectedTile;
    std::string validationMessage; // To display validation result

    bool validateMap(); // Method to validate the map
    void dfs(int x, int y, std::vector<std::vector<bool>>& visited); // DFS for path connectivity
    const char* tileTypeToString(CellType type); // Convert CellType to string
    void wrapText(const std::string& text, int x, int y, int maxWidth, int fontSize); // Wrap text function
};

#endif
