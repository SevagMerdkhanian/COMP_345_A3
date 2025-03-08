#include "raylib.h"
#include "raymath.h"
#include <vector>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include "mapLogic.h"
#include "mapUi.h"
#include "size_query.h"
#include "towerUI.h"
#include "critterLogic.h"

int main(void)
{
    std::srand(std::time(0));
    //testMain(); 
    //towerMain();
    TowerManager towerManager;
    TowerUIManager towerUIManager;
    CritterManager critterManager;
    const int screenWidth = 500;
    const int screenHeight = 300;
    InitWindow(screenWidth, screenHeight, "Panel Input Example");
    SetTargetFPS(60);

    PanelInput panel;

    while (!WindowShouldClose()) {
        panel.Draw();
        EndDrawing();
        panel.Update();

        BeginDrawing();
        ClearBackground(RAYWHITE);
       
    }
    int width = panel.GetWidth();
    int height = panel.GetHeight();
   /* const int width = 10;
    const int height = 10;*/

    // Create map logic and UI instances
    MapLogic mapLogic(width, height);
    MapUI mapUI(mapLogic);

    // Initialize UI
    mapUI.initUI();
    int mid = height / 2;
    for (int x = 0; x < width; x++) {
        mapLogic.setCell(x, mid, PATH);
    }
    mapLogic.setEntry(0, mid);
    mapLogic.setExit(width - 1, mid);
    // Main game loop
    while (!WindowShouldClose()) {
        

        // Draw the UI
        mapUI.drawUI();
        // Update the UI
        mapUI.updateUI();
    }
    // Initialize UI
    mapUI.initUI();



    while (!WindowShouldClose()) {
        
        // Draw the UI
        mapUI.drawUIWithTowersCustom(towerManager, towerUIManager, critterManager);

        mapUI.drawCritters(critterManager);
        
    }


    // Close the window and OpenGL context
    CloseWindow();

    return 0;
}
