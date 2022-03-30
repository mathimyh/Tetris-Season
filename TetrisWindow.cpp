#include "TetrisWindow.h"


TetrisWindow::TetrisWindow(int x, int y, int width,  int height, const string& title) : 
lossFeedback(width*Tetromino::blockSize/4, 2*height*Tetromino::blockSize/5, width*Tetromino::blockSize/2, height*Tetromino::blockSize/5, ""),
AnimationWindow(x, y, width*Tetromino::blockSize + 2*Tetromino::blockSize, height*Tetromino::blockSize, title),
width{width}, height{height}, startPosition{Point{(Tetromino::blockSize*4), 0}}
{
    

    // Lager matrisen som dekker vinduet
    for (int i = 0; i < height; i++) {
        vector<TetrominoType> newRow;
        for (int j=0; j <width; j++) {
            newRow.push_back(TetrominoType::NONE);
        }
        gridMatrix.push_back(newRow);
    }
    
    // Lager den første Tetromino-en her
    generateRandomTetromino();
};


void TetrisWindow::run() {
    unsigned int framesSinceLastTetronimoMove = 0;
    const unsigned int framesPerTetronimoMove = 20;

    while(!should_close()) { 
        framesSinceLastTetronimoMove++;
        if(framesSinceLastTetronimoMove >= framesPerTetronimoMove) {
            framesSinceLastTetronimoMove = 0;
            /********************************************************/
            //Kall moveTetrominoDown() her
            moveTetrominoDown();
            /********************************************************/
        }
        handleInput();
        
        
        
        if (gamestate == GameState::Lost) {
            add(lossFeedback);
            lossFeedback.value("      You have lost.");
        }

        else if (gamestate == GameState::Playing ) {
        /********************************************************/
        //Kall draw-funksjonene her
            // Tegner bakgrunnsfarge, litt kjedelig med grå så lager svart som i Tetris Battle
            draw_rectangle({0,0}, width*Tetromino::blockSize, height*Tetromino::blockSize, 0x0A233700);
            
            removeFullRows();
            drawCurrentTetromino();
            drawGridMatrix();
            
            // Tegner rutenett slik at det ser finere ut
            for (int i = 0; i < width; i++) {
                draw_line({i*Tetromino::blockSize, 0}, {i*Tetromino::blockSize, height*Tetromino::blockSize}, 0x343A4000);
            } for (int i = 0; i < height; i++) {
                draw_line({0, i*Tetromino::blockSize}, {width*Tetromino::blockSize, i*Tetromino::blockSize}, 0x343A4000);
            }
            // Tegner høyresida, som skal være en UI 
            draw_rectangle({width*Tetromino::blockSize, 0}, Tetromino::blockSize*2, height*Tetromino::blockSize, 0x1876C100);
        /********************************************************/

            
        } next_frame();
    }
}


void TetrisWindow::handleInput() {

    static bool lastZKeyState = false;
    static bool lastUpKeyState = false;
    static bool lastDownKeyState = false;
    static bool lastLeftKeyState = false;
    static bool lastRightKeyState = false;
    static bool lastQKeyState = false;
    static bool lastTabKeyState = false;

    bool currentZKeyState = is_key_down(KeyboardKey::Z);
    bool currentUpKeyState = is_key_down(KeyboardKey::UP);
    bool currentDownKeyState = is_key_down(KeyboardKey::DOWN);
    bool currentRightKeyState = is_key_down(KeyboardKey::RIGHT);
    bool currentLeftKeyState = is_key_down(KeyboardKey::LEFT);
    bool currentQKeyState = is_key_down(KeyboardKey::Q);
    bool currentTabKeyState = is_key_down(KeyboardKey::TAB);

    if(currentZKeyState && !lastZKeyState) {
        currentTetromino.rotateCounterClockwise();
        if (hasCrashed()) {
            currentTetromino.rotateClockwise();
        }
    }

    if(currentUpKeyState && !lastUpKeyState) {
        currentTetromino.rotateClockwise();
        if (hasCrashed()) {
            currentTetromino.rotateCounterClockwise();
        }
    }

    if(currentDownKeyState && !lastDownKeyState) {
        moveTetrominoDown();
    }

    if(currentLeftKeyState && !lastLeftKeyState) {
        currentTetromino.moveLeft();
        if (hasCrashed()) {
            currentTetromino.moveRight();
        }
        
    }

    if(currentRightKeyState && !lastRightKeyState) {
        currentTetromino.moveRight();
        if (hasCrashed()) {
            currentTetromino.moveLeft();
        }
    }

    if(currentQKeyState && !lastQKeyState) {
        fastenTetromino();
    }

    if(currentTabKeyState && !lastTabKeyState) {
        hardDrop();
    }

    lastZKeyState = currentZKeyState;
    lastUpKeyState = currentUpKeyState;
    lastRightKeyState = currentRightKeyState;
    lastLeftKeyState = currentLeftKeyState;
    //lastDownKeyState = currentDownKeyState;
    lastTabKeyState = currentTabKeyState;
}

