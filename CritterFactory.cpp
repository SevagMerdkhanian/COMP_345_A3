#include "CritterFactory.h"

//Critter Factory
CritterLogic* CritterFactory::createCritter(CritterType type, MapLogic& mapLogic, int level) {
    switch (type) {
    case SPEEDY:
        return new SpeedyCritter(mapLogic, level);
    case TANKY:
        return new TankyCritter(mapLogic, level);
    case STRONG:
        return new StrongCritter(mapLogic, level);
    case BALANCED:
        return new BasicCritter(mapLogic, level);
    default:
        return nullptr;
    }
}