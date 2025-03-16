#include "mapUi.h"
#include "mapLogic.h"

#include "raylib.h"
#include <sstream>
#include <vector>
#include <stack>
#include <tuple>


MapUI::MapUI(MapLogic& mapLogic): mapLogic(mapLogic), observable(&mapLogic), cellSize(40), selectedTile(PATH), validationMessage("") {
    observable->Attach(this); 
}

void MapUI::initUI()
{
    // Initialize raylib window
    InitWindow(1024, 768, "Tower Defense - Map Editor");
    SetTargetFPS(60);
}

MapUI::~MapUI() {
    if (observable) {
        observable->Detach(this); // Remove observer before destruction
    }
};
void MapUI::Update() {
    drawUI();
    updateUI();
};

void MapUI::updateUI()
{
    // Switch selected tile based on key press
    if (IsKeyPressed(KEY_ONE))
        selectedTile = PATH;
    if (IsKeyPressed(KEY_TWO))
        selectedTile = SCENERY;
    if (IsKeyPressed(KEY_THREE))
        selectedTile = ENTRY;
    if (IsKeyPressed(KEY_FOUR))
        selectedTile = EXIT;

    // Handle user input to place selected tiles
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) || IsMouseButtonPressed(MOUSE_RIGHT_BUTTON))
    {
        int x = GetMouseX() / cellSize;
        int y = GetMouseY() / cellSize;

        if (x < mapLogic.getWidth() && y < mapLogic.getHeight())
        {
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
            {
                mapLogic.setCell(x, y, selectedTile);
                if (selectedTile == ENTRY)
                {
                    mapLogic.setEntry(x, y);
                }
                else if (selectedTile == EXIT)
                {
                    mapLogic.setExit(x, y);
                }
            }
        }
    }

    // Validate map when the button is clicked
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
    {

        // Validate Map Button
        int buttonWidth = 160;
        int buttonHeight = 40;
        int buttonX = GetScreenWidth() - buttonWidth - 10; // right-align the button
        int buttonY = 200;                                 // fixed y-coordinate for the button

        // Check if the "Validate Map" button is clicked
        if (CheckCollisionPointRec(Vector2{(float)GetMouseX(), (float)GetMouseY()},
                                   Rectangle{(float)buttonX, (float)buttonY, (float)buttonWidth, (float)buttonHeight}))
        {
            if (validateMap())
            {
                validationMessage = "MAP VALID";
                CloseWindow();
            }
        }
    }
}

