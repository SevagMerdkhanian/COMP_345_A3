#ifndef PANEL_INPUT_H
#define PANEL_INPUT_H

#include "raylib.h"

class PanelInput {
public:
    PanelInput();
    void Update();
    void Draw();
    bool IsInputComplete() const;
    int GetWidth() const;
    int GetHeight() const;

private:
    char widthInput[10];
    char heightInput[10];
    int widthIndex;
    int heightIndex;
    bool widthSelected;
    bool heightSelected;
    bool inputComplete;
};

#endif
#pragma once
