#include "critterLogic.h"

class TowerTargetingStrategy {
public:
    virtual CritterLogic* GetTargetCritter(std::vector<Critter*>& critters, int minDistance) = 0;
    virtual ~TowerTargetingStrategy() = default;
};
