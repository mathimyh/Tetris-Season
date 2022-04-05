#include "Tetromino.h"
    

const map<TetrominoType, vector<vector<int>>> initialMatrixMap {

    {
    TetrominoType::J,
        {
        {0, 0, 0},
        {1, 1, 1},
        {0, 0, 1}
        }
    },
    { 
    TetrominoType::L, 
        {
        {0, 0, 0},
        {1, 1, 1},
        {1, 0, 0}
        }
    },
    {
    TetrominoType::I, 
        {
        //Denne er representert som en 6x6 matrise for å få en bedre rotasjonsfunksjonalitet
        {0, 0, 0, 0},
        {0, 0, 0, 0},
        {1, 1, 1, 1},
        {0, 0, 0, 0}
        }
    },
    {
    TetrominoType::T,
        {
        {0, 0, 0},
        {1, 1, 1},
        {0, 1, 0}
        }
    },
    {
    TetrominoType::O,
        {
        {1, 1},
        {1, 1},
        }
    },
    {
    TetrominoType::S,
        {
        {0, 1, 1},
        {1, 1, 0},
        {0, 0, 0}
        }
    },
    {
    TetrominoType::Z,
        {
        {1, 1, 0},
        {0, 1, 1},
        {0, 0, 0}
        }
    }

};


Tetromino::Tetromino() 
{
    topLeftCorner = {0,0};
    matrixSize = 0;
}


Tetromino::Tetromino(Point startingPoint, TetrominoType tetType) : 
topLeftCorner{startingPoint}
{
    // Initialiserer matrixSize
    auto it = initialMatrixMap.find(tetType);
    vector<vector<int>> temp = it->second;
    matrixSize = temp.size();
    
    // Itererer gjennom matrisen og lager blockMatrix utifra den
    for (auto & row : temp) {
        vector<TetrominoType> newRow;
        for (auto & column : row) {
            if (column == 1) {
                newRow.push_back(tetType);
            } else {
                newRow.push_back(TetrominoType::NONE);
            }
        }

        blockMatrix.push_back(newRow);
    }

    // Finner så fargen på tetrominoen
    auto that = colorMap.find(tetType);
    tetrominoColor = that->second;
}

void Tetromino::rotateCounterClockwise() {
    //Første rotasjon, med hensyn på diagonalen
    for(int row = 0; row < matrixSize; row++){
        for(int column = 0; column < row; column++){
            std::swap(blockMatrix[row][column], blockMatrix[column][row]);
        }
    }

    //Andre rotasjon, med hensyn på den midtre raden
    for(int row = 0; row < matrixSize/2; row++){
        for(int column = 0; column < matrixSize; column++){
            std::swap(blockMatrix[row][column], blockMatrix[matrixSize-row-1][column]);
            
        }
    }
}

void Tetromino::rotateClockwise() {
    //Første rotasjon, med hensyn på diagonalen
    for(int row = 0; row < matrixSize; row++){
        for(int column = 0; column < row; column++){
            std::swap(blockMatrix[row][column], blockMatrix[column][row]);
        }
    }
     
    //Andre rotasjon, med hensyn på den midtre kolonnen
    for(int row = 0; row < matrixSize; row++){
        for(int column = 0; column < matrixSize/2; column++){
            std::swap(blockMatrix[row][column], blockMatrix[row][matrixSize-column-1]);
        }
    }
}

void Tetromino::moveDown()
{
    topLeftCorner.y += blockSize; 
}

void Tetromino::moveRight()
{
    topLeftCorner.x += blockSize;
}

void Tetromino::moveLeft()
{
    topLeftCorner.x -= blockSize;
}

void Tetromino::moveUp()
{
    topLeftCorner.y -= blockSize;
}

bool Tetromino::blockExist(int row, int column)
{
    if (row < 0 || row >= matrixSize || column < 0 || column >= matrixSize ) {
        return false;
    }

    return blockMatrix[row][column] != TetrominoType::NONE;
}

TetrominoType Tetromino::getType()
{
    for (auto & row : blockMatrix) {
        for (auto & column : row) {
            if (column != TetrominoType::NONE){
                return column;
            }
        }
    }
}
