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
/**
struct Critter
{
    CritterType type;
    int reward;
    int hit_points;
    int strength;
    int speed;
    int level;
    int x, y;
};

class CritterLogic : public ObserverVec
{
public:
    CritterLogic(MapLogic &mapLogic, CritterType critterType, int level);

    CritterType getType();
    int getHealth();
    int getMaxHealth() const;
    int getX();
    int getY();
    int getDistanceToExit() const;

    void Update() override;
    void minusHealth(int minusHealth);
    void move();
    bool isDead() const;
    static std::vector<CritterLogic> generateCritter(int waveNum, int numCritters, MapLogic &mapLogic); //usually generates 1 critter. Use only in update
    static void render(const Critter &critter);
    std::string critterTypeToString(CritterType type);

private:
    CritterType critterType;
    MapLogic mapLogic;
    int frameCounter;
    int moveInterval;
    int reward;
    int hit_points;
    int maxHealth;
    int strength;
    int speed;
    int level;
    int x, y;
    int lastX, lastY;
};

class CritterManager : public ObservableVec
{

public:
    CritterManager();
    ~CritterManager();

    void addCritter(CritterLogic *critter); //attach
    void removeCritter(CritterLogic* critter); //detach


    void update(MapLogic &mapLogic, int wave, int numCritters); //notify
    void resetWave();


    int getCrittersSpawned();
    std::vector<CritterLogic *> &getCritters();

private:
    std::vector<CritterLogic*> critters;
    int crittersSpawned = 0;
    int spawnFrameCounter = 0;
    int spawnInterval = 120;

};


#endif
*/
//------------------------------------ New Implementation -----------------------------------------------------------------------------------------
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

//Factory class
class CritterFactory
{
public:
    static CritterLogic* createCritter(CritterType type, MapLogic& mapLogic, int level);
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