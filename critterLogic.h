#pragma once
#ifndef CRITTER_LOGIC_H
#define CRITTER_LOGIC_H

#include <vector>
#include <cstdlib>
#include <ctime>
#include <iostream>
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