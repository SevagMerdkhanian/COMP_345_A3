#include "towerUI.h"
#include <string>

// Draw a single tower and its bullets.
void TowerUI::drawTower(const Tower* tower, int cellSize) {
    Vector2 pos = tower->getPosition();
    Color color;
    // Choose color based on tower type.
    switch (tower->getTowerType()) {
    case TowerType::BASIC:   color = PURPLE; break;
    case TowerType::SPLASH:  color = ORANGE; break;
    case TowerType::SLOW:    color = SKYBLUE; break;
    case TowerType::SNIPER:    color = DARKGREEN; break;
    default:                 color = GRAY; break;
    }

    // Calculate tower radius: diameter is cellSize/1.5, so radius = cellSize/3.
    float towerRadius = static_cast<float>(cellSize) / 3.0f;
    DrawCircleV(pos, towerRadius, color);

    // Draw the tower's level in the center.
    std::string levelStr = std::to_string(tower->getLevel());
    int levelTextWidth = MeasureText(levelStr.c_str(), 16);
    DrawText(levelStr.c_str(), pos.x - levelTextWidth / 2, pos.y - 8, 16, WHITE);

    // Draw the tower's range circle.
    float rangeDisplayRadius = tower->getRange() * cellSize; // Range in pixels.
    DrawCircleLines(pos.x, pos.y, rangeDisplayRadius, LIGHTGRAY);

    // Draw the numeric range above the tower.
    std::string rangeStr = std::to_string(tower->getRange());
    int rangeTextWidth = MeasureText(rangeStr.c_str(), 14);
    DrawText(rangeStr.c_str(), pos.x - rangeTextWidth / 2, pos.y - static_cast<int>(rangeDisplayRadius) - 20, 14, DARKGRAY);

    // --- Draw Bullets ---
    const std::vector<Bullet>& bullets = tower->getBullets();
    for (const Bullet& b : bullets) {
        if (b.active) {
            DrawCircleV(b.position, 3, YELLOW);
        }
    }
}

// Draw all towers.
void TowerUIManager::drawTowers(const TowerManager& towerManager, int cellSize) {
    const std::vector<Tower*>& towers = towerManager.getTowers();
    for (const Tower* tower : towers) {
        TowerUI::drawTower(tower, cellSize);
    }
}
