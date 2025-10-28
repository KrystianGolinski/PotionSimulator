#include "Psykg5State.h"
#include "Psykg5GameEngine.h"
#include "Psykg5Tree.h"
#include <chrono>
#include <cstdio>

void MainMenuState::enter(Psykg5GameEngine* engine) {
    engine->lockAndSetupBackground(0);
    engine->m_gameRunning = false;
    engine->setEnteringName(false);
    engine->clearEnteredText();
    m_showNamePrompt = false;
    engine->enableAnimation(true);
    engine->enableScrolling(false);
}

void MainMenuState::exit(Psykg5GameEngine* engine) {
}

void MainMenuState::handleKeyDown(Psykg5GameEngine* engine, int iKeyCode) {
    if (engine->isEnteringName()) {
        if (iKeyCode == SDLK_RETURN) {
            if (!engine->getCurrentEnteredText().empty()) {
                engine->setPlayerName(engine->getCurrentEnteredText());
            }
            engine->setEnteringName(false);
            m_showNamePrompt = false;
            engine->redrawDisplay();
        }
        else if (iKeyCode == SDLK_ESCAPE) {
            engine->setEnteringName(false);
            m_showNamePrompt = false;
            engine->redrawDisplay();
        }
        else if (iKeyCode == SDLK_BACKSPACE) {
            engine->removeLastCharFromEnteredText();
            engine->redrawDisplay();
        }
        else if ((iKeyCode >= 'a' && iKeyCode <= 'z') ||
            (iKeyCode >= 'A' && iKeyCode <= 'Z') ||
            (iKeyCode >= '0' && iKeyCode <= '9') ||
            iKeyCode == ' ' || iKeyCode == '_' || iKeyCode == '-') {
            if (engine->getCurrentEnteredText().length() < 15) { //name limit
                engine->appendToEnteredText((char)iKeyCode);
                engine->redrawDisplay();
            }
        }
        return;
    }

    switch (iKeyCode) {
    case 101://e
        engine->saveHighScores();
        engine->setExitWithCode(0);
        break;
    case 112://p
        engine->setState(GameState::Playing);
        break;
    case 105://i
        engine->setState(GameState::Information);
        break;
    case 110://n
        m_showNamePrompt = true;
        engine->setEnteringName(true);
        engine->clearEnteredText();
        engine->redrawDisplay();
        break;
    }
}

void MainMenuState::update(Psykg5GameEngine* engine, int iCurrentTime) {
}

void MainMenuState::setupBackgroundBuffer(Psykg5GameEngine* engine) {
    SimpleImage menuImage = ImageManager::loadImage("../Images/MainMenu.png", true);
    if (menuImage.exists()) {
        menuImage.renderImage(
            engine->getBackgroundSurface(),
            0, 0,
            0, 0,
            engine->getWindowWidth(), engine->getWindowHeight()
        );
    }
    else {
        //sample menu if image cant load
        engine->fillBackground(0x202060);//blue
        engine->drawBackgroundString(200, 300, "Potion Simulator", 0xFFFFFF, NULL);
        engine->drawBackgroundString(200, 350, "P - Play  I - Info  E - Exit", 0xFFFFFF, NULL);
    }
}

void MainMenuState::drawStringsOnTop(Psykg5GameEngine* engine) {
    if (engine->isEnteringName()) {
        //text entry overlay
        engine->drawForegroundRectangle(m_nameInputBoxX - 10, m_nameInputBoxY - 10,
            m_nameInputBoxX + m_nameInputBoxWidth + 10, m_nameInputBoxY + m_nameInputBoxHeight + 10, 0x000000);
        engine->drawForegroundRectangle(m_nameInputBoxX, m_nameInputBoxY,
            m_nameInputBoxX + m_nameInputBoxWidth, m_nameInputBoxY + m_nameInputBoxHeight, 0x303030);
        engine->drawForegroundString(m_nameInputBoxX, m_nameInputBoxY - 40,
            "Enter your name (press Enter to confirm):", 0xFFFFFF, NULL);

        //display name as entered
        std::string displayText = engine->getCurrentEnteredText() + "_";
        engine->drawForegroundString(m_nameInputBoxX + 10, m_nameInputBoxY + 10,
            displayText.c_str(), 0xFFFFFF, NULL);

        engine->drawForegroundString(m_nameInputBoxX, m_nameInputBoxY + m_nameInputBoxHeight + 20,
            "Press Escape to cancel", 0xFFFFFF, NULL);
    }
    else if (m_showNamePrompt) {
        //shouldn't enter here
        m_showNamePrompt = false;
    }
    else {
        //current name
        std::string nameText = "Current Player: " + engine->getPlayerName();
        engine->drawForegroundString(350, 300, nameText.c_str(), 0xFFFFFF, NULL);
        engine->drawForegroundString(20, 20, "Press 'N' to change name", 0xFFFFFF, NULL);
    }
}

