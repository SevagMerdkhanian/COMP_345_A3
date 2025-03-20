#include "critterLogic.h"
#include "CritterFactory.h"
#include <memory>

//Superclass
//creates a critter based on type
CritterLogic::CritterLogic(MapLogic& mapLogic, int level) : mapLogic(mapLogic), level(level), x(mapLogic.getEntryX()), y(mapLogic.getEntryY()), lastX(-1), lastY(-1), frameCounter(0) {}

void CritterLogic::move() {
    std::vector<std::vector<Cell>> map = mapLogic.getMap();
    std::vector<std::pair<int, int>> possibleMoves;

    if (frameCounter < moveInterval) {
        frameCounter++;  // Increase frame counter
        return;  // Skip movement if not time to move yet
    }

    frameCounter = 0;  // Reset counter after movement

    // Ensure x and y are within bounds before accessing map[y][x]
    if (y >= 0 && y < map.size() && x + 1 >= 0 && x + 1 < map[0].size() &&
        (map[y][x + 1].type == CellType::PATH || map[y][x + 1].type == CellType::EXIT) && (x + 1 != lastX || y != lastY)) {
        possibleMoves.emplace_back(x + 1, y);
    }
    if (y + 1 >= 0 && y + 1 < map.size() && x >= 0 && x < map[0].size() &&
        (map[y + 1][x].type == CellType::PATH || map[y + 1][x].type == CellType::EXIT) && (x != lastX || y + 1 != lastY)) {
        possibleMoves.emplace_back(x, y + 1);
    }
    if (y >= 0 && y < map.size() && x - 1 >= 0 && x - 1 < map[0].size() &&
        (map[y][x - 1].type == CellType::PATH || map[y][x - 1].type == CellType::EXIT) && (x - 1 != lastX || y != lastY)) {
        possibleMoves.emplace_back(x - 1, y);
    }
    if (y - 1 >= 0 && y - 1 < map.size() && x >= 0 && x < map[0].size() &&
        (map[y - 1][x].type == CellType::PATH || map[y - 1][x].type == CellType::EXIT) && (x != lastX || y - 1 != lastY)) {
        possibleMoves.emplace_back(x, y - 1);
    }


    if (!possibleMoves.empty()) {
        std::srand(std::time(0));
        std::pair<int, int> chosenMove = possibleMoves[std::rand() % possibleMoves.size()];
        int newX = chosenMove.first;
        int newY = chosenMove.second;

        lastX = x;
        lastY = y;
        x = newX;
        y = newY;
    }

}

void CritterLogic::takeDamage(int damage) {
    hit_points -= damage;
}

bool CritterLogic::isDead() const {
    return hit_points <= 0;
}

void CritterLogic::Update() {
    move();
}

int CritterLogic::getHealth() const { return hit_points; }
int CritterLogic::getMaxHealth() const { return maxHealth; }
void CritterLogic::minusHealth(int minusHealth) { hit_points -= minusHealth; }
int CritterLogic::getX() const { return x; }
int CritterLogic::getY() const { return y; }
int CritterLogic::getDistanceToExit() const {
    int exitX = mapLogic.getExitX();
    int exitY = mapLogic.getExitY();
    return std::abs(x - exitX) + std::abs(y - exitY);
}
CritterType CritterLogic::getType() const { return critterType; }
std::string CritterLogic::critterTypeToString(CritterType type) {
    switch (type) {
        case CritterType::TANKY: return "Tanky";
        case CritterType::SPEEDY: return "Speedy";
        case CritterType::STRONG: return "Strong";
        case CritterType::BALANCED: return "Balanced";
        default: return "Unknown";
    }
}

//Speedy
SpeedyCritter::SpeedyCritter(MapLogic& mapLogic, int level) : CritterLogic::CritterLogic(mapLogic, level)
{
    critterType = SPEEDY;
    hit_points = maxHealth = 30 + (level * 5);
    strength = 4 + (level * 2);
    reward = 10 + (level * 4);
    moveInterval = 30;
}
void SpeedyCritter::render(Vector2 position) {
    Color critterColor;

    critterColor = MAGENTA;

    Vector2 v1 = { position.x, position.y - 10 - (10.0f / 3.0f) }; // position.y - 40/3
    Vector2 v2 = { position.x - 10, position.y + 10 - (10.0f / 3.0f) }; // position.y + 20/3
    Vector2 v3 = { position.x + 10, position.y + 10 - (10.0f / 3.0f) }; // position.y + 20/3

    DrawTriangle(v1, v2, v3, critterColor);

    // Draw Health Bar
    float healthBarWidth = 20.0f; // Max width of health bar
    float healthPercentage = (float)getHealth() / getMaxHealth();
    float barWidth = healthBarWidth * healthPercentage; // Scale based on health

    Vector2 healthBarPos = { position.x - (healthBarWidth / 2), position.y - 10 }; // Above critter

    DrawRectangle(healthBarPos.x, healthBarPos.y, barWidth, 4, GREEN);            // Health amount
    DrawRectangleLines(healthBarPos.x, healthBarPos.y, healthBarWidth, 4, BLACK); // Border
}