bool MapUI::validateMap()
{
    // Check for exactly one entry and one exit tile
    int entryCount = 0, exitCount = 0;

    for (int y = 0; y < mapLogic.getHeight(); ++y)
    {
        for (int x = 0; x < mapLogic.getWidth(); ++x)
        {
            CellType type = mapLogic.getCell(x, y).type;
            if (type == ENTRY)
                entryCount++;
            if (type == EXIT)
                exitCount++;
        }
    }

    if (entryCount != 1)
    {
        validationMessage = "Invalid: There must be exactly one entry tile.";
        return false;
    }

    if (exitCount != 1)
    {
        validationMessage = "Invalid: There must be exactly one exit tile.";
        return false;
    }

    // Find the entry point
    int startX, startY;
    bool foundEntry = false;

    for (int y = 0; y < mapLogic.getHeight(); ++y)
    {
        for (int x = 0; x < mapLogic.getWidth(); ++x)
        {
            if (mapLogic.getCell(x, y).type == ENTRY)
            {
                startX = x;
                startY = y;
                foundEntry = true;
                break;
            }
        }
        if (foundEntry)
            break;
    }

    // Create a visited grid for path tracking
    std::vector<std::vector<bool>> visited(mapLogic.getHeight(), std::vector<bool>(mapLogic.getWidth(), false));
    std::stack<std::pair<int, int>> stack;
    stack.push({ startX, startY });
    visited[startY][startX] = true;

    // Directions to move on the map (Right, Down, Left, Up)
    std::vector<std::pair<int, int>> directions = {
        {1, 0},  // Right
        {0, 1},  // Down
        {-1, 0}, // Left
        {0, -1}  // Up
    };

    // DFS or BFS to check connectivity from entry point
    while (!stack.empty())
    {
        int x, y;
        std::tie(x, y) = stack.top();
        stack.pop();

        // Check all 4 directions
        for (const auto& direction : directions)
        {
            int dx = direction.first;
            int dy = direction.second;

            int nx = x + dx, ny = y + dy;
            if (nx >= 0 && nx < mapLogic.getWidth() && ny >= 0 && ny < mapLogic.getHeight())
            {
                // Check if the tile is walkable and hasn't been visited
                if (!visited[ny][nx] && (mapLogic.getCell(nx, ny).type == PATH || mapLogic.getCell(nx, ny).type == EXIT))
                {
                    visited[ny][nx] = true;
                    stack.push({ nx, ny });
                }
            }
        }
    }

    // Now check if the exit is reachable
    bool exitReachable = false;
    for (int y = 0; y < mapLogic.getHeight(); ++y)
    {
        for (int x = 0; x < mapLogic.getWidth(); ++x)
        {
            if (mapLogic.getCell(x, y).type == EXIT && visited[y][x])
            {
                exitReachable = true;
                break;
            }
        }
        if (exitReachable)
            break;
    }

    if (!exitReachable)
    {
        validationMessage = "Invalid: Path is not connected between entry and exit.";
        return false;
    }

    // Now let's check for dead-ends that could block the critter's path
    std::vector<std::vector<bool>> deadEndVisited(mapLogic.getHeight(), std::vector<bool>(mapLogic.getWidth(), false));
    bool deadEndFound = false;

    // Start from the entry point again and make sure critter doesn't get stuck in a dead-end loop
    stack.push({ startX, startY });
    deadEndVisited[startY][startX] = true;

    while (!stack.empty())
    {
        int x, y;
        std::tie(x, y) = stack.top();
        stack.pop();

        // Check all 4 directions for potential dead-ends
        bool hasValidNeighbor = false;
        for (const auto& direction : directions)
        {
            int dx = direction.first;
            int dy = direction.second;

            int nx = x + dx, ny = y + dy;
            if (nx >= 0 && nx < mapLogic.getWidth() && ny >= 0 && ny < mapLogic.getHeight())
            {
                // We only care about walkable tiles that haven't been visited yet
                if (!deadEndVisited[ny][nx] && (mapLogic.getCell(nx, ny).type == PATH || mapLogic.getCell(nx, ny).type == EXIT))
                {
                    // If we find a valid neighbor, mark it as visited and continue exploring
                    hasValidNeighbor = true;
                    deadEndVisited[ny][nx] = true;
                    stack.push({ nx, ny });
                }
            }
        }

        // If a tile has no valid neighbor to proceed to, it's a dead end
        if (!hasValidNeighbor && (mapLogic.getCell(x, y).type != EXIT)) // Only consider dead-ends that are not the exit
        {
            deadEndFound = true;
            break;  // We found a dead-end, no need to continue
        }
    }

    if (deadEndFound)
    {
        validationMessage = "Invalid: There can only be 1 path.";
        return false;
    }

    // If no issues found, return true
    return true;
}

void MapUI::dfs(int x, int y, std::vector<std::vector<bool>> &visited)
{
    // Boundary checks
    if (x < 0 || x >= mapLogic.getWidth() || y < 0 || y >= mapLogic.getHeight() || visited[y][x])
    {
        return;
    }

    if (mapLogic.getCell(x, y).type != PATH && mapLogic.getCell(x, y).type != ENTRY && mapLogic.getCell(x, y).type != EXIT)
    {
        return; // Only traverse paths and entry points and exit
    }

    visited[y][x] = true;

    // Move in all 4 directions (up, down, left, right)
    dfs(x + 1, y, visited);
    dfs(x - 1, y, visited);
    dfs(x, y + 1, visited);
    dfs(x, y - 1, visited);
}

/**
 * Draw a right-aligned text block.
 *
 * lines: an array of null-terminated strings.
 * colors: an array of Colors corresponding to each line.
 * numLines: number of lines in the array.
 * startY: the y-coordinate for the first line.
 * spacing: vertical space between lines (in pixels).
 * fontSize: the font size to use.
 * margin: distance (in pixels) from the right edge.
 */
void DrawRightAlignedTextBlock(const char *lines[], const Color colors[], int numLines, int startY, int spacing, int fontSize, int margin)
{
    int screenWidth = GetScreenWidth();
    int maxWidth = 0;

    // Find the maximum width among the lines.
    for (int i = 0; i < numLines; i++)
    {
        int w = MeasureText(lines[i], fontSize);
        if (w > maxWidth)
        {
            maxWidth = w;
        }
    }

    // Compute the starting x-coordinate so the block is right aligned.
    int startX = screenWidth - maxWidth - margin;

    // Draw each line with the same starting x-coordinate.
    for (int i = 0; i < numLines; i++)
    {
        DrawText(lines[i], startX, startY + i * spacing, fontSize, colors[i]);
    }
}

