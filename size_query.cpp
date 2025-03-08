#include "size_query.h"
#include <string>

PanelInput::PanelInput() {
    widthInput[0] = '\0';
    heightInput[0] = '\0';
    widthIndex = 0;
    heightIndex = 0;
    widthSelected = true;
    heightSelected = false;
    inputComplete = false;
}

void PanelInput::Update() {
    if (WindowShouldClose()) {
        exit(0); // Exit the entire program if the window close button is pressed
    }
    int key = GetCharPressed();
    while (key > 0) {
        if (widthSelected && widthIndex < 9) {
            if ((key >= 48) && (key <= 57)) {
                widthInput[widthIndex++] = (char)key;
                widthInput[widthIndex] = '\0';
            }
        }
        else if (heightSelected && heightIndex < 9) {
            if ((key >= 48) && (key <= 57)) {
                heightInput[heightIndex++] = (char)key;
                heightInput[heightIndex] = '\0';
            }
        }
        key = GetCharPressed();
    }

    if (IsKeyPressed(KEY_BACKSPACE)) {
        if (widthSelected && widthIndex > 0) {
            widthInput[--widthIndex] = '\0';
        }
        else if (heightSelected && heightIndex > 0) {
            heightInput[--heightIndex] = '\0';
        }
    }

    if (IsKeyPressed(KEY_TAB)) {
        widthSelected = !widthSelected;
        heightSelected = !heightSelected;
    }

    if (IsKeyPressed(KEY_ENTER)) {
        if (GetWidth() > 0 && GetHeight() > 0) {
            CloseWindow(); // Close the window when input is complete
        }
    }
}

void PanelInput::Draw() {
    DrawText("Enter Width and Height:", 50, 30, 20, DARKGRAY);
    DrawText("Press TAB to switch, ENTER to submit", 50, 50, 20, DARKGRAY);
    DrawRectangle(50, 70, 100, 30, widthSelected ? LIGHTGRAY : GRAY);
    DrawText(widthInput, 60, 78, 20, BLACK);
    DrawRectangle(50, 120, 100, 30, heightSelected ? LIGHTGRAY : GRAY);
    DrawText(heightInput, 60, 128, 20, BLACK);
    DrawRectangle(50, 180, 100, 40, inputComplete ? GREEN : DARKGRAY);
    DrawText("Submit", 70, 190, 20, WHITE);
}

bool PanelInput::IsInputComplete() const {
    return inputComplete;
}

int PanelInput::GetWidth() const {
    return widthIndex > 0 ? std::stoi(widthInput) : 0;
}

int PanelInput::GetHeight() const {
    return heightIndex > 0 ? std::stoi(heightInput) : 0;
}