//Tanky
TankyCritter::TankyCritter(MapLogic& mapLogic, int level) : CritterLogic::CritterLogic(mapLogic, level) {
    critterType = TANKY;
    hit_points = maxHealth = 100 + (level * 15);
    strength = 5 + (level * 2);
    reward = 15 + (level * 6);
    moveInterval = 90;
}
void TankyCritter::render(Vector2 position) {
    Color critterColor;

    critterColor = ORANGE;

    Vector2 v1 = { position.x, position.y - 10 - (10.0f / 3.0f) }; // position.y - 40/3
    Vector2 v2 = { position.x - 10, position.y + 10 - (10.0f / 3.0f) }; // position.y + 20/3
    Vector2 v3 = { position.x + 10, position.y + 10 - (10.0f / 3.0f) }; // position.y + 20/3

    DrawTriangle(v1, v2, v3, critterColor);

    // Draw Health Bar
    float healthBarWidth = 20.0f; // Max width of health bar
    float healthPercentage = (float)getHealth() / getMaxHealth();
    float barWidth = healthBarWidth * healthPercentage; // Scale based on health

    Vector2 healthBarPos = { position.x - (healthBarWidth / 2), position.y - 10 }; // Above critter

    DrawRectangle(healthBarPos.x, healthBarPos.y, barWidth, 4, GREEN);            // Health amount
    DrawRectangleLines(healthBarPos.x, healthBarPos.y, healthBarWidth, 4, BLACK); // Border
}

//Strong
StrongCritter::StrongCritter(MapLogic& mapLogic, int level) : CritterLogic::CritterLogic(mapLogic, level) {
    critterType = STRONG;
    hit_points = maxHealth = 50 + (level * 10);
    strength = 7 + (level * 2);
    reward = 12 + (level * 5);
    moveInterval = 60;
}
void StrongCritter::render(Vector2 position) {
    Color critterColor;

    critterColor = DARKPURPLE;

    Vector2 v1 = { position.x, position.y - 10 - (10.0f / 3.0f) }; // position.y - 40/3
    Vector2 v2 = { position.x - 10, position.y + 10 - (10.0f / 3.0f) }; // position.y + 20/3
    Vector2 v3 = { position.x + 10, position.y + 10 - (10.0f / 3.0f) }; // position.y + 20/3

    DrawTriangle(v1, v2, v3, critterColor);

    // Draw Health Bar
    float healthBarWidth = 20.0f; // Max width of health bar
    float healthPercentage = (float)getHealth() / getMaxHealth();
    float barWidth = healthBarWidth * healthPercentage; // Scale based on health

    Vector2 healthBarPos = { position.x - (healthBarWidth / 2), position.y - 10 }; // Above critter

    DrawRectangle(healthBarPos.x, healthBarPos.y, barWidth, 4, GREEN);            // Health amount
    DrawRectangleLines(healthBarPos.x, healthBarPos.y, healthBarWidth, 4, BLACK); // Border
}

//Balanced
BasicCritter::BasicCritter(MapLogic& mapLogic, int level) : CritterLogic::CritterLogic(mapLogic, level) {
    critterType = BALANCED;
    hit_points = maxHealth = 50 + (level * 10);
    strength = 5 + (level * 2);
    reward = 12 + (level * 5);
    moveInterval = 40;
}
void BasicCritter::render(Vector2 position) {
    Color critterColor;

    critterColor = BLACK;

    Vector2 v1 = { position.x, position.y - 10 - (10.0f / 3.0f) }; // position.y - 40/3
    Vector2 v2 = { position.x - 10, position.y + 10 - (10.0f / 3.0f) }; // position.y + 20/3
    Vector2 v3 = { position.x + 10, position.y + 10 - (10.0f / 3.0f) }; // position.y + 20/3

    DrawTriangle(v1, v2, v3, critterColor);

    // Draw Health Bar
    float healthBarWidth = 20.0f; // Max width of health bar
    float healthPercentage = (float) getHealth() / getMaxHealth();
    float barWidth = healthBarWidth * healthPercentage; // Scale based on health

    Vector2 healthBarPos = { position.x - (healthBarWidth / 2), position.y - 10 }; // Above critter

    DrawRectangle(healthBarPos.x, healthBarPos.y, barWidth, 4, GREEN);            // Health amount
    DrawRectangleLines(healthBarPos.x, healthBarPos.y, healthBarWidth, 4, BLACK); // Border
}

//Critter Manager Modified
CritterManager::CritterManager() {}
CritterManager::~CritterManager() {}

void CritterManager::addCritter(CritterLogic* critter) {
    critters.push_back(critter);
}

void CritterManager::removeCritter(CritterLogic* critter) {
    auto it = std::find(critters.begin(), critters.end(), critter);
    if (it != critters.end()) {
        delete* it;
        critters.erase(it); 
    }
}

void CritterManager::update(MapLogic& mapLogic) {
    if (crittersSpawned < totalCritters) {
        if (spawnFrameCounter >= spawnInterval) {
            addCritter(CritterFactory::createCritter(static_cast<CritterType>(rand() % 4), mapLogic, currentWave));
            crittersSpawned++;
            spawnFrameCounter = 0;
        }
        else {
            spawnFrameCounter++;
        }
    }
    for (auto& critter : critters) {
        critter->Update();
    }

    if (critters.empty() && crittersSpawned >= totalCritters) {
        startNextWave();
    }
}

void CritterManager::startNextWave() {
    std::cout << "New wave started" << currentWave;
    currentWave++;
    crittersSpawned = 0;
    totalCritters = 5 + (currentWave * 2);  // Increase critter count each wave
    spawnFrameCounter = 0;
}


void CritterManager::resetWave() {
    for (CritterLogic* critter : critters) {
        delete critter;  // Free memory
    }
    critters.clear();
    crittersSpawned = 0;
    spawnFrameCounter = 0;
}

int CritterManager::getCurrentWave() const { return currentWave; }
int CritterManager::getCrittersSpawned() const { return crittersSpawned; }
std::vector<CritterLogic*>& CritterManager::getCritters() { return critters; }

