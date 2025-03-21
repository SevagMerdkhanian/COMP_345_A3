#include "towerLogic.h"
#include <cmath>
#include <algorithm>

// --------------------
// TowerManager Static Member
// --------------------
CritterManager* TowerManager::critterManager;

// ================== Tower Base Class Implementation ==================

Tower::Tower(const std::string& name, int cost, int refundValue, int range, int power, float rateOfFire, TowerTargetingStrategy* strategy)
    : name(name), level(1), cost(cost), refundValue(refundValue),
    range(range), power(power), rateOfFire(rateOfFire), targetingStrategy(strategy),
    cooldownTimer(0.0f) {
    position = { 0, 0 };
}

Tower::~Tower() {}

void Tower::Update() {
    auto critters = TowerManager::critterManager->getCritters();
    int cellSize = 40;
    float towerRangePixels = getRange() * cellSize;
    Vector2 towerPos = getPosition();

    // Use targetingStrategy to find the appropriate target
    CritterLogic* targetCritter = targetingStrategy->GetTargetCritter(critters, cellSize, towerRangePixels, towerPos);

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
    std::vector<Bullet>& bullets = const_cast<std::vector<Bullet>&>(getBullets());
    for (Bullet& bullet : bullets) {
        if (bullet.active) {
            for (auto& critter : critters) {
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
                }
            }
        }
    }
    updateBullets();
}

void Tower::setPosition(Vector2 pos) {
    position = pos;
}

Vector2 Tower::getPosition() const {
    return position;
}

