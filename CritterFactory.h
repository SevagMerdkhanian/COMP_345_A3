#pragma once
#ifndef CRITTER_FACTORY_H
#define CRITTER_FACTORY_H

#include "critterLogic.h"


//Factory class
class CritterFactory
{
public:
    static CritterLogic* createCritter(CritterType type, MapLogic& mapLogic, int level);
};

#endif