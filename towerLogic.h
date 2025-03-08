#ifndef TOWER_LOGIC_H
#define TOWER_LOGIC_H

#include <string>
#include <iostream>
#include <vector>
#include "raylib.h"  // For Vector2
#include "critterLogic.h"

// Enum to distinguish tower types
enum class TowerType {
    BASIC,
    SPLASH,
    SLOW
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
// Tower Base Class
// --------------------
class Tower : public ObserverVec {
protected:
    std::string name;
    int level;
    int cost;
    int refundValue;
    int range;      // In grid cells
    int power;
    float rateOfFire;  // Shots per second
    Vector2 position; // Tower position
    std::vector<Bullet> bullets;


    // New: cooldown timer (in seconds) to control rate of fire.
    float cooldownTimer;
public:
    Tower(const std::string& name, int cost, int refundValue, int range, int power, float rateOfFire);
    virtual ~Tower();

    void Update() override;
    
    // Position setters/getters
    void setPosition(Vector2 pos);
    Vector2 getPosition() const;
    
    // Pure virtual functions for behavior
    virtual void attack() = 0;
    virtual TowerType getTowerType() const = 0;

    // Upgrade and sell methods
    virtual void upgrade();
    virtual int sell() const;

    // Getters for common attributes
    std::string getName() const;
    int getLevel() const;
    int getCost() const;
    int getRefundValue() const;
    int getRange() const;
    int getPower() const;
    float getRateOfFire() const;


    


    // ---------- Bullet Functionality ----------
    // Spawn a bullet aimed at a target position.
    void shootAt(Vector2 target);
    // Update all bullets (move them, mark inactive if needed) and update cooldownTimer.
    void updateBullets();
    // Access bullets for drawing.
    // Returns true if the tower is ready to shoot based on its rateOfFire.
    bool readyToShoot() const;
    // Resets the internal cooldown timer.
    void resetCooldown();


    const std::vector<Bullet>& getBullets() const;
};

// --------------------
// Derived Towers
// --------------------
class BasicTower : public Tower {
public:
    BasicTower();
    virtual void attack() override;
    virtual TowerType getTowerType() const override;
};

class SplashTower : public Tower {
public:
    SplashTower();
    virtual void attack() override;
    virtual TowerType getTowerType() const override;
};

class SlowTower : public Tower {
public:
    SlowTower();
    virtual void attack() override;
    virtual TowerType getTowerType() const override;
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
    void addTower(Tower* tower); // Attach
    void removeTower(int index); // Detach

    // Call attack and update bullets on all towers
    void updateTowers(int cellSize); // Notify

    // Upgrade or sell a specific tower
    void upgradeTower(int index);
    int sellTower(int index);

    // Getter to access the towers (for UI rendering)
    const std::vector<Tower*>& getTowers() const;
    

    // Debug: print info about towers
    void printTowers() const;
};

#endif // TOWER_LOGIC_H
