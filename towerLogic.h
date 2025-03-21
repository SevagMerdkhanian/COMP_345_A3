#ifndef TOWER_LOGIC_H
#define TOWER_LOGIC_H

#include <string>
#include <iostream>
#include <vector>
#include "raylib.h"  // For Vector2
#include "critterLogic.h"
#include "TowerTargetingStrategy.h"

// Enum to distinguish tower types
enum class TowerType {
    BASIC,
    SPLASH,
    SLOW,
    SNIPER
};

// --------------------
// Bullet structure
// --------------------
struct Bullet {
    Vector2 position;
    Vector2 velocity;
    int damage;
    bool active;
};

// --------------------
// Tower Interface
// --------------------
class ITower {
public:
    virtual ~ITower() = default;
    virtual void Update() = 0;
    virtual void attack() = 0;
    virtual TowerType getTowerType() const = 0;
    virtual void upgrade() = 0;
    virtual int sell() const = 0;
    virtual void setPosition(Vector2 pos) = 0;
    virtual Vector2 getPosition() const = 0;
    virtual const std::vector<Bullet>& getBullets() const = 0;
};

// --------------------
// Tower Base Class
// --------------------
class Tower : public ITower, public ObserverVec {
protected:
    std::string name;
    int level;
    int cost;
    int refundValue;
    int range;      // In grid cells
    int power;
    float rateOfFire;  // Shots per second
    Vector2 position;  // Tower position
    std::vector<Bullet> bullets;
    TowerTargetingStrategy* targetingStrategy;
    // Cooldown timer (in seconds) to control rate of fire.
    float cooldownTimer;
public:
    Tower(const std::string& name, int cost, int refundValue, int range, int power, float rateOfFire, TowerTargetingStrategy* strategy);
    virtual ~Tower();

    // ITower interface implementations
    void Update() override;
    void setPosition(Vector2 pos) override;
    Vector2 getPosition() const override;
    virtual void attack() override = 0;
    virtual TowerType getTowerType() const override = 0;
    // Upgrade now uses a decorator internally.
    virtual void upgrade() override;
    virtual int sell() const override;

    // Additional getters for common attributes
    std::string getName() const;
    int getLevel() const;
    int getCost() const;
    int getRefundValue() const;
    int getRange() const;
    int getPower() const;
    float getRateOfFire() const;

    // ---------- Bullet Functionality ----------
    void shootAt(Vector2 target);
    void updateBullets();
    bool readyToShoot() const;
    void resetCooldown();
    const std::vector<Bullet>& getBullets() const override;

    // Allow our decorators to access protected members.
    friend class TowerDecorator;
    friend class PowerUpgradeDecorator;
    friend class RangeUpgradeDecorator;
};

// --------------------
// Derived Towers
// --------------------

// BasicTower targets the nearest critter to the tower.
class BasicTower : public Tower, public TowerTargetingStrategy {
public:
    BasicTower();
    virtual void attack() override;
    CritterLogic* GetTargetCritter(std::vector<CritterLogic*>& critters, int cellSize, int towerRangePixels, Vector2 towerPos);
    virtual TowerType getTowerType() const override;
};

// SplashTower targets the critter nearest to the exit.
class SplashTower : public Tower, public TowerTargetingStrategy {
public:
    SplashTower();
    virtual void attack() override;
    CritterLogic* GetTargetCritter(std::vector<CritterLogic*>& critters, int cellSize, int towerRangePixels, Vector2 towerPos);
    virtual TowerType getTowerType() const override;
};

// SlowTower targets the weakest (least health) critter.
class SlowTower : public Tower, public TowerTargetingStrategy {
public:
    SlowTower();
    virtual void attack() override;
    CritterLogic* GetTargetCritter(std::vector<CritterLogic*>& critters, int cellSize, int towerRangePixels, Vector2 towerPos);
    virtual TowerType getTowerType() const override;
};

// SniperTower targets the strongest (most health) critter.
class SniperTower : public Tower, public TowerTargetingStrategy {
public:
    SniperTower();
    virtual void attack() override;
    CritterLogic* GetTargetCritter(std::vector<CritterLogic*>& critters, int cellSize, int towerRangePixels, Vector2 towerPos);
    virtual TowerType getTowerType() const override;
};

// --------------------
// TowerDecorator Class
// --------------------
class TowerDecorator : public Tower {
protected:
    // Decorated tower (the component being wrapped)
    Tower* decoratedTower;
    bool ownsComponent;
public:
    // The decorator takes a Tower pointer.
    // If takeOwnership is true, the decorator will delete the tower in its destructor.
    TowerDecorator(Tower* tower, bool takeOwnership = true);
    virtual ~TowerDecorator();

    // ITower interface delegation
    void Update() override;
    void attack() override;
    TowerType getTowerType() const override;
    virtual void upgrade() override;
    int sell() const override;
    void setPosition(Vector2 pos) override;
    Vector2 getPosition() const override;
    const std::vector<Bullet>& getBullets() const override;
};

// --------------------
// Concrete Upgrade Decorators
// --------------------
class PowerUpgradeDecorator : public TowerDecorator {
public:
    // When used for self-decoration, set takeOwnership to false.
    PowerUpgradeDecorator(Tower* tower, bool takeOwnership = true);
    void upgrade() override;
};

class RangeUpgradeDecorator : public TowerDecorator {
public:
    RangeUpgradeDecorator(Tower* tower, bool takeOwnership = true);
    void upgrade() override;
};

// --------------------
// TowerManager Class
// --------------------
class TowerManager : public ObservableVec {
private:
    std::vector<Tower*> towers;
public:
    static CritterManager* critterManager;
    TowerManager();
    ~TowerManager();

    // Add or remove towers
    void addTower(Tower* tower);
    void removeTower(int index);

    // Call attack and update bullets on all towers
    void updateTowers(int cellSize);

    // Upgrade or sell a specific tower
    void upgradeTower(int index);
    int sellTower(int index);

    // Getter to access the towers (for UI rendering)
    const std::vector<Tower*>& getTowers() const;

    // Debug: print info about towers
    void printTowers() const;
};

#endif // TOWER_LOGIC_H
