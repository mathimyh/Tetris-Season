#include "TetrisWindow.h"


TetrisWindow::TetrisWindow(int x, int y, int width,  int height, const string& title) : 
    AnimationWindow(x, y, width*Tetromino::blockSize + 3*Tetromino::blockSize, height*Tetromino::blockSize, title),
    width{width}, height{height}, startPosition{Point{(Tetromino::blockSize*4), 0}}
{
    //PlaySound(TEXT("Game Boy Tetris Music B.wav"), NULL, SND_FILENAME);

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

    unsigned int speedUp = 0;

    // Gjør at farten øker etterhvert slik at det blir vanskeligere og vanskeligere
    int c = 0;
    while(!should_close()) { 
        if (speedUp % 1500 == 0 && framesPerTetronimoMove >= 5) {
            framesPerTetronimoMove -= 1;
        }
        
        framesSinceLastTetronimoMove++;
        if(framesSinceLastTetronimoMove >= framesPerTetronimoMove) {
            framesSinceLastTetronimoMove = 0;
            if (gamestate == GameState::Playing){
                moveTetrominoDown(); // Tetrominoen flyttes nedover ved et visst intervall
            }
        }
        handleInput();
        
        // Tegner bakgrunnsfarge, litt kjedelig med grå så lager svart som i Tetris Battle
            draw_rectangle({0,0}, width*Tetromino::blockSize, height*Tetromino::blockSize, 0x1C1E2200);
            // Tegner rutenett slik at det ser finere ut
            for (int i = 0; i < width; i++) {
                draw_line({i*Tetromino::blockSize, 0}, {i*Tetromino::blockSize, height*Tetromino::blockSize}, 0x343A4000);
            } for (int i = 0; i < height; i++) {
                draw_line({0, i*Tetromino::blockSize}, {width*Tetromino::blockSize, i*Tetromino::blockSize}, 0x343A4000);
            }
            // Tegner høyresida, som skal være en UI 
            draw_rectangle({width*Tetromino::blockSize, 0}, Tetromino::blockSize*3, height*Tetromino::blockSize, 0x0E0E0E00);

            // Tegner poeng 
            draw_text({width*Tetromino::blockSize+Tetromino::blockSize/2, 9*height*Tetromino::blockSize/10}, "Points:", Color::white, 15U, Font::helvetica_bold);
            draw_text({width*Tetromino::blockSize+Tetromino::blockSize/2, 19*height*Tetromino::blockSize/20}, to_string(points), Color::white, 15U, Font::helvetica);
        
        
        // Har man tapt vil det komme opp en boks med score og beskjed om å trykke r for å restarte spillet
        if (gamestate == GameState::Lost) {
            int h = 7;
            if (!removeQ) {h = 4;}
            drawGridMatrix();
            draw_rectangle({width*Tetromino::blockSize/4, height*Tetromino::blockSize/4 + Tetromino::blockSize/2}, 6*Tetromino::blockSize, h*Tetromino::blockSize, 0x0E0E0E00);
            draw_text({width*Tetromino::blockSize/3, height*Tetromino::blockSize/3}, "GAME OVER", Color::white, 20U, Font::helvetica_bold);  
            draw_text({width*Tetromino::blockSize/3 - Tetromino::blockSize/2, height*Tetromino::blockSize/3 + 2*Tetromino::blockSize}, " Press R to restart", Color::white, 20U, Font::helvetica); 
            if (removeQ) {
                draw_text({width*Tetromino::blockSize/3 - Tetromino::blockSize/2, height*Tetromino::blockSize/3 + 4*Tetromino::blockSize}, " Press Q to save", Color::white, 20U,Font::helvetica);
                draw_text({width*Tetromino::blockSize/3 - Tetromino::blockSize/2, height*Tetromino::blockSize/3 + 5*Tetromino::blockSize}, "        score", Color::white, 20U,Font::helvetica);
            }
        }

        // Er man i spillet tegnes alle tetrominoene
        else if (gamestate == GameState::Playing ) {
            removeFullRows();
            drawShadowTetromino();
            drawCurrentTetromino();
            drawGridMatrix();
            drawNextTetrominos();
            drawSpareTetromino();
            speedUp += 1;
        } 
        
        // Er spillet pauset får man opp en boks med beskjed om å trykke P for å fortsette, eller R for å restarte
        else if (gamestate == GameState::Paused) {
            drawCurrentTetromino();
            drawGridMatrix();
            drawNextTetrominos();
            drawSpareTetromino();
            draw_rectangle({width*Tetromino::blockSize/4 - Tetromino::blockSize, height*Tetromino::blockSize/4 + Tetromino::blockSize/2}, 7*Tetromino::blockSize, 6*Tetromino::blockSize, 0x0E0E0E00);
            draw_text({width*Tetromino::blockSize/3 - Tetromino::blockSize, height*Tetromino::blockSize/3}, "GAME PAUSED", Color::white, 20U, Font::helvetica_bold);  
            draw_text({width*Tetromino::blockSize/3 - 3*Tetromino::blockSize/2, height*Tetromino::blockSize/3 + 2*Tetromino::blockSize}, "Press R to restart", Color::white, 20U, Font::helvetica_bold);
            draw_text({width*Tetromino::blockSize/3 - 3*Tetromino::blockSize/2, height*Tetromino::blockSize/3 + 4*Tetromino::blockSize}, "Press P to continue", Color::white, 20U, Font::helvetica_bold);
        } 

        // Her skriver man inn navnet sitt og lagrer scoren i Leadeboard.txt
        else if (gamestate == GameState::WritingName) {
            drawGridMatrix();
            draw_rectangle({width*Tetromino::blockSize/4 - Tetromino::blockSize, height*Tetromino::blockSize/4 + Tetromino::blockSize/2}, 7*Tetromino::blockSize, 3*Tetromino::blockSize, 0x0E0E0E00);
            draw_text({width*Tetromino::blockSize/3 - Tetromino::blockSize, height*Tetromino::blockSize/3}, "NAME:", Color::white, 20U, Font::helvetica_bold); 
            if (is_key_down(KeyboardKey::ENTER)) {
                addNameToFile();
                gamestate = GameState::Lost;
            }
            else {
                saveName();
                draw_text({width*Tetromino::blockSize/3 - Tetromino::blockSize, height*Tetromino::blockSize/3 + Tetromino::blockSize}, name, Color::white, 20U, Font::helvetica);
            }
        }
        
        next_frame();
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
    static bool lastRKeyState = false;
    static bool lastPKeyState = false;
    static bool lastCKeyState = false;

    bool currentZKeyState = is_key_down(KeyboardKey::Z);
    bool currentUpKeyState = is_key_down(KeyboardKey::UP);
    bool currentDownKeyState = is_key_down(KeyboardKey::DOWN);
    bool currentRightKeyState = is_key_down(KeyboardKey::RIGHT);
    bool currentLeftKeyState = is_key_down(KeyboardKey::LEFT);
    bool currentQKeyState = is_key_down(KeyboardKey::Q);
    bool currentTabKeyState = Fl::get_key(32);
    bool currentRKeyState = is_key_down(KeyboardKey::R);
    bool currentPKeyState = is_key_down(KeyboardKey::P);
    bool currentCKeyState = is_key_down(KeyboardKey::C);

    // Man kan kun flytte på blokkene når man spiller
    if (gamestate == GameState::Playing) {
        if(currentZKeyState && !lastZKeyState) {
        currentTetromino.rotateCounterClockwise();
            if (hasCrashed(currentTetromino)) {
                currentTetromino.rotateClockwise();
            }
        }

        if(currentUpKeyState && !lastUpKeyState) {
            currentTetromino.rotateClockwise();
            if (hasCrashed(currentTetromino)) {
                currentTetromino.rotateCounterClockwise();
            }
        }

        if(currentDownKeyState && !lastDownKeyState) {
            moveTetrominoDown();
        }

        if(currentLeftKeyState && !lastLeftKeyState) {
            currentTetromino.moveLeft();
            if (hasCrashed(currentTetromino)) {
                currentTetromino.moveRight();
            }
            
        }

        if(currentRightKeyState && !lastRightKeyState) {
            currentTetromino.moveRight();
            if (hasCrashed(currentTetromino)) {
                currentTetromino.moveLeft();
            }
        }

        if(currentQKeyState && !lastQKeyState) {
            fastenTetromino();
        }

        if(currentTabKeyState && !lastTabKeyState) {
            hardDrop();
        }

        if(currentPKeyState && !lastPKeyState) {
            gamestate = GameState::Paused;
        }

        if(currentCKeyState && !lastCKeyState) {
            if (spareTetromino.getMatrixSize() == 0) {
                spareTetromino = Tetromino(startPosition, currentTetromino.getType());
                generateRandomTetromino();
            }
            else {
                Tetromino temp(startPosition, currentTetromino.getType());
                currentTetromino = Tetromino(currentTetromino.getPosition(), spareTetromino.getType());
                spareTetromino = Tetromino(startPosition, temp.getType());
            } 
            int tries = 1;
            while (hasCrashed(currentTetromino) && tries < 4) {
                currentTetromino.moveRight();
                tries += 1;
            } while (hasCrashed(currentTetromino) && tries > 0) {
                currentTetromino.moveLeft();
                tries -= 1;  
            }
        }
    }
    
    
    // Har man tapt er det ikke så mye å gjøre
    else if (gamestate == GameState::Lost) {
        // Man kan skrive inn navnet sitt på leaderboardet. På en veldig dårlig måte. Kan bare skrive en gang dermed 
        if(currentQKeyState && !lastQKeyState && removeQ) {
            gamestate = GameState::WritingName;
        }
        // Restarter vinduet ved å trykke på R
        if(currentRKeyState && !lastRKeyState) {
                restartWindow();
        }
    }

    // Har man pauset kan man restarte spillet eller fortsette
    else if (gamestate == GameState::Paused) {
        if(currentRKeyState && !lastRKeyState) {
            restartWindow();
        } else if(currentPKeyState && !lastPKeyState) {
            gamestate = GameState::Playing;
        }
    }

    lastZKeyState = currentZKeyState;
    lastUpKeyState = currentUpKeyState;
    lastRightKeyState = currentRightKeyState;
    lastLeftKeyState = currentLeftKeyState;
    //lastDownKeyState = currentDownKeyState;
    lastTabKeyState = currentTabKeyState;
    lastRKeyState = currentRKeyState;
    lastPKeyState = currentPKeyState;
    lastCKeyState = currentCKeyState;
    lastQKeyState = currentQKeyState;
}

void TetrisWindow::generateRandomTetromino()
{
    // I starten er vektoren av neste Tetrominoer tom, lager fyller da denne opp
    if (nextTetrominos.size() == 0 ) {
        for (int i = 0; i < 3; i++) {
            int temp = rand() % 7;
            nextTetrominos.push_back(Tetromino(startPosition, TetrominoType{temp}));
        }
        int temp = rand() % 7;
        currentTetromino = Tetromino(startPosition, TetrominoType{temp});
    }
    
    // Hvis listen har 3 elementer vil currentTetromino bli den første av disse. 
    // Sletter så dette elementet og legger til et nytt på slutten.
    else {
        currentTetromino = nextTetrominos[0];
        nextTetrominos.erase(nextTetrominos.begin());
        int temp = rand() % 7;
        nextTetrominos.push_back(Tetromino(startPosition, TetrominoType{temp}));
    }
    
    // Man taper hvis man krasjer ved å generere en ny Tetromino
    if (hasCrashed(currentTetromino)) { gamestate = GameState::Lost; }
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
                //FreezingFace f({currentPos.x + (j*Tetromino::blockSize), currentPos.y + (i*Tetromino::blockSize)}, Tetromino::blockSize/2, {currentPos.x + (j*Tetromino::blockSize) - Tetromino::blockSize/4, currentPos.y + (i*Tetromino::blockSize) - Tetromino::blockSize/4}, {currentPos.x + (j*Tetromino::blockSize) + Tetromino::blockSize/4, currentPos.y + (i*Tetromino::blockSize) - Tetromino::blockSize/4}, 3, {currentPos.x + (j*Tetromino::blockSize) - Tetromino::blockSize/4, currentPos.y + (i*Tetromino::blockSize) - Tetromino::blockSize/4}, {currentPos.x + (j*Tetromino::blockSize), currentPos.y + (i*Tetromino::blockSize) + Tetromino::blockSize/4}, 3, {currentPos.x + (j*Tetromino::blockSize), currentPos.y + (i*Tetromino::blockSize)});
            }
        }
    }
}