void MapUI::drawUI()
{
    BeginDrawing();
    ClearBackground(RAYWHITE);

    // Draw the grid
    for (int y = 0; y < mapLogic.getHeight(); ++y)
    {
        for (int x = 0; x < mapLogic.getWidth(); ++x)
        {
            Color color;
            CellType cellType = mapLogic.getCell(x, y).type;

            if (cellType == PATH)
            {
                color = BLUE;
            }
            else if (cellType == SCENERY)
            {
                color = LIGHTGRAY;
            }
            else if (cellType == ENTRY)
            {
                color = GREEN;
            }
            else if (cellType == EXIT)
            {
                color = RED;
            }

            DrawRectangle(x * cellSize, y * cellSize, cellSize, cellSize, color);
            DrawRectangleLines(x * cellSize, y * cellSize, cellSize, cellSize, DARKGRAY);
        }
    }
    // Display the selected tile
    std::ostringstream tileText;
    tileText << "Current Tile: " << tileTypeToString(selectedTile);
    DrawText(tileText.str().c_str(), 10, 10, 20, DARKGRAY);

    const char* legendLines[] = {
        "Legend:",
        "1 - Path (Blue)",
        "2 - Scenery (Gray)",
        "3 - Entry (Green)",
        "4 - Exit (Red)" };

    Color legendColors[] = {
        DARKGRAY,
        BLUE,
        LIGHTGRAY,
        GREEN,
        RED };

    int numLines = 5;
    int fontSize = 20;
    int spacing = 30; // 30 pixels between lines.
    int margin = 10;  // 10 pixels from the right edge.
    int startY = 10;  // Start drawing at y = 10.

    // Validate Map Button
    int buttonWidth = 160;
    int buttonHeight = 40;

    // Move the button even more to the left by reducing buttonX further
    int buttonX = GetScreenWidth() - buttonWidth - 3 * margin; // Reduced margin to shift further left
    int buttonY = 200;                                     // fixed y-coordinate for the button

    // Draw validation message below the button
    int messageY = buttonY + buttonHeight + margin;  // Place the validation message below the button
    int messageX = buttonX; // Align the message with the button

    DrawRightAlignedTextBlock(legendLines, legendColors, numLines, startY, spacing, fontSize, margin);

    DrawRectangle(buttonX, buttonY, buttonWidth, buttonHeight, DARKGRAY);
    DrawText("Validate Map", buttonX + 15, buttonY + 10, 20, RAYWHITE);

    // Display validation result message below the button, aligned with button
    wrapText(validationMessage, messageX, messageY, buttonWidth, 20);

    EndDrawing();


}


// Make sure to include your CritterManager header as well.
#include "critterLogic.h"  // For CritterManager

