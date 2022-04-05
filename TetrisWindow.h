#pragma once
#include "AnimationWindow.h"
#include "Tetromino.h"
#include "GUI.h"
#include <atomic>
#include "../Øving 7/FreezingFace.h"
#include <iostream>
#include <fstream>

struct Points
{
    string name;
    int points;
};

enum class GameState{Playing, Lost, Paused, WritingName};

class TetrisWindow : public AnimationWindow {

public:
    TetrisWindow(int x, int y, int width,  int height, const string& title);
    void run();
    
    void generateRandomTetromino();
    void drawCurrentTetromino();
    void drawShadowTetromino();
    void drawNextTetrominos();
    void drawSpareTetromino();
    void moveTetrominoDown();
    void handleInput();
    void hardDrop();

    void fastenTetromino();
    void drawGridMatrix();

    bool hasCrashed(Tetromino tet);
    void removeFullRows();

    void restartWindow();
    void saveName();
    void addNameToFile();



private:
    // Som i minesweeper er height og width antall blokker i hver retning
    const int height;
    const int width;
    const Point startPosition;

    int points = 0;

    vector<vector<TetrominoType>> gridMatrix;
    Tetromino currentTetromino;
    Tetromino shadowTetromino;
    Tetromino spareTetromino;

    GameState gamestate = GameState::Playing;

    vector<Tetromino> nextTetrominos;

    unsigned int framesPerTetronimoMove = 20;

    string name = "";
    bool removeQ = true;

};



