#include "towerLogic.h"
#include <cmath>
#include <algorithm>


CritterManager* TowerManager::critterManager;


// ================== Tower Base Class Implementation ==================


Tower::Tower(const std::string& name, int cost, int refundValue, int range, int power, float rateOfFire)
    : name(name), level(1), cost(cost), refundValue(refundValue),
    range(range), power(power), rateOfFire(rateOfFire) {
    position = { 0, 0 };

    cooldownTimer = 0.0f;
}

Tower::~Tower() {}

void Tower::Update() {
    std::vector<CritterLogic*> critters = TowerManager::critterManager->getCritters();
    int cellSize = 40;

    float towerRangePixels = getRange() * cellSize;
    Vector2 towerPos = getPosition();

    // Find the closest critter within range.
    CritterLogic* targetCritter = nullptr;
    float minDistance = towerRangePixels;
    for (CritterLogic* critter : critters) {
        // Convert critter grid position to pixel position.
        Vector2 critterPos = {
            critter->getX() * (float)cellSize + cellSize / 2.0f,
            critter->getY() * (float)cellSize + cellSize / 2.0f
        };
        float dx = towerPos.x - critterPos.x;
        float dy = towerPos.y - critterPos.y;
        float distance = sqrt(dx * dx + dy * dy);
        if (distance <= towerRangePixels && distance < minDistance) {
            targetCritter = critter;
            minDistance = distance;
        }
    }

    // If a critter is in range and the tower is ready to shoot, fire a bullet.
    if (targetCritter && readyToShoot()) {
        Vector2 targetPos = {
            targetCritter->getX() * (float)cellSize + cellSize / 2.0f,
            targetCritter->getY() * (float)cellSize + cellSize / 2.0f
        };
        shootAt(targetPos);
        resetCooldown();
    }

    // Update bullets for this tower and check for collisions.
    // Note: We need to modify the bullets; so we get a non-const reference.
    std::vector<Bullet>& bullets = const_cast<std::vector<Bullet>&>(getBullets());
    for (Bullet& bullet : bullets) {
        if (bullet.active) {
            // Bullet position is updated in tower->updateBullets(), but we can check collision here.
            for (CritterLogic* critter : critters) {
                Vector2 critterPos = {
                    critter->getX() * (float)cellSize + cellSize / 2.0f,
                    critter->getY() * (float)cellSize + cellSize / 2.0f
                };
                float dx = bullet.position.x - critterPos.x;
                float dy = bullet.position.y - critterPos.y;
                float distance = sqrt(dx * dx + dy * dy);

                if (distance < 5.0f) { // collision threshold in pixels
                    critter->minusHealth(bullet.damage);
                    if (critter->isDead()) {

                        TowerManager::critterManager->removeCritter(critter);
                    }
                    bullet.active = false;
                    // (Optional) Check if critter is dead and remove it.
                }
            }
        }
    }
    // Finally, update bullet positions (which also advances the cooldown internally).
    updateBullets();
}

void Tower::setPosition(Vector2 pos) {
    position = pos;
}

Vector2 Tower::getPosition() const {
    return position;
}

void Tower::upgrade() {
    level++;
    power += 5;
    range += 1;
    refundValue += cost / 10;
    std::cout << name << " upgraded to level " << level << "\n";
}

int Tower::sell() const {
    int sellValue = refundValue * level;
    std::cout << name << " sold for " << sellValue << " coins.\n";
    return sellValue;
}

std::string Tower::getName() const { return name; }
int Tower::getLevel() const { return level; }
int Tower::getCost() const { return cost; }
int Tower::getRefundValue() const { return refundValue; }
int Tower::getRange() const { return range; }
int Tower::getPower() const { return power; }
float Tower::getRateOfFire() const { return rateOfFire; }

// ---------- Bullet Functionality Implementation ----------

void Tower::shootAt(Vector2 target) {
    Bullet b;
    b.position = position;
    // Compute normalized direction vector toward target.
    Vector2 dir = { target.x - position.x, target.y - position.y };
    float length = sqrt(dir.x * dir.x + dir.y * dir.y);
    if (length != 0) {
        dir.x /= length;
        dir.y /= length;
    }
    // Set bullet velocity (adjust speed as needed).
    b.velocity = { dir.x * 5.0f, dir.y * 5.0f };
    b.damage = power;  // Damage based on tower's power.
    b.active = true;
    bullets.push_back(b);
}

