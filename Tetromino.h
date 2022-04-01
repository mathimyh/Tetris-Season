#pragma once
#include "Graph.h"




enum class TetrominoType{J, L, T, S, Z, O, I, NONE};

const map<TetrominoType, int> colorMap {
    {TetrominoType::J, 4},
    {TetrominoType::I, 6},
    {TetrominoType::L, 92},
    {TetrominoType::S, 2},
    {TetrominoType::O, 3},
    {TetrominoType::Z, 1},
    {TetrominoType::T, 13}
};

class Tetromino {
public:
    static constexpr int blockSize {30};

    Tetromino();
    
    Tetromino(Point startingPoint, TetrominoType tetType);
    
    void rotateClockwise();
    void rotateCounterClockwise();

    void moveDown();
    void moveLeft();
    void moveRight();
    void moveUp();

    bool blockExist(int row, int column);

    TetrominoType getBlock(int row, int column) { return blockMatrix[row][column]; }
    int getMatrixSize() { return matrixSize; }
    Point getPosition() { return topLeftCorner; }
    int getColor() { return tetrominoColor; }
    TetrominoType getType();
private: 
    int tetrominoColor;
    int matrixSize;
    Point topLeftCorner;
    vector<vector<TetrominoType>> blockMatrix;
};  
