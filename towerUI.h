#ifndef TOWER_UI_H
#define TOWER_UI_H

#include "towerLogic.h"
#include "raylib.h"

// UI class for drawing a single tower and its bullets.
class TowerUI {
public:
    // Draw a tower using raylib drawing functions.
    // The parameter cellSize is used to scale the tower drawing.
    static void drawTower(const Tower* tower, int cellSize);
};

// UI Manager class that draws all towers from a TowerManager.
class TowerUIManager {
public:
    // Draw all towers stored in the TowerManager.
    // cellSize is passed along for scaling.
    static void drawTowers(const TowerManager& towerManager, int cellSize);
};

#endif // TOWER_UI_H