void Tower::upgrade() {
    // Use PowerUpgradeDecorator for all towers.
    PowerUpgradeDecorator temp(this, false); 
    temp.upgrade();
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
    Vector2 dir = { target.x - position.x, target.y - position.y };
    float length = sqrt(dir.x * dir.x + dir.y * dir.y);
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
    cooldownTimer += 1.0f / 60.0f;
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

BasicTower::BasicTower() : Tower("Basic Tower", 100, 70, 3, 25, 1.0f, this) {}

void BasicTower::attack() {
    float cooldownPeriod = 1.0f / getRateOfFire();
    if (cooldownTimer >= cooldownPeriod) {
        Vector2 target = { getPosition().x + 100, getPosition().y };
        shootAt(target);
        cooldownTimer = 0.0f;
        std::cout << name << " attacks with direct damage, power: " << power << "\n";
    }
}

CritterLogic* BasicTower::GetTargetCritter(std::vector<CritterLogic*>& critters, int cellSize, int towerRangePixels, Vector2 towerPos) {
    CritterLogic* targetCritter = nullptr;
    float minDistance = towerRangePixels;
    for (auto& critter : critters) {
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
    return targetCritter;
}

TowerType BasicTower::getTowerType() const {
    return TowerType::BASIC;
}

SplashTower::SplashTower() : Tower("Splash Tower", 150, 100, 2, 20, 0.8f, this) {}

void SplashTower::attack() {
    float cooldownPeriod = 1.0f / getRateOfFire();
    if (cooldownTimer >= cooldownPeriod) {
        Vector2 target = { getPosition().x + 100, getPosition().y };
        shootAt(target);
        cooldownTimer = 0.0f;
        std::cout << name << " attacks with splash damage, power: " << power << "\n";
    }
}

CritterLogic* SplashTower::GetTargetCritter(std::vector<CritterLogic*>& critters, int cellSize, int towerRangePixels, Vector2 towerPos) {
    CritterLogic* nearestCritter = nullptr;
    float minDistanceToTower = towerRangePixels;
    std::vector<CritterLogic*> crittersInRange;
    for (auto& critter : critters) {
        Vector2 critterPos = {
            critter->getX() * (float)cellSize + cellSize / 2.0f,
            critter->getY() * (float)cellSize + cellSize / 2.0f
        };
        float dx = towerPos.x - critterPos.x;
        float dy = towerPos.y - critterPos.y;
        float distance = sqrt(dx * dx + dy * dy);
        if (distance <= towerRangePixels) {
            crittersInRange.push_back(critter);
            if (distance < minDistanceToTower) {
                nearestCritter = critter;
                minDistanceToTower = distance;
            }
        }
    }
    if (crittersInRange.size() == 1) {
        return nearestCritter;
    }
    CritterLogic* closestToExit = nullptr;
    int minDistanceToExit = INT_MAX;
    for (auto& critter : crittersInRange) {
        int distanceToExit = critter->getDistanceToExit();
        if (distanceToExit < minDistanceToExit) {
            closestToExit = critter;
            minDistanceToExit = distanceToExit;
        }
    }
    return closestToExit;
}

TowerType SplashTower::getTowerType() const {
    return TowerType::SPLASH;
}

SlowTower::SlowTower() : Tower("Slow Tower", 120, 80, 3, 15, 1.2f, this) {}

void SlowTower::attack() {
    float cooldownPeriod = 1.0f / getRateOfFire();
    if (cooldownTimer >= cooldownPeriod) {
        Vector2 target = { getPosition().x + 100, getPosition().y };
        shootAt(target);
        cooldownTimer = 0.0f;
        std::cout << name << " attacks and slows enemies, power: " << power << "\n";
    }
}

CritterLogic* SlowTower::GetTargetCritter(std::vector<CritterLogic*>& critters, int cellSize, int towerRangePixels, Vector2 towerPos) {
    CritterLogic* weakestCritter = nullptr;
    float minHealth = std::numeric_limits<float>::max();
    for (auto& critter : critters) {
        Vector2 critterPos = {
            critter->getX() * (float)cellSize + cellSize / 2.0f,
            critter->getY() * (float)cellSize + cellSize / 2.0f
        };
        float dx = towerPos.x - critterPos.x;
        float dy = towerPos.y - critterPos.y;
        float distance = sqrt(dx * dx + dy * dy);
        if (distance <= towerRangePixels) {
            float critterHealth = critter->getHealth();
            if (critterHealth < minHealth) {
                weakestCritter = critter;
                minHealth = critterHealth;
            }
        }
    }
    return weakestCritter;
}

TowerType SlowTower::getTowerType() const {
    return TowerType::SLOW;
}

SniperTower::SniperTower() : Tower("Sniper Tower", 110, 70, 5, 20, 0.7f, this) {}

void SniperTower::attack() {
    float cooldownPeriod = 1.0f / getRateOfFire();
    if (cooldownTimer >= cooldownPeriod) {
        Vector2 target = { getPosition().x + 100, getPosition().y };
        shootAt(target);
        cooldownTimer = 0.0f;
        std::cout << name << " attacks and slows enemies, power: " << power << "\n";
    }
}

CritterLogic* SniperTower::GetTargetCritter(std::vector<CritterLogic*>& critters, int cellSize, int towerRangePixels, Vector2 towerPos) {
    CritterLogic* strongestCritter = nullptr;
    float maxHealth = std::numeric_limits<float>::lowest();
    for (auto& critter : critters) {
        Vector2 critterPos = {
            critter->getX() * (float)cellSize + cellSize / 2.0f,
            critter->getY() * (float)cellSize + cellSize / 2.0f
        };
        float dx = towerPos.x - critterPos.x;
        float dy = towerPos.y - critterPos.y;
        float distance = sqrt(dx * dx + dy * dy);
        if (distance <= towerRangePixels) {
            float critterHealth = critter->getHealth();
            if (critterHealth > maxHealth) {
                strongestCritter = critter;
                maxHealth = critterHealth;
            }
        }
    }
    return strongestCritter;
}

TowerType SniperTower::getTowerType() const {
    return TowerType::SNIPER;
}

// ================== TowerDecorator Implementation ==================

TowerDecorator::TowerDecorator(Tower* tower, bool takeOwnership)
// We call the base constructor with values taken from the wrapped tower.
    : Tower(tower->getName(), tower->getCost(), tower->getRefundValue(), tower->getRange(), tower->getPower(), tower->getRateOfFire(), nullptr),
    decoratedTower(tower), ownsComponent(takeOwnership) {
}

TowerDecorator::~TowerDecorator() {
    if (ownsComponent) {
        delete decoratedTower;
    }
}

void TowerDecorator::Update() {
    decoratedTower->Update();
}

void TowerDecorator::attack() {
    decoratedTower->attack();
}

TowerType TowerDecorator::getTowerType() const {
    return decoratedTower->getTowerType();
}

// The default behavior here is delegation. (This method will be overridden in concrete decorators.)
void TowerDecorator::upgrade() {
    decoratedTower->upgrade();
}

int TowerDecorator::sell() const {
    return decoratedTower->sell();
}

void TowerDecorator::setPosition(Vector2 pos) {
    decoratedTower->setPosition(pos);
}

Vector2 TowerDecorator::getPosition() const {
    return decoratedTower->getPosition();
}

const std::vector<Bullet>& TowerDecorator::getBullets() const {
    return decoratedTower->getBullets();
}

// ================== Concrete Upgrade Decorators ==================

// PowerUpgradeDecorator increases the tower's damage more aggressively.
PowerUpgradeDecorator::PowerUpgradeDecorator(Tower* tower, bool takeOwnership)
    : TowerDecorator(tower, takeOwnership) {
}

void PowerUpgradeDecorator::upgrade() {
    // Directly update the decorated tower's fields.
    decoratedTower->level++;
    decoratedTower->power += 10; // extra bonus to power
    decoratedTower->range += 1;
    decoratedTower->refundValue += decoratedTower->cost / 10;
    std::cout << decoratedTower->name << " powered-up to level " << decoratedTower->level
        << " with increased damage!\n";
}

// RangeUpgradeDecorator increases the tower's range more.
RangeUpgradeDecorator::RangeUpgradeDecorator(Tower* tower, bool takeOwnership)
    : TowerDecorator(tower, takeOwnership) {
}

void RangeUpgradeDecorator::upgrade() {
    decoratedTower->level++;
    decoratedTower->power += 5;
    decoratedTower->range += 2; // extra bonus to range
    decoratedTower->refundValue += decoratedTower->cost / 10;
    std::cout << decoratedTower->name << " upgraded to level " << decoratedTower->level
        << " with extended range!\n";
}

// ================== TowerManager Implementation ==================

TowerManager::TowerManager() {}

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

void TowerManager::updateTowers(int cellSize) {
    Notify();
}

void TowerManager::upgradeTower(int index) {
    if (index < 0 || index >= static_cast<int>(towers.size())) {
        std::cout << "Invalid tower index for upgrade.\n";
        return;
    }
    // Calling upgrade() now automatically uses the decorator logic.
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
        std::cout << i << ": " << towers[i]->getName() << " (Level " << towers[i]->getLevel() << ")\n";
    }
}