void TetrisWindow::generateRandomTetromino()
{
    int temp = rand() % 7;
    currentTetromino = Tetromino(startPosition, TetrominoType{temp});
    if (hasCrashed()) { gamestate = GameState::Lost; }
}

void TetrisWindow::drawCurrentTetromino()
{
    // Går gjennom currentTetromino og tegner hver blokk 
    int size = currentTetromino.getMatrixSize();
    Point currentPos = currentTetromino.getPosition();
    int color = currentTetromino.getColor();

    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            if (currentTetromino.getBlock(i, j) != TetrominoType::NONE) {
                draw_rectangle({currentPos.x + (j*Tetromino::blockSize), currentPos.y + (i*Tetromino::blockSize)}, Tetromino::blockSize, Tetromino::blockSize, color);
            }
        }
    }
}

void TetrisWindow::moveTetrominoDown()
{
    currentTetromino.moveDown();
    if (hasCrashed()) {
        currentTetromino.moveUp();
        fastenTetromino();
        generateRandomTetromino();
    }

}

void TetrisWindow::fastenTetromino()
{
    int xCoord = currentTetromino.getPosition().x / Tetromino::blockSize;
    int yCoord = currentTetromino.getPosition().y / Tetromino::blockSize;
    int tetrSize = currentTetromino.getMatrixSize();
    for (int i = 0; i < tetrSize; i++) {
        for (int j = 0; j < tetrSize; j++) {
            if (currentTetromino.getBlock(i, j) != TetrominoType::NONE) {
                gridMatrix[yCoord + i][xCoord + j] = currentTetromino.getBlock(i,j);
            }
        }
    }
}

bool TetrisWindow::hasCrashed()
{

    // Sjekker hver blokk i currentTetromino, sjekker om den er utfor matrisen først, deretter om den har truffet en annen blokk;
    int xCoord = currentTetromino.getPosition().x / Tetromino::blockSize;
    int yCoord = currentTetromino.getPosition().y / Tetromino::blockSize;
    int tetrSize = currentTetromino.getMatrixSize();
    
    for (int i = 0; i < tetrSize; i++) {
        for (int j = 0; j < tetrSize; j++) {
            
            if (currentTetromino.getBlock(i,j) != TetrominoType::NONE) {  // Sjekker bare blokker som ikke er NONE
                // Først om den har gått utfor veggene
                if ( xCoord+j >= width || xCoord+j < 0) { return true; }
                // Så om den har truffet bunnen
                if ( yCoord+i >= height) { return true; }
                // Deretter om den treffer en annen blokk. Nå vil det (forhåpentligvis) ikke være noen indekser utfor matrisen
                if (currentTetromino.getBlock(i, j) != TetrominoType::NONE && gridMatrix[yCoord + i][xCoord + j] != TetrominoType::NONE) {
                    return true;
                }
            }
        }
    }

    return false;
}

void TetrisWindow::drawGridMatrix()
{
    int nRows = gridMatrix.size();
    int nColumns = gridMatrix[0].size();
    for (int i = 0; i < nRows; i++) {
        for (int j = 0; j < nColumns; j++) {
            if (gridMatrix[i][j] != TetrominoType::NONE) {
                auto that = colorMap.find(gridMatrix[i][j]);
                int color = that->second; 
                draw_rectangle({j*Tetromino::blockSize, i*Tetromino::blockSize}, Tetromino::blockSize, Tetromino::blockSize, color);
            }
        }
    }
}

void TetrisWindow::removeFullRows()
{
    
    int rowsToFall = 0;
    
    // Itererer gjennom gridMatrix og fjerner hver rad som er full
    int nRows = gridMatrix.size();
    int nColumns = gridMatrix[0].size();
    int finalRow = 0;
    for (int i = 0; i < nRows; i++) {
        bool fullRow = true;                    // Lager en bool som sjekker om raden er full
        for (int j = 0; j < nColumns; j++) {
            if (gridMatrix[i][j] == TetrominoType::NONE) {
                fullRow = false;
            } 
        }

        if (fullRow == true) {      // Dersom raden er full, gjør om alle blokk til NONE og legger til en i rader resten skal falle ned
            for (auto& column : gridMatrix[i]) {
                column = TetrominoType::NONE;
            }
            rowsToFall += 1;
            finalRow = i;
        }

    } 

    // FLytter så alle elementene i gridMatrix ned rowsToFall hakk
    for (int i = finalRow; i >= rowsToFall; i--) {
        for (int j = 0; j < nColumns; j++) {
            gridMatrix[i][j] = gridMatrix[i-rowsToFall][j];
        }
    } // De øverste radene må initialiseres påny
    for (int i = 0; i < rowsToFall; i++) {
        for (int j = 0; j < nColumns; j++) {
            gridMatrix[i][j] = TetrominoType::NONE;
        }
    }
}

void TetrisWindow::hardDrop()
{
    while (!hasCrashed()) {
        currentTetromino.moveDown();
    }
    currentTetromino.moveUp();

}