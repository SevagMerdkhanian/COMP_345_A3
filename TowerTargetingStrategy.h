#include "critterLogic.h"

class TowerTargetingStrategy {
public:
    virtual CritterLogic* GetTargetCritter(std::vector<CritterLogic*>& critters, int cellSize, int towerRangePixels, Vector2 towerPos) = 0;
    virtual ~TowerTargetingStrategy() = default;
};
