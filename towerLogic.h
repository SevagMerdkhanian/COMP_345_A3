#ifndef TOWER_LOGIC_H
#define TOWER_LOGIC_H

#include <string>
#include <iostream>
#include <vector>
#include "raylib.h"             // For Vector2
#include "critterLogic.h"       // For CritterLogic
#include "TowerTargetingStrategy.h" // For TowerTargetingStrategy

// Assume ObserverVec and ObservableVec are defined elsewhere.
// For example:
// class ObserverVec { public: virtual void Update() {} };
// class ObservableVec { /* ... */ };

// --------------------
// Tower Type Enumeration (plain enum for UI compatibility)
// --------------------
enum TowerType {
    BASIC,
    SPLASH,
    SLOW,
    SNIPER
};

// --------------------
// Bullet Structure
// --------------------
struct Bullet {
    Vector2 position;
    Vector2 velocity;
    int damage;
    bool active;
};

// --------------------
// Tower Base Class (Component)
// --------------------
class Tower : public ObserverVec {
protected:
    std::string name;
    int level;
    int cost;
    int refundValue;
    int range;         // In grid cells
    int power;
    float rateOfFire;  // Shots per second
    Vector2 position;  // Tower position
    std::vector<Bullet> bullets;
    TowerTargetingStrategy* targetingStrategy;
    float cooldownTimer;  // In seconds
public:
    Tower(const std::string& name, int cost, int refundValue, int range, int power, float rateOfFire, TowerTargetingStrategy* strategy);
    virtual ~Tower();

    // Observer method: made concrete.
    virtual void Update();

    // Position accessors.
    virtual void setPosition(Vector2 pos);
    virtual Vector2 getPosition() const;

    // Pure virtual behavior.
    virtual void attack() = 0;
    virtual TowerType getTowerType() const = 0;

    // Upgrade and sell.
    virtual void upgrade();
    virtual int sell() const;

    // Common attribute getters.
    virtual std::string getName() const;
    virtual int getLevel() const;
    virtual int getCost() const;
    virtual int getRefundValue() const;
    virtual int getRange() const;
    virtual int getPower() const;
    virtual float getRateOfFire() const;

    // ---------- Bullet Functionality ----------
    virtual void shootAt(Vector2 target);
    virtual void updateBullets();
    virtual bool readyToShoot() const;
    virtual void resetCooldown();

    // Pure virtual: must return bullets.
    virtual const std::vector<Bullet>& getBullets() const = 0;
};

// --------------------
// Concrete Component: BasicTower
// (Contains the core tower functionality.)
// --------------------
class BasicTower : public Tower {
public:
    BasicTower();
    virtual ~BasicTower() {}

    virtual void attack() override;
    virtual TowerType getTowerType() const override;
    virtual const std::vector<Bullet>& getBullets() const override;
};

// --------------------
// Decorator Base Class: TowerDecorator
// (Wraps a Tower pointer and delegates calls by default.)
// --------------------
class TowerDecorator : public Tower {
protected:
    Tower* innerTower;
public:
    TowerDecorator(Tower* tower);
    virtual ~TowerDecorator();

    virtual void Update() override;
    virtual void setPosition(Vector2 pos) override;
    virtual Vector2 getPosition() const override;
    virtual void attack() override;
    virtual void upgrade() override;
    virtual int sell() const override;
    virtual std::string getName() const override;
    virtual int getLevel() const override;
    virtual int getCost() const override;
    virtual int getRefundValue() const override;
    virtual int getRange() const override;
    virtual int getPower() const override;
    virtual float getRateOfFire() const override;
    virtual void shootAt(Vector2 target) override;
    virtual void updateBullets() override;
    virtual bool readyToShoot() const override;
    virtual void resetCooldown() override;
    virtual const std::vector<Bullet>& getBullets() const override;
};

// --------------------
// Concrete Decorators
// --------------------
// Provide both parameterized and default constructors.
class SplashDecorator : public TowerDecorator {
public:
    SplashDecorator(Tower* tower);
    SplashDecorator();  // default constructor: wraps a new BasicTower
    virtual void attack() override;
    virtual TowerType getTowerType() const override;
};

class SlowDecorator : public TowerDecorator {
public:
    SlowDecorator(Tower* tower);
    SlowDecorator(); // default constructor: wraps a new BasicTower
    virtual void attack() override;
    virtual TowerType getTowerType() const override;
};

class SniperDecorator : public TowerDecorator {
public:
    SniperDecorator(Tower* tower);
    SniperDecorator(); // default constructor: wraps a new BasicTower
    virtual void attack() override;
    virtual TowerType getTowerType() const override;
};

// For backward compatibility with UI code.
typedef SplashDecorator SplashTower;
typedef SlowDecorator SlowTower;
typedef SniperDecorator SniperTower;

// --------------------
// TowerManager Class (Observer Subject)
// --------------------
class TowerManager : public ObservableVec {
private:
    std::vector<Tower*> towers;
public:
    static CritterManager* critterManager;
    TowerManager();
    virtual ~TowerManager();

    // Observer methods.
    void Attach(Tower* tower);
    void Detach(Tower* tower);
    void Notify();

    // Tower management.
    void addTower(Tower* tower);
    void removeTower(int index);
    void updateTowers(int cellSize);
    void upgradeTower(int index);
    int sellTower(int index);
    const std::vector<Tower*>& getTowers() const;
    void printTowers() const;
};

#endif // TOWER_LOGIC_H
