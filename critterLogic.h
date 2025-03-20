#pragma once
#ifndef CRITTER_LOGIC_H
#define CRITTER_LOGIC_H

#include <vector>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <memory>
#include "raylib.h"
#include "ObservableVec.h"
#include "ObserverVec.h"

#include "mapLogic.h"

enum CritterType
{
    SPEEDY,
    TANKY,
    STRONG,
    BALANCED
};

//Critter Superclass
class CritterLogic : public ObserverVec {
public:
    CritterLogic(MapLogic& mapLogic, int level);
    virtual ~CritterLogic() = default;

    virtual void move();
    virtual void takeDamage(int damage);
    virtual bool isDead() const;
    virtual void Update() override;
    virtual void render(Vector2 position) = 0;
    std::string critterTypeToString(CritterType type);

    int getHealth() const;
    int getMaxHealth() const;
    void minusHealth(int minusHealth);
    int getX() const;
    int getY() const;
    int getDistanceToExit() const;
    CritterType getType() const;

protected:
    CritterType critterType;
    MapLogic mapLogic;
    int frameCounter;
    int moveInterval;
    int reward;
    int hit_points;
    int maxHealth;
    int strength;
    int level;
    int x, y;
    int lastX, lastY;
};

// Concrete Classes for Different Critters

//Speedy - Higher speed then the rest of the types
class SpeedyCritter : public CritterLogic
{
public:
    SpeedyCritter(MapLogic& mapLogic, int level);
    void render(Vector2 position) override;
};

//Tanky - Higher health than the rest of the types
class TankyCritter : public CritterLogic
{
public:
    TankyCritter(MapLogic& mapLogic, int level);
    void render(Vector2 position) override;
};

//Strong - stronger damage dealt than the other types
class StrongCritter : public CritterLogic
{
public:
    StrongCritter(MapLogic& mapLogic, int level);
    void render(Vector2 position) override;
};

//Basic - the most balanced of them all
class BasicCritter : public CritterLogic
{
public:
    BasicCritter(MapLogic& mapLogic, int level);
    void render(Vector2 position) override;
};

//Modified CritterManager
class CritterManager : public ObservableVec
{

public:
    CritterManager();
    ~CritterManager();

    void addCritter(CritterLogic* critter);
    void removeCritter(CritterLogic* critter);

    void update(MapLogic& mapLogic);
    void startNextWave();
    void resetWave();
    
    int getCurrentWave() const;
    int getCrittersSpawned() const;
    std::vector<CritterLogic*>& getCritters();

private:
    std::vector<CritterLogic*> critters;
    int currentWave = 1;
    int totalCritters = 5;
    int crittersSpawned = 0;
    int spawnFrameCounter = 0;
    int spawnInterval = 120;

};
#endif