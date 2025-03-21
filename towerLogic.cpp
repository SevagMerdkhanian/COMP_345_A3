#include "towerLogic.h"
#include <cmath>
#include <algorithm>
#include <iostream>
#include <limits>
#include <climits>

// ======================================================
// Tower Base Class Implementation
// ======================================================
Tower::Tower(const std::string& name, int cost, int refundValue, int range, int power, float rateOfFire, TowerTargetingStrategy* strategy)
    : name(name), level(1), cost(cost), refundValue(refundValue),
    range(range), power(power), rateOfFire(rateOfFire), targetingStrategy(strategy),
    cooldownTimer(0.0f) {
    position = { 0, 0 };
}

Tower::~Tower() {}

void Tower::Update() {
    // Default update: attack then update bullets.
    attack();
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

void Tower::shootAt(Vector2 target) {
    Bullet b;
    b.position = position;
    Vector2 dir = { target.x - position.x, target.y - position.y };
    float length = std::sqrt(dir.x * dir.x + dir.y * dir.y);
    if (length != 0) {
        dir.x /= length;
        dir.y /= length;
    }
    b.velocity = { dir.x * 5.0f, dir.y * 5.0f };
    b.damage = power;
    b.active = true;
    bullets.push_back(b);
}

void Tower::updateBullets() {
    // Advance the cooldown timer.
    cooldownTimer += 1.0f / 60.0f;

    // Get critters from the manager (if available)
    std::vector<CritterLogic*> critters;
    if (TowerManager::critterManager) {
        critters = TowerManager::critterManager->getCritters();
    }

    // For each bullet, update position and check collisions.
    for (auto& b : bullets) {
        if (b.active) {
            // Update bullet position.
            b.position.x += b.velocity.x;
            b.position.y += b.velocity.y;

            // Check collision with any critter.
            for (CritterLogic* critter : critters) {
                Vector2 critterPos = {
                    critter->getX() * 40.0f + 20.0f,  // cellSize 40, center offset 20
                    critter->getY() * 40.0f + 20.0f
                };

                float dx = b.position.x - critterPos.x;
                float dy = b.position.y - critterPos.y;
                float distance = std::sqrt(dx * dx + dy * dy);

                if (distance < 5.0f) { // Collision threshold.
                    critter->minusHealth(b.damage);
                    if (critter->isDead()) {
                        TowerManager::critterManager->removeCritter(critter);
                    }
                    b.active = false;
                    break; // Stop checking after a collision.
                }
            }
            // Additionally, deactivate the bullet if it has traveled too far.
            float dx = b.position.x - position.x;
            float dy = b.position.y - position.y;
            if (std::sqrt(dx * dx + dy * dy) > 300)
                b.active = false;
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

// ======================================================
// BasicTower Implementation
// ======================================================
BasicTower::BasicTower()
    : Tower("Basic Tower", 100, 70, 3, 25, 1.0f, nullptr) {
    // Additional initialization if needed.
}

void BasicTower::attack() {
    if (readyToShoot()) {
        // Only fire if there is at least one critter in range.
        if (TowerManager::critterManager) {
            std::vector<CritterLogic*> critters = TowerManager::critterManager->getCritters();
            int cellSize = 40;
            float towerRangePixels = getRange() * cellSize;
            Vector2 towerPos = getPosition();
            CritterLogic* targetCritter = nullptr;

            // Find the first critter within range.
            for (CritterLogic* critter : critters) {
                Vector2 critterPos = {
                    critter->getX() * static_cast<float>(cellSize) + cellSize / 2.0f,
                    critter->getY() * static_cast<float>(cellSize) + cellSize / 2.0f
                };

                float dx = towerPos.x - critterPos.x;
                float dy = towerPos.y - critterPos.y;
                float distance = std::sqrt(dx * dx + dy * dy);
                if (distance <= towerRangePixels) {
                    targetCritter = critter;
                    break;
                }
            }

            // Only shoot if a target critter is found.
            if (targetCritter) {
                Vector2 targetPos = {
                    targetCritter->getX() * static_cast<float>(cellSize) + cellSize / 2.0f,
                    targetCritter->getY() * static_cast<float>(cellSize) + cellSize / 2.0f
                };
                shootAt(targetPos);
                resetCooldown();
                std::cout << name << " attacks a critter (power: " << power << ").\n";
                return;
            }
        }
        // If no critter is found, do not shoot.
    }
}


TowerType BasicTower::getTowerType() const {
    return BASIC;
}

const std::vector<Bullet>& BasicTower::getBullets() const {
    return bullets;
}

// ======================================================
// TowerDecorator Implementation
// ======================================================
TowerDecorator::TowerDecorator(Tower* tower)
    : Tower("", 0, 0, 0, 0, 0.0f, nullptr), innerTower(tower) {
    // Decorator does not use its own attributes.
}

TowerDecorator::~TowerDecorator() {
    delete innerTower;
}

void TowerDecorator::Update() { innerTower->Update(); }
void TowerDecorator::setPosition(Vector2 pos) { innerTower->setPosition(pos); }
Vector2 TowerDecorator::getPosition() const { return innerTower->getPosition(); }
void TowerDecorator::attack() { innerTower->attack(); }
void TowerDecorator::upgrade() { innerTower->upgrade(); }
int TowerDecorator::sell() const { return innerTower->sell(); }
std::string TowerDecorator::getName() const { return innerTower->getName(); }
int TowerDecorator::getLevel() const { return innerTower->getLevel(); }
int TowerDecorator::getCost() const { return innerTower->getCost(); }
int TowerDecorator::getRefundValue() const { return innerTower->getRefundValue(); }
int TowerDecorator::getRange() const { return innerTower->getRange(); }
int TowerDecorator::getPower() const { return innerTower->getPower(); }
float TowerDecorator::getRateOfFire() const { return innerTower->getRateOfFire(); }
void TowerDecorator::shootAt(Vector2 target) { innerTower->shootAt(target); }
void TowerDecorator::updateBullets() { innerTower->updateBullets(); }
bool TowerDecorator::readyToShoot() const { return innerTower->readyToShoot(); }
void TowerDecorator::resetCooldown() { innerTower->resetCooldown(); }
const std::vector<Bullet>& TowerDecorator::getBullets() const { return innerTower->getBullets(); }

// ======================================================
// Concrete Decorators
// ======================================================
SplashDecorator::SplashDecorator(Tower* tower) : TowerDecorator(tower) {}
SplashDecorator::SplashDecorator() : TowerDecorator(new BasicTower()) {}
void SplashDecorator::attack() {
    std::cout << getName() << " applies splash damage effect!\n";
    TowerDecorator::attack();
}
TowerType SplashDecorator::getTowerType() const {
    return SPLASH;
}

SlowDecorator::SlowDecorator(Tower* tower) : TowerDecorator(tower) {}
SlowDecorator::SlowDecorator() : TowerDecorator(new BasicTower()) {}
void SlowDecorator::attack() {
    std::cout << getName() << " applies a slowing effect to enemies!\n";
    TowerDecorator::attack();
}
TowerType SlowDecorator::getTowerType() const {
    return SLOW;
}

SniperDecorator::SniperDecorator(Tower* tower) : TowerDecorator(tower) {}
SniperDecorator::SniperDecorator() : TowerDecorator(new BasicTower()) {}
void SniperDecorator::attack() {
    std::cout << getName() << " executes a sniper shot!\n";
    TowerDecorator::attack();
}
TowerType SniperDecorator::getTowerType() const {
    return SNIPER;
}

// ======================================================
// TowerManager Implementation (Observer Pattern)
// ======================================================
CritterManager* TowerManager::critterManager = nullptr;

TowerManager::TowerManager() {}

TowerManager::~TowerManager() {
    for (Tower* tower : towers) {
        Detach(tower);
        delete tower;
    }
    towers.clear();
}

void TowerManager::Attach(Tower* tower) {
    std::cout << "Attaching " << tower->getName() << ".\n";
}

void TowerManager::Detach(Tower* tower) {
    std::cout << "Detaching " << tower->getName() << ".\n";
}

void TowerManager::Notify() {
    for (Tower* tower : towers) {
        tower->Update();
    }
}

void TowerManager::addTower(Tower* tower) {
    Attach(tower);
    towers.push_back(tower);
    std::cout << tower->getName() << " added to TowerManager.\n";
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

void TowerManager::updateTowers(int cellSize) {
    // cellSize is preserved if needed.
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
        Vector2 pos = towers[i]->getPosition();
        std::cout << i << ": " << towers[i]->getName() << " (Level " << towers[i]->getLevel()
            << ", Pos: " << pos.x << ", " << pos.y << ")\n";
    }
}