void PlayingState::enter(Psykg5GameEngine* engine) {
    engine->lockAndSetupBackground(1);
    if (!engine->m_gameRunning) {
        engine->startNewGame();
        engine->m_gameStartTime = std::chrono::high_resolution_clock::now();
    }
    engine->m_gameRunning = true;
    engine->enableScrolling(true);
    engine->enableAnimation(false);
    engine->clearTreeList();

    int windowWidth = engine->getWindowWidth();
    int windowHeight = engine->getWindowHeight();

    //foreground trees
    Psykg5Tree* leftTree = new Psykg5Tree(engine, static_cast<int>(windowWidth * 0.1), static_cast<int>(windowHeight * 0.9));
    engine->appendObjectToArray(leftTree);
    engine->addTreeToScrollList(leftTree);
    Psykg5Tree* left2Tree = new Psykg5Tree(engine, static_cast<int>(windowWidth * 0.3), static_cast<int>(windowHeight * 0.9));
    engine->appendObjectToArray(left2Tree);
    engine->addTreeToScrollList(left2Tree);
    Psykg5Tree* rightTree = new Psykg5Tree(engine, static_cast<int>(windowWidth * 0.8), static_cast<int>(windowHeight * 0.9));
    engine->appendObjectToArray(rightTree);
    engine->addTreeToScrollList(rightTree);
}

void PlayingState::exit(Psykg5GameEngine* engine) {
    engine->clearTreeList();
}

void PlayingState::handleKeyDown(Psykg5GameEngine* engine, int iKeyCode) {
    switch (iKeyCode) {
    case SDLK_SPACE://space
        engine->setState(GameState::Paused);
        break;
    }
}

void PlayingState::update(Psykg5GameEngine* engine, int iCurrentTime) {
    //game timer
    auto currentTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::seconds>(
        currentTime - engine->m_gameStartTime);
    engine->m_elapsedSeconds = duration.count();

    engine->updateDifficulty();

    //flyer spawn based on difficulty
    if (engine->m_elapsedSeconds > engine->m_flyerSpawnTimer) {
        engine->m_flyerSpawnTimer = engine->m_elapsedSeconds + 5 - (engine->m_difficulty / 2);
        engine->CreateFlyers(engine, engine->m_difficulty);
    }

    // Win after 2 minutes
    if (engine->m_elapsedSeconds >= 120) {
        engine->setState(GameState::Win);
        return;
    }
}

void PlayingState::setupBackgroundBuffer(Psykg5GameEngine* engine) {
}

void PlayingState::drawStringsOnTop(Psykg5GameEngine* engine) {
    char buf[128];
    sprintf(buf, "Survival Time: %d  Score: %d  High Score: %d",
        engine->m_elapsedSeconds, engine->m_score, engine->m_highScore);
    engine->drawForegroundString(10, 30, buf, 0xFFFFFF, NULL);
}

void PausedState::enter(Psykg5GameEngine* engine) {
    engine->lockAndSetupBackground(2);
    engine->pause();
}

void PausedState::exit(Psykg5GameEngine* engine) {
    engine->unpause();
}

void PausedState::handleKeyDown(Psykg5GameEngine* engine, int iKeyCode) {
    switch (iKeyCode) {
    case SDLK_SPACE://space
        engine->setState(GameState::Playing);
        break;
    }
}

void PausedState::update(Psykg5GameEngine* engine, int iCurrentTime) {
}

void PausedState::setupBackgroundBuffer(Psykg5GameEngine* engine) {
}

void PausedState::drawStringsOnTop(Psykg5GameEngine* engine) {
    engine->drawForegroundString(280, 400, "GAME PAUSED", 0xFFFFFF, NULL);
}

void GameOverState::enter(Psykg5GameEngine* engine) {
    engine->lockAndSetupBackground(3);
    engine->m_gameRunning = false;
    engine->enableScrolling(false);
}

void GameOverState::exit(Psykg5GameEngine* engine) {
}

void GameOverState::handleKeyDown(Psykg5GameEngine* engine, int iKeyCode) {
    if (iKeyCode == 101) {//e
        engine->saveHighScores();
        engine->setState(GameState::MainMenu);
    }
    else if (iKeyCode == 112) {//p
        engine->setState(GameState::Playing);
    }
}

void GameOverState::update(Psykg5GameEngine* engine, int iCurrentTime) {
}

