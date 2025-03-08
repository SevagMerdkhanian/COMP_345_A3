#include "critterLogic.h"

//creates a critter based on type
CritterLogic::CritterLogic(MapLogic& mapLogic, CritterType type, int lvl) : mapLogic(mapLogic), critterType(type), level(lvl), frameCounter(0) {
    //x = startingcell[x];
    //y = startingcell[y];
    x = mapLogic.getEntryX();
    y = mapLogic.getEntryY();
    lastX = -1;
    lastY = -1;

    switch (type) {
    case CritterType::TANKY:
        hit_points,maxHealth = 100 + (lvl * 15);
        strength = 5 + (lvl * 2);
        speed = 1;
        reward = 15 + (lvl * 6);
        moveInterval = 90;
        break;
    case CritterType::SPEEDY:
        hit_points, maxHealth = 40 + (lvl * 5);
        strength = 4 + (lvl * 2);
        //speed = 3 + (lvl / 2);
        speed = 1;
        reward = 10 + (lvl * 4);
        moveInterval = 30;
        break;
    case CritterType::STRONG:
        hit_points, maxHealth = 50 + (lvl * 10);
        strength = 7 + (lvl * 2);
        //speed = 2 + (lvl / 2);
        speed = 1;
        reward = 12 + (lvl * 5);
        moveInterval = 60;
        break;
    case CritterType::BALANCED:
    default:
        hit_points, maxHealth = 50 + (lvl * 10);
        strength = 5 + (lvl * 2);
        //speed = 2 + (lvl / 2);
        speed = 1;
        reward = 12 + (lvl * 5);
        moveInterval = 40;
        break;
    }
}

CritterType CritterLogic::getType() {
    return critterType;
}

int CritterLogic::getHealth() {
    return hit_points;
}

int CritterLogic::getMaxHealth() const {
    return maxHealth;
}

int CritterLogic::getX() {
    return x;
}
int CritterLogic::getY() {
    return y;
}



void CritterLogic::minusHealth(int minusHealth) {
    hit_points -= minusHealth;
}

//checks if a critter is dead
bool CritterLogic::isDead() const {
    return hit_points <= 0;
}

//spawn critter group based on the wave
std::vector<CritterLogic> CritterLogic::generateCritter(int wave, int numOfCritters, MapLogic& mapLogic) {
    std::vector<CritterLogic> critters;
    CritterType critterType[] = { CritterType::SPEEDY, CritterType::TANKY, CritterType::STRONG };

    for (int i = 0; i < numOfCritters; i++) {
        CritterType type = critterType[rand() % 3];
        critters.emplace_back(mapLogic, type, wave);
    }

    return critters;
}

//movement
void CritterLogic::move(MapLogic& mapLogic) {
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

void CritterLogic::render(const Critter& critter) {
    Vector2 position = { (float)critter.x * 20, (float)critter.y * 20 };
    Color critterColor;

    switch (critter.type) {
    case CritterType::TANKY:
        critterColor = BLUE;
        break;
    case CritterType::SPEEDY:
        critterColor = GREEN;
        break;
    case CritterType::STRONG:
        critterColor = RED;
        break;
    case CritterType::BALANCED:
    default:
        critterColor = BLACK;
        break;
    }

    DrawTriangle(Vector2{ position.x, position.y },
        Vector2{ position.x - 10, position.y + 20 },
        Vector2{ position.x + 10, position.y + 20 },
        critterColor);
}

std::string CritterLogic::critterTypeToString(CritterType type) {
    switch (type) {
    case CritterType::TANKY: return "Tanky";
    case CritterType::SPEEDY: return "Speedy";
    case CritterType::STRONG: return "Strong";
    case CritterType::BALANCED: return "Balanced";
    default: return "Unknown";
    }
}

//------------------------------------------------------------------------------------------------
// CritterManager class

CritterManager::CritterManager() {}
CritterManager::~CritterManager() {
    for (CritterLogic* critter: critters) {
        delete critter;
    }
    critters.clear();
}

void CritterManager::addCritter(CritterLogic* critter) {
    critters.push_back(critter);
    std::cout << critter->critterTypeToString(critter->getType()) << " added.\n";
}


void CritterManager::removeCritter(std::vector<CritterLogic*>::iterator it) {
    if (it == critters.end()) {
        std::cout << "Critter not found.\n";
        return;
    }

    std::cout << (*it)->critterTypeToString((*it)->getType()) << " removed.\n";
    delete* it;  // Free memory
    critters.erase(it);  // Remove from vector
}

int CritterManager::getCrittersSpawned() {
    return crittersSpawned;
}

void CritterManager::update(MapLogic& mapLogic, int wave, int numCritters) {
    // Spawn a new critter at regular intervals
    if (crittersSpawned < numCritters) {
        if (spawnFrameCounter >= spawnInterval) {
            // Create and add a single critter
            std::vector<CritterLogic> generatedCritter = CritterLogic::generateCritter(wave, 1, mapLogic);
            addCritter(new CritterLogic(mapLogic, generatedCritter[0].getType(), wave));

            crittersSpawned++;      // Increase spawn count
            spawnFrameCounter = 0;  // Reset the frame counter
        }
        else {
            spawnFrameCounter++;  // Increment frame counter
        }
    }

    // Update existing critters
    for (auto& critter : critters) {
        critter->move(mapLogic);
    }
}

void CritterManager::resetWave() {
    crittersSpawned = 0;      // Reset the number of critters spawned
    spawnFrameCounter = 0;    // Reset the frame counter
}

std::vector<CritterLogic*>& CritterManager::getCritters() {
    return critters;
}