void Tower::updateBullets() {
    cooldownTimer += 1.0f / 60.0f;

    // Update bullet positions.
    for (auto& b : bullets) {
        if (b.active) {
            b.position.x += b.velocity.x;
            b.position.y += b.velocity.y;
            float dx = b.position.x - position.x;
            float dy = b.position.y - position.y;
            if (sqrt(dx * dx + dy * dy) > 300) {
                b.active = false;
            }
        }
    }
    // Remove inactive bullets.
    bullets.erase(std::remove_if(bullets.begin(), bullets.end(),
        [](const Bullet& b) { return !b.active; }), bullets.end());
}

bool Tower::readyToShoot() const {
    return cooldownTimer >= (1.0f / rateOfFire);
}

void Tower::resetCooldown() {
    cooldownTimer = 0.0f;
}


const std::vector<Bullet>& Tower::getBullets() const {
    return bullets;
}

// ================== Derived Towers Implementation ==================

BasicTower::BasicTower() : Tower("Basic Tower", 100, 70, 3, 25, 1.0f) {}

void BasicTower::attack() {
    // Fire only if cooldown period has passed.
    float cooldownPeriod = 1.0f / getRateOfFire(); // seconds per shot
    if (cooldownTimer >= cooldownPeriod) {
        // For demonstration, shoot to the right.
        Vector2 target = { getPosition().x + 100, getPosition().y };
        shootAt(target);
        cooldownTimer = 0.0f;
        std::cout << name << " attacks with direct damage, power: " << power << "\n";
    }
}

TowerType BasicTower::getTowerType() const {
    return TowerType::BASIC;
}

SplashTower::SplashTower() : Tower("Splash Tower", 150, 100, 2, 20, 0.8f) {}

void SplashTower::attack() {
    float cooldownPeriod = 1.0f / getRateOfFire();
    if (cooldownTimer >= cooldownPeriod) {
        Vector2 target = { getPosition().x + 100, getPosition().y };
        shootAt(target);
        cooldownTimer = 0.0f;
        std::cout << name << " attacks with splash damage, power: " << power << "\n";
    }
}

TowerType SplashTower::getTowerType() const {
    return TowerType::SPLASH;
}

SlowTower::SlowTower() : Tower("Slow Tower", 120, 80, 3, 15, 1.2f) {}

void SlowTower::attack() {
    float cooldownPeriod = 1.0f / getRateOfFire();
    if (cooldownTimer >= cooldownPeriod) {
        Vector2 target = { getPosition().x + 100, getPosition().y };
        shootAt(target);
        cooldownTimer = 0.0f;
        std::cout << name << " attacks and slows enemies, power: " << power << "\n";
    }
}

TowerType SlowTower::getTowerType() const {
    return TowerType::SLOW;
}

// ================== TowerManager Implementation ==================

TowerManager::TowerManager(){
}

TowerManager::~TowerManager() {
    for (Tower* tower : towers) {
        Detach(tower);
        delete tower;
    }
    towers.clear();
}

void TowerManager::addTower(Tower* tower) {

    Attach(tower);
    towers.push_back(tower);
    std::cout << tower->getName() << " added.\n";
}

void TowerManager::removeTower(int index) {

    if (index < 0 || index >= static_cast<int>(towers.size())) {
        std::cout << "Invalid tower index.\n";
        return;
    }
    std::cout << towers[index]->getName() << " removed.\n";

    Detach(towers[index]);
    delete towers[index];
    towers.erase(towers.begin() + index);
}


void TowerManager::updateTowers( int cellSize) {
    // Get the list of critters from the CritterManager.
    // Iterate over all towers.
    Notify();
}

void TowerManager::upgradeTower(int index) {
    if (index < 0 || index >= static_cast<int>(towers.size())) {
        std::cout << "Invalid tower index for upgrade.\n";
        return;
    }
    towers[index]->upgrade();
}

int TowerManager::sellTower(int index) {
    if (index < 0 || index >= static_cast<int>(towers.size())) {
        std::cout << "Invalid tower index for selling.\n";
        return 0;
    }
    int sellValue = towers[index]->sell();
    Detach(towers[index]);
    delete towers[index];
    towers.erase(towers.begin() + index);
    return sellValue;
}

const std::vector<Tower*>& TowerManager::getTowers() const {
    return towers;
}

void TowerManager::printTowers() const {
    std::cout << "Tower Manager - Towers:\n";
    for (size_t i = 0; i < towers.size(); i++) {
        std::cout << i << ": " << towers[i]->getName()
            << " (Level " << towers[i]->getLevel() << ")\n";
    }
}