void MapUI::drawUIWithTowersCustom(TowerManager& towerManager, TowerUIManager& towerUIManager, CritterManager& critterManager) {
    // --- Persistent State Variables ---
    static int selectedTowerIndex = -1;              // Currently selected tower index (-1 if none)
    static TowerType currentTowerType = TowerType::BASIC;  // Currently selected tower type (default BASIC)
    static int playerMoney = 500;                      // Player's starting money

    // --- Update Tower Type Based on Keyboard Input ---
    if (IsKeyPressed(KEY_ONE)) { currentTowerType = TowerType::BASIC; }
    if (IsKeyPressed(KEY_TWO)) { currentTowerType = TowerType::SPLASH; }
    if (IsKeyPressed(KEY_THREE)) { currentTowerType = TowerType::SLOW; }
    if (IsKeyPressed(KEY_FOUR)) { currentTowerType = TowerType::SNIPER; }

    BeginDrawing();
    ClearBackground(RAYWHITE);

    // --- Draw the Map Grid ---
    for (int y = 0; y < mapLogic.getHeight(); ++y) {
        for (int x = 0; x < mapLogic.getWidth(); ++x) {
            Color cellColor;
            CellType cellType = mapLogic.getCell(x, y).type;
            switch (cellType) {
            case PATH:     cellColor = BLUE; break;
            case SCENERY:  cellColor = LIGHTGRAY; break;
            case ENTRY:    cellColor = GREEN; break;
            case EXIT:     cellColor = RED; break;
            default:       cellColor = DARKGRAY; break;
            }
            DrawRectangle(x * cellSize, y * cellSize, cellSize, cellSize, cellColor);
            DrawRectangleLines(x * cellSize, y * cellSize, cellSize, cellSize, DARKGRAY);
        }
    }

    // --- Handle Tower Placement & Selection ---
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        Vector2 mousePos = GetMousePosition();
        if (mousePos.x < mapLogic.getWidth() * cellSize &&
            mousePos.y < mapLogic.getHeight() * cellSize) {

            int gridX = mousePos.x / cellSize;
            int gridY = mousePos.y / cellSize;
            const std::vector<Tower*>& towers = towerManager.getTowers();
            bool towerClicked = false;
            // Check if a tower was clicked.
            for (size_t i = 0; i < towers.size(); i++) {
                Vector2 towerPos = towers[i]->getPosition();
                float dx = mousePos.x - towerPos.x;
                float dy = mousePos.y - towerPos.y;
                if (sqrt(dx * dx + dy * dy) < (cellSize / 3.0f)) {
                    selectedTowerIndex = static_cast<int>(i);
                    towerClicked = true;
                    break;
                }
            }
            // If no tower was clicked, try to place a new tower.
            if (!towerClicked) {
                if (mapLogic.getCell(gridX, gridY).type == SCENERY) {
                    bool cellOccupied = false;
                    for (Tower* t : towers) {
                        int tCellX = static_cast<int>(t->getPosition().x) / cellSize;
                        int tCellY = static_cast<int>(t->getPosition().y) / cellSize;
                        if (tCellX == gridX && tCellY == gridY) {
                            cellOccupied = true;
                            break;
                        }
                    }
                    if (!cellOccupied) {
                        Vector2 towerPos = { gridX * cellSize + cellSize / 2.0f,
                                             gridY * cellSize + cellSize / 2.0f };
                        Tower* newTower = nullptr;
                        switch (currentTowerType) {
                        case TowerType::BASIC: newTower = new BasicTower(); break;
                        case TowerType::SPLASH: newTower = new SplashTower(); break;
                        case TowerType::SLOW: newTower = new SlowTower(); break;
                        case TowerType::SNIPER: newTower = new SniperTower(); break;
                        default: newTower = new BasicTower(); break;
                        }
                        if (playerMoney >= newTower->getCost()) {
                            playerMoney -= newTower->getCost();
                            newTower->setPosition(towerPos);
                            towerManager.addTower(newTower);
                            selectedTowerIndex = -1;
                        }
                        else {
                            delete newTower;
                        }
                    }
                }
            }
        }
    }

    // --- Handle Tower Selling ---
    if (IsKeyPressed(KEY_D) && selectedTowerIndex != -1) {
        const std::vector<Tower*>& towers = towerManager.getTowers();
        if (selectedTowerIndex < static_cast<int>(towers.size())) {
            int sellValue = towers[selectedTowerIndex]->sell();
            playerMoney += sellValue;
            towerManager.removeTower(selectedTowerIndex);
            selectedTowerIndex = -1;
        }
    }

    // --- Handle Tower Upgrading ---
    if (IsKeyPressed(KEY_L) && selectedTowerIndex != -1) {
        const std::vector<Tower*>& towers = towerManager.getTowers();
        if (selectedTowerIndex < static_cast<int>(towers.size())) {
            int upgradeCost = towers[selectedTowerIndex]->getCost();
            if (playerMoney >= upgradeCost) {
                towerManager.upgradeTower(selectedTowerIndex);
                playerMoney -= upgradeCost;
            }
        }
    }

    // --- Update Tower Shooting with Critter Detection ---
    // Now call the overloaded updateTowers with the CritterManager reference.
    towerManager.Notify();

    // --- Draw Towers ---
    towerUIManager.drawTowers(towerManager, cellSize);

    // --- Highlight Selected Tower ---
    if (selectedTowerIndex != -1) {
        const std::vector<Tower*>& towers = towerManager.getTowers();
        if (selectedTowerIndex < static_cast<int>(towers.size())) {
            Vector2 pos = towers[selectedTowerIndex]->getPosition();
            DrawCircleLines(pos.x, pos.y, (cellSize / 3.0f) + 2, RED);
        }
    }

    // --- Draw UI Elements: Money & Level ---
    std::string moneyText = "Money: " + std::to_string(playerMoney);
    DrawText(moneyText.c_str(), 10, 10, 20, DARKGRAY);


    // --- Draw the Legend (Right-Aligned) ---
    const char* legendLines[] = {
        "Legend:",
        "1 - Basic (Purple)",
        "2 - Splash (Orange)",
        "3 - Slow (Blue)",
        "L - Upgrade",
        "D - Sell"
    };
    Color legendColors[] = {
        DARKGREEN,
        DARKPURPLE,
        ORANGE,
        BLUE,
        DARKBLUE,
        DARKBLUE
        
    };
    int numLines = 6;
    int fontSize = 20;
    int spacing = 30;
    int margin = 10;
    int startY = 10;
    DrawRightAlignedTextBlock(legendLines, legendColors, numLines, startY, spacing, fontSize, margin);

}