void TetrisWindow::drawNextTetrominos()
{
    // Lager boks og skriver tekst
    draw_rectangle({(width) * Tetromino::blockSize + Tetromino::blockSize/2, 3*Tetromino::blockSize}, 2*Tetromino::blockSize, 8*Tetromino::blockSize, 0x0E0E0E00);
    draw_text({(width)*Tetromino::blockSize + 3*Tetromino::blockSize/4, 4*Tetromino::blockSize}, "NEXT", Color::white, 15U, Font::helvetica_bold);
    
    int index = 0;
    for (auto& tet : nextTetrominos) {
        // Kopierer her koden fra drawCurrentTetromino, men tegnes på høyresiden av vinduet
        int size = tet.getMatrixSize();
        int color = tet.getColor();
        for (int i = 0; i < size; i++) {
            for (int j = 0; j < size; j++) {
                TetrominoType block = tet.getBlock(i,j);
                // Deler opp draw for å gjøre det finere å se på. Tetrominoene som ikke har blokker i første rad vil flyttes opp
                // et hakk slik at mellomrommet blir likt på begge sider
                if (block == TetrominoType::J || block == TetrominoType::L || block == TetrominoType::T) {
                    draw_rectangle({(width) * Tetromino::blockSize + (j+1)*Tetromino::blockSize/2 + Tetromino::blockSize/4, Tetromino::blockSize*i/2 + 2*Tetromino::blockSize*(index+1) - Tetromino::blockSize/2 + Tetromino::blockSize*3}, Tetromino::blockSize/2, Tetromino::blockSize/2, color);
                } else if (block == TetrominoType::I) {
                    draw_rectangle({(width) * Tetromino::blockSize + (j+1)*Tetromino::blockSize/2, Tetromino::blockSize*i/2 + 2*Tetromino::blockSize*(index+1) - Tetromino::blockSize/2 + Tetromino::blockSize*3}, Tetromino::blockSize/2, Tetromino::blockSize/2, color);
                } else if (block == TetrominoType::O) {
                    draw_rectangle({(width) * Tetromino::blockSize + (j+1)*Tetromino::blockSize/2 + Tetromino::blockSize/2, Tetromino::blockSize*i/2 + 2*Tetromino::blockSize*(index+1) + Tetromino::blockSize*3}, Tetromino::blockSize/2, Tetromino::blockSize/2, color);
                }
                else if (block != TetrominoType::NONE) {
                    draw_rectangle({(width) * Tetromino::blockSize + (j+1)*Tetromino::blockSize/2 + Tetromino::blockSize/4, Tetromino::blockSize*i/2 + 2*Tetromino::blockSize*(index+1) + Tetromino::blockSize*3}, Tetromino::blockSize/2, Tetromino::blockSize/2, color);
                }
            }
        }

        index += 1;
    }
}

