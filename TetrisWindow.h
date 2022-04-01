#pragma once
#include "AnimationWindow.h"
#include "Tetromino.h"
#include "GUI.h"
#include <atomic>
#include "windows.h"

enum class GameState{Playing, Lost, Paused};

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

};
