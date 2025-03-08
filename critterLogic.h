#pragma once
#ifndef CRITTER_LOGIC_H
#define CRITTER_LOGIC_H

#include <vector>
#include "mapLogic.h"
#include <cstdlib>
#include <ctime>
#include <iostream>
#include "raylib.h"

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

class CritterLogic
{
public:
    CritterLogic(MapLogic &mapLogic, CritterType critterType, int level);

    CritterType getType();
    int getHealth();
    int getMaxHealth() const;
    int getX();
    int getY();

    void minusHealth(int minusHealth);
    void move(MapLogic &mapLogic);
    bool isDead() const;
    static std::vector<CritterLogic> generateCritter(int waveNum, int numCritters, MapLogic &mapLogic);
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

class CritterManager
{
public:
    CritterManager();
    ~CritterManager();

    void addCritter(CritterLogic *critter);
    void removeCritter(std::vector<CritterLogic *>::iterator it);

    int getCrittersSpawned();

    void update(MapLogic &mapLogic, int wave, int numCritters);
    void resetWave();

    std::vector<CritterLogic *> &getCritters();

private:
    std::vector<CritterLogic *> critters;
    int crittersSpawned = 0;
    int spawnFrameCounter = 0;
    int spawnInterval = 120;
};

#endif