// Critter Drawing
void MapUI::drawCritters(CritterManager &manager)
{
    int wave = 1;
    int numCritters = 5;

    std::string levelText = "Wave: " + std::to_string(wave);
    DrawText(levelText.c_str(), 10, 40, 20, DARKGRAY);
    
    // Generate critters for the wave if none exist
    // if (manager.getCritters().empty()) {
    //    std::vector<CritterLogic> generatedCritters = CritterLogic::generateCritter(wave, numCritters, mapLogic);
    //    for (auto& critter : generatedCritters) {
    //        manager.addCritter(new CritterLogic(mapLogic, critter.getType(), wave));
    //    }
    //}

    if (manager.getCritters().empty() && manager.getCrittersSpawned() >= numCritters)
    {
        manager.resetWave();
    }

    manager.update(mapLogic, wave, numCritters); // Spawns critters at regular intervals

    auto &critters = manager.getCritters();
    for (auto it = critters.begin(); it != critters.end();)
    {
        
        //(*it)->move(mapLogic);  // Moves only when frameCounter allows

        // Check if critter reached the exit
        if (mapLogic.getCell((*it)->getX(), (*it)->getY()).type == EXIT)
        {
            auto critterIt = std::find(critters.begin(), critters.end(), *it);
            if (mapLogic.getCell((*it)->getX(), (*it)->getY()).type == EXIT) 
            {
                CritterLogic* critterToRemove = *it;
                it = critters.erase(it);
                manager.removeCritter(critterToRemove);
                continue;
            }
            // else {
            //     ++it;  // Move to the next element if not found
            // }
        }

        Vector2 position = {((float)(*it)->getX() +0.5)* cellSize, ((float)(*it)->getY() +0.5) * cellSize};
        Color critterColor;

        switch ((*it)->getType())
        {
        case CritterType::TANKY:
            critterColor = ORANGE;
            break;
        case CritterType::SPEEDY:
            critterColor = GREEN;
            break;
        case CritterType::STRONG:
            critterColor = RED;
            break;
        case CritterType::BALANCED:
        default:
            critterColor = BLACK;
            break;
        }

        Vector2 v1 = { position.x, position.y - 10 - (10.0f / 3.0f) }; // position.y - 40/3
        Vector2 v2 = { position.x - 10, position.y + 10 - (10.0f / 3.0f) }; // position.y + 20/3
        Vector2 v3 = { position.x + 10, position.y + 10 - (10.0f / 3.0f) }; // position.y + 20/3

        DrawTriangle(v1, v2, v3, critterColor);

        // Draw Health Bar
        float healthBarWidth = 20.0f; // Max width of health bar
        float healthPercentage = (float)(*it)->getHealth() / (*it)->getMaxHealth();
        float barWidth = healthBarWidth * healthPercentage; // Scale based on health

        Vector2 healthBarPos = {position.x - (healthBarWidth / 2), position.y - 10}; // Above critter

        DrawRectangle(healthBarPos.x, healthBarPos.y, barWidth, 4, GREEN);            // Health amount
        DrawRectangleLines(healthBarPos.x, healthBarPos.y, healthBarWidth, 4, BLACK); // Border

        ++it;
    }

    EndDrawing();
}

const char *MapUI::tileTypeToString(CellType type)
{
    switch (type)
    {
    case PATH:
        return "Path";
    case SCENERY:
        return "Scenery";
    case ENTRY:
        return "Entry";
    case EXIT:
        return "Exit";
    default:
        return "Unknown";
    }
}

void MapUI::wrapText(const std::string &text, int x, int y, int maxWidth, int fontSize)
{
    int currentX = x;
    int currentY = y;
    std::string currentLine = "";

    for (char c : text)
    {
        currentLine += c;

        if (MeasureText(currentLine.c_str(), fontSize) > maxWidth)
        {
            DrawText(currentLine.c_str(), currentX, currentY, fontSize, DARKGRAY);
            currentY += fontSize + 2;
            currentLine = c; // Start new line with the current character
        }
    }

    // Draw the last line
    if (!currentLine.empty())
    {
        DrawText(currentLine.c_str(), currentX, currentY, fontSize, DARKGRAY);
    }
}