void TetrisWindow::moveTetrominoDown()
{
    currentTetromino.moveDown();
    if (hasCrashed(currentTetromino)) {
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

bool TetrisWindow::hasCrashed(Tetromino tet)
{

    int xCoord = tet.getPosition().x / Tetromino::blockSize;
    int yCoord = tet.getPosition().y / Tetromino::blockSize;
    int tetrSize = tet.getMatrixSize();
    
    // Itererer gjennom Tetronimoen for å sjekke 
    for (int i = 0; i < tetrSize; i++) {
        for (int j = 0; j < tetrSize; j++) {
            
            if (tet.getBlock(i,j) != TetrominoType::NONE) {  // Sjekker bare blokker som ikke er NONE
                // Først om den har gått utfor veggene
                if ( xCoord+j >= width || xCoord+j < 0) { return true; }
                // Så om den har truffet bunnen
                if ( yCoord+i >= height) { return true; }
                // Deretter om den treffer en annen blokk. Nå vil det (forhåpentligvis) ikke være noen indekser utfor matrisen
                if (tet.getBlock(i, j) != TetrominoType::NONE && gridMatrix[yCoord + i][xCoord + j] != TetrominoType::NONE) {
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
    vector<int> rowIndex;
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
            rowIndex.push_back(i);
        }

    } 

    // Itererer baklengs gjennom rowIndex. Starter så på hver rad som er fjernet, og swapper den oppover radene. Deretter neste 
    // rad som er fjernet. Etter hver swap vil indeksen til hver rad flyttes et hakk ned, og man må ta det i betraktning på hvor man
    // skal starte når vi itererer gjennom matrisen for det andre, tredje osv swappet
    vector<int>::reverse_iterator it = rowIndex.rbegin();       // Reverse iterator!! Dette har vi faktisk lært om!!!!!!!!!
    int rowsSwapped = 0;
    while(it != rowIndex.rend()) {
        
        for (int i = *it + rowsSwapped; i > 0; i--) {
            swap(gridMatrix[i], gridMatrix[i-1]);
        }

        it++;
        rowsSwapped += 1;
    }

    // Legger til slutt sammen poengene man får
    switch (rowsToFall)
    {
    case 1:
        points += 40;
        break;
    
    case 2:
        points += 100;
        break;

    case 3:
        points += 300;
        break;

    case 4:
        points += 1200;
        break;

    default:
        break;
    }
}

void TetrisWindow::hardDrop()
{
    int linesSkipped = 0;
    while (!hasCrashed(currentTetromino)) { currentTetromino.moveDown(); linesSkipped += 1; }
    currentTetromino.moveUp();
    points += linesSkipped - 1;
}

void TetrisWindow::restartWindow()
{
    // Resetter bakgrunnen 
    for (int i = 0; i < height; i++) {
        for (int j=0; j <width; j++) {
            gridMatrix[i][j] = TetrominoType::NONE;
        }
    }

    // Variabler som må nullstilles
    gamestate = GameState::Playing;
    points = 0;
    nextTetrominos.clear();
    spareTetromino = Tetromino();
    framesPerTetronimoMove = 20;
    

    // Navnvariabler
    removeQ = true;
    name = "";

    // Starter spillet på ny
    generateRandomTetromino();
}

void TetrisWindow::drawShadowTetromino()
{
    shadowTetromino = currentTetromino;
    while (!hasCrashed(shadowTetromino)) { shadowTetromino.moveDown(); }
    shadowTetromino.moveUp();

    // Akkurat som for current, bare at fargen er den samme som bakgrunnen slik at kun kantene vises.
    int size = shadowTetromino.getMatrixSize();
    Point currentPos = shadowTetromino.getPosition();

    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            if (shadowTetromino.getBlock(i, j) != TetrominoType::NONE) {
                draw_rectangle({currentPos.x + (j*Tetromino::blockSize), currentPos.y + (i*Tetromino::blockSize)}, Tetromino::blockSize, Tetromino::blockSize, 0x1C1E2200);
            }
        }
    }

}

void TetrisWindow::drawSpareTetromino()
{
    // Tegner et lite vindu som den ligger inni
    draw_rectangle({(width) * Tetromino::blockSize + Tetromino::blockSize/2, Tetromino::blockSize/2}, 2*Tetromino::blockSize, 2*Tetromino::blockSize, 0x0E0E0E00);
    int size = spareTetromino.getMatrixSize();
    int color = spareTetromino.getColor();

    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            TetrominoType block = spareTetromino.getBlock(i,j);
            // Deler opp igjen, da blir alle sentrert i rektangelet
            if (block == TetrominoType::J || block == TetrominoType::L || block == TetrominoType::T) {
                draw_rectangle({(width) * Tetromino::blockSize + 3*Tetromino::blockSize/4 + j*Tetromino::blockSize/2 , i*Tetromino::blockSize/2 + Tetromino::blockSize/2}, Tetromino::blockSize/2, Tetromino::blockSize/2, color);
            } else if (block == TetrominoType::O) {
                draw_rectangle({(width) * Tetromino::blockSize + Tetromino::blockSize + j*Tetromino::blockSize/2, i*Tetromino::blockSize/2 + 3/2*Tetromino::blockSize}, Tetromino::blockSize/2, Tetromino::blockSize/2, color);
            } else if (block == TetrominoType::I) {
                draw_rectangle({(width) * Tetromino::blockSize + Tetromino::blockSize/2 + j*Tetromino::blockSize/2, i*Tetromino::blockSize/2 + 1/2*Tetromino::blockSize}, Tetromino::blockSize/2, Tetromino::blockSize/2, color);
            } else if (block == TetrominoType::S || block == TetrominoType::Z) {
                draw_rectangle({(width) * Tetromino::blockSize + 3*Tetromino::blockSize/4 + j*Tetromino::blockSize/2, i*Tetromino::blockSize/2 + 3/2*Tetromino::blockSize}, Tetromino::blockSize/2, Tetromino::blockSize/2, color);
            }
        }
    }
}



