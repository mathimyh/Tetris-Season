#pragma once
#include "AnimationWindow.h"
#include "Tetromino.h"
#include "GUI.h"
#include <atomic>

enum class GameState{Playing, Lost, Paused};

class TetrisWindow : public AnimationWindow {

public:
    TetrisWindow(int x, int y, int width,  int height, const string& title);
    void run();
    
    void generateRandomTetromino();
    void drawCurrentTetromino();
    void moveTetrominoDown();
    void handleInput();
    void hardDrop();

    void fastenTetromino();
    void drawGridMatrix();

    bool hasCrashed();
    void removeFullRows();

private:
    // Som i minesweeper er height og width antall blokker i hver retning
    const int height;
    const int width;
    const Point startPosition;

    
    vector<vector<TetrominoType>> gridMatrix;
    Tetromino currentTetromino;

    GameState gamestate = GameState::Playing;

    Fl_Output lossFeedback;
};