void GameOverState::setupBackgroundBuffer(Psykg5GameEngine* engine) {
    SimpleImage GameOverImage = ImageManager::loadImage("../Images/GameOver.png", true);
    if (GameOverImage.exists()) {
        GameOverImage.renderImage(
            engine->getBackgroundSurface(),
            0, 0,
            0, 0,
            engine->getWindowWidth(), engine->getWindowHeight()
        );
    }
    else {
        //cant load image so sample background
        engine->fillBackground(0x600000);//red
        engine->drawBackgroundString(250, 300, "GAME OVER", 0xFFFFFF, NULL);
    }
}

void GameOverState::drawStringsOnTop(Psykg5GameEngine* engine) {
    char buf[128];
    sprintf(buf, "Your Score: %d  Time Survived: %d seconds", engine->m_score, engine->m_elapsedSeconds);
    engine->drawForegroundString(50, 300, buf, 0xFFFFFF, NULL);
    engine->drawForegroundString(50, 350, "High Scores", 0xFFFFFF, NULL);
    int y = 400;
    for (size_t i = 0; i < engine->m_highScores.size() && i < 5; ++i) {
        sprintf(buf, "%d. %s - %d pts (%d sec)", (int)i + 1,
            engine->m_highScores[i].name.c_str(),
            engine->m_highScores[i].score,
            engine->m_highScores[i].timeElapsed);
        engine->drawForegroundString(50, y, buf, 0xFFFFFF, NULL);
        y += 30;
    }
}

void WinState::enter(Psykg5GameEngine* engine) {
    engine->lockAndSetupBackground(4);
    engine->m_gameRunning = false;
    engine->enableAnimation(true);
    engine->enableScrolling(false);
}

void WinState::exit(Psykg5GameEngine* engine) {
}

void WinState::handleKeyDown(Psykg5GameEngine* engine, int iKeyCode) {
    if (iKeyCode == 101) {//e
        engine->setState(GameState::MainMenu);
    }
    else if (iKeyCode == 112) {//p
        engine->setState(GameState::Playing);
    }
}

void WinState::update(Psykg5GameEngine* engine, int iCurrentTime) {
}

void WinState::setupBackgroundBuffer(Psykg5GameEngine* engine) {
    SimpleImage GameOverImage = ImageManager::loadImage("../Images/GameWon.png", true);
    if (GameOverImage.exists()) {
        GameOverImage.renderImage(
            engine->getBackgroundSurface(),
            0, 0,
            0, 0,
            engine->getWindowWidth(), engine->getWindowHeight()
        );
    }
    else {
		//cant load image so sample background
        engine->fillBackground(0x006000);//green
        engine->drawBackgroundString(250, 300, "YOU WIN!", 0xFFFFFF, NULL);
    }
}

void WinState::drawStringsOnTop(Psykg5GameEngine* engine) {
    char buf[128];
    sprintf(buf, "Your Score: %d", engine->m_score);
    engine->drawForegroundString(50, 200, buf, 0x000000, NULL);

    engine->drawForegroundString(50, 300, "High Scores", 0x000000, NULL);
    int y = 350;
    for (size_t i = 0; i < engine->m_highScores.size() && i < 5; ++i) {
        sprintf(buf, "%d. %s - %d", (int)i + 1, engine->m_highScores[i].name.c_str(), engine->m_highScores[i].score);
        engine->drawForegroundString(50, y, buf, 0x000000, NULL);
        y += 30;
    }
}

void InformationState::enter(Psykg5GameEngine* engine) {
    engine->lockAndSetupBackground(5);
}

void InformationState::exit(Psykg5GameEngine* engine) {
}

void InformationState::handleKeyDown(Psykg5GameEngine* engine, int iKeyCode) {
    if (iKeyCode == 101) {//e
        engine->setState(GameState::MainMenu);
    }
}

void InformationState::update(Psykg5GameEngine* engine, int iCurrentTime) {
}

void InformationState::setupBackgroundBuffer(Psykg5GameEngine* engine) {
    SimpleImage infoImage = ImageManager::loadImage("../Images/InformationScreen.png", true);
    if (infoImage.exists()) {
        infoImage.renderImage(
            engine->getBackgroundSurface(),
            0, 0,
            0, 0,
            engine->getWindowWidth(), engine->getWindowHeight()
        );
    }
    else {
        //cant load image so sample background
        engine->fillBackground(0x204080);//blue
        engine->drawBackgroundString(250, 200, "GAME INFORMATION", 0xFFFFFF, NULL);
    }
}

void InformationState::drawStringsOnTop(Psykg5GameEngine* engine) {
    return;
}