void TetrisWindow::addNameToFile()
{
    ifstream leaderboard("Leaderboard.txt");

    vector<Points> sorted;
    string line;
    string spaceDelimiter = " ";
    
    // Legger inn alle linjene i en vektor som inneholder vektorer med navn og poeng
    while (getline(leaderboard, line)) {
        if (!line.empty()){
            vector<string> temp;
            size_t pos = 0;
            while ((pos = line.find(spaceDelimiter)) != string::npos) {
                temp.push_back(line.substr(0,pos));
                line.erase(0, pos + spaceDelimiter.length());
            }
            Points tempP = {temp[0], stoi(temp[1])};
            sorted.push_back(tempP);
        }
    }
    
    leaderboard.close();
    int size = sorted.size();
    
    // Er den større enn førsteplassen
    if (points > sorted[0].points) {
        Points temp = {name, points};
        sorted.insert(sorted.begin(), temp);
    }
    
        
    //Er den mindre enn siste
    
    else if (points <= sorted[size-1].points) {
        Points temp = {name, points};
        sorted.push_back(temp);
    }
    
    // Sjekker alle andre plasser, her fra andre plass til nest siste plass
    else {
        for (int i = 1; i < size; i++) {
            if (points < sorted[i-1].points && points > sorted[i].points ) {
                Points temp = {name, points};
                sorted.insert(sorted.begin() + i, temp);
                i++;
            }
        }
    }
    
    ofstream myFile("Leaderboard.txt");
    for (auto& element : sorted) {
        myFile << element.name << " " << to_string(element.points) << " " << "P" << "\n";
    }

    myFile.close();
    sorted.clear();


}

void TetrisWindow::saveName()
{
    static bool lastAKeyState = false;
    static bool lastBKeyState = false;
    static bool lastCKeyState = false;
    static bool lastDKeyState = false;
    static bool lastEKeyState = false;
    static bool lastFKeyState = false;
    static bool lastGKeyState = false;
    static bool lastHKeyState = false;
    static bool lastIKeyState = false;
    static bool lastJKeyState = false;
    static bool lastKKeyState = false;
    static bool lastLKeyState = false;
    static bool lastMKeyState = false;
    static bool lastNKeyState = false;
    static bool lastOKeyState = false;
    static bool lastPKeyState = false;
    static bool lastQKeyState = false;
    static bool lastRKeyState = false;
    static bool lastSKeyState = false;
    static bool lastTKeyState = false;
    static bool lastUKeyState = false;
    static bool lastVKeyState = false;
    static bool lastWKeyState = false;
    static bool lastXKeyState = false;
    static bool lastYKeyState = false;
    static bool lastZKeyState = false;
    static bool lastBackspaceKeyState = false;

    bool currentAKeyState = is_key_down(KeyboardKey::A);
    bool currentBKeyState = is_key_down(KeyboardKey::B);
    bool currentCKeyState = is_key_down(KeyboardKey::C);
    bool currentDKeyState = is_key_down(KeyboardKey::D);
    bool currentEKeyState = is_key_down(KeyboardKey::E);
    bool currentFKeyState = is_key_down(KeyboardKey::F);
    bool currentGKeyState = is_key_down(KeyboardKey::G);
    bool currentHKeyState = is_key_down(KeyboardKey::H);
    bool currentIKeyState = is_key_down(KeyboardKey::I);
    bool currentJKeyState = is_key_down(KeyboardKey::J);
    bool currentKKeyState = is_key_down(KeyboardKey::K);
    bool currentLKeyState = is_key_down(KeyboardKey::L);
    bool currentMKeyState = is_key_down(KeyboardKey::M);
    bool currentNKeyState = is_key_down(KeyboardKey::N);
    bool currentOKeyState = is_key_down(KeyboardKey::O);
    bool currentPKeyState = is_key_down(KeyboardKey::P);
    bool currentQKeyState = is_key_down(KeyboardKey::Q);
    bool currentRKeyState = is_key_down(KeyboardKey::R);
    bool currentSKeyState = is_key_down(KeyboardKey::S);
    bool currentTKeyState = is_key_down(KeyboardKey::T);
    bool currentUKeyState = is_key_down(KeyboardKey::U);
    bool currentVKeyState = is_key_down(KeyboardKey::V);
    bool currentWKeyState = is_key_down(KeyboardKey::W);
    bool currentXKeyState = is_key_down(KeyboardKey::X);
    bool currentYKeyState = is_key_down(KeyboardKey::Y);
    bool currentZKeyState = is_key_down(KeyboardKey::Z);
    bool currentBackspaceKeyState = is_key_down(KeyboardKey::BACKSPACE);


    if(currentAKeyState && !lastAKeyState) {
            name.push_back('A');
        }
    if(currentBKeyState && !lastBKeyState) {
            name.push_back('B');
        }
    if(currentCKeyState && !lastCKeyState) {
            name.push_back('C');
        }
    if(currentDKeyState && !lastDKeyState) {
            name.push_back('D');
        }

    if(currentEKeyState && !lastEKeyState) {
            name.push_back('E');
        }
    if(currentFKeyState && !lastFKeyState) {
            name.push_back('F');
        }
    if(currentGKeyState && !lastGKeyState) {
            name.push_back('G');
        }
    if(currentHKeyState && !lastHKeyState) {
            name.push_back('H');
        }
    if(currentIKeyState && !lastIKeyState) {
            name.push_back('I');
        }
    if(currentJKeyState && !lastJKeyState) {
            name.push_back('J');
        }
    if(currentKKeyState && !lastKKeyState) {
            name.push_back('K');
        }
    if(currentLKeyState && !lastLKeyState) {
            name.push_back('L');
        }
    if(currentMKeyState && !lastMKeyState) {
            name.push_back('M');
        }
    if(currentNKeyState && !lastNKeyState) {
            name.push_back('N');
        }
    if(currentOKeyState && !lastOKeyState) {
            name.push_back('O');
        }
    if(currentPKeyState && !lastPKeyState) {
            name.push_back('P');
        }
    if(currentQKeyState && !lastQKeyState) {
            name.push_back('Q');
        }
    if(currentRKeyState && !lastRKeyState) {
            name.push_back('R');
        }
    if(currentSKeyState && !lastSKeyState) {
            name.push_back('S');
        }
    if(currentTKeyState && !lastTKeyState) {
            name.push_back('T');
        }
    if(currentUKeyState && !lastUKeyState) {
            name.push_back('U');
        }
    if(currentVKeyState && !lastVKeyState) {
            name.push_back('V');
        }
    if(currentWKeyState && !lastWKeyState) {
            name.push_back('W');
        }
    if(currentXKeyState && !lastXKeyState) {
            name.push_back('X');
        }
    if(currentYKeyState && !lastYKeyState) {
            name.push_back('Y');
        }
    if(currentZKeyState && !lastZKeyState) {
            name.push_back('Z');
        }
    if(currentBackspaceKeyState && !lastBackspaceKeyState) {
            if (!name.empty()) {
                name.pop_back();
            }
        }

    if(name.length() > 8) {
        name.pop_back();
    }

    if(removeQ) {
        name.pop_back();
        removeQ = false;
    }

    lastAKeyState = currentAKeyState;
    lastBKeyState = currentBKeyState;
    lastCKeyState = currentCKeyState;
    lastDKeyState = currentDKeyState;
    lastEKeyState = currentEKeyState;
    lastFKeyState = currentFKeyState;
    lastGKeyState = currentGKeyState;
    lastHKeyState = currentHKeyState;
    lastIKeyState = currentIKeyState;
    lastJKeyState = currentJKeyState;
    lastKKeyState = currentKKeyState;
    lastLKeyState = currentLKeyState;
    lastMKeyState = currentMKeyState;
    lastNKeyState = currentNKeyState;
    lastOKeyState = currentOKeyState;
    lastPKeyState = currentPKeyState;
    lastQKeyState = currentQKeyState;
    lastRKeyState = currentRKeyState;
    lastSKeyState = currentSKeyState;
    lastTKeyState = currentTKeyState;
    lastUKeyState = currentUKeyState;
    lastVKeyState = currentVKeyState;
    lastWKeyState = currentWKeyState;
    lastXKeyState = currentXKeyState;
    lastYKeyState = currentYKeyState;
    lastZKeyState = currentZKeyState;
    lastBackspaceKeyState = currentBackspaceKeyState;

}

