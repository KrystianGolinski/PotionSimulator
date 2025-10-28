#include "header.h"
#include "Psykg5GameEngine.h"
#include "Psykg5Flyer.h"
#include "Psykg5Shield.h"
#include "Psykg5Cauldron.h"
#include "Psykg5State.h"
#include "Psykg5AdvancedCollision.h"
#include "Psykg5SparkleEffect.h"
#include "Psykg5Tree.h"
#include <chrono>
#include <cstdlib>


Psykg5GameEngine::Psykg5GameEngine(Psykg5StateManager& stateManager)
    : BaseEngine(), stateManager(stateManager), m_bScrollingEnabled(false), m_bAnimationEnabled(false)
{

    m_mainMenuState = std::make_unique<MainMenuState>();
    m_playingState = std::make_unique<PlayingState>();
    m_pausedState = std::make_unique<PausedState>();
    m_gameOverState = std::make_unique<GameOverState>();
    m_winState = std::make_unique<WinState>();
    m_informationState = std::make_unique<InformationState>();
    m_pForegroundFilter = std::make_unique<Psykg5FilterPoints>(this);
    m_pScrollingBackground = std::make_unique<Psykg5ScrollingBackground>(this);

    m_pWaveMapping = std::make_unique<Psykg5WavePixelMapping>(3.0, 0.2, 0.05);

    m_pScrollingBackground->addLayer("../Images/BackgroundLayer3.png", 0.0f, 0.0f);//sky
    m_pScrollingBackground->addLayer("../Images/BackgroundLayer2.png", 2.0f, 0.0f);//clouds
    m_pScrollingBackground->addLayer("../Images/BackgroundLayer1.png", 0.0f, 0.0f);//grass

    m_pScrollingBackground->setLayerBackAndForth(1, true, -200, 200);//clouds

    m_currentState = m_mainMenuState.get();
    stateManager.SetCurrentState(GameState::MainMenu);
}

Psykg5GameEngine::~Psykg5GameEngine() {
}

void Psykg5GameEngine::setState(GameState newState) {
    if (m_currentState != nullptr) {
        m_currentState->exit(this);
    }
    stateManager.SetCurrentState(newState);

    clearWaveEffects();

    if (newState == GameState::MainMenu ||
        newState == GameState::GameOver ||
        newState == GameState::Win ||
        newState == GameState::Information) {
        clearGameObjects();
    }

    switch (newState) {
    case GameState::MainMenu:
        m_currentState = m_mainMenuState.get();
        break;
    case GameState::Playing:
        m_currentState = m_playingState.get();
        break;
    case GameState::Paused:
        m_currentState = m_pausedState.get();
        break;
    case GameState::GameOver:
        m_currentState = m_gameOverState.get();
        break;
    case GameState::Win:
        m_currentState = m_winState.get();
        break;
    case GameState::Information:
        m_currentState = m_informationState.get();
        break;
    }

    if (m_currentState != nullptr) {
        m_currentState->enter(this);
    }
    redrawDisplay();
}

void Psykg5GameEngine::clearGameObjects() {
    drawableObjectsChanged();
    destroyOldObjects(true);
    createObjectArray(30);
    clearWaveEffects();
    m_gameRunning = false;
    redrawDisplay();
}

void Psykg5GameEngine::resetGameData() {
    m_score = 0;
    m_difficulty = 1;
    m_flyerSpawnTimer = 0;
    m_elapsedSeconds = 0;
    clearWaveEffects();
    clearGameObjects();
}

void Psykg5GameEngine::lockAndSetupBackground(int StateKey)
{
    lockBackgroundForDrawing();
    virtSetupBackgroundBuffer(StateKey);
    unlockBackgroundForDrawing();
    redrawDisplay();
}

void Psykg5GameEngine::virtSetupBackgroundBuffer(int StateKey)
{
    if (m_currentState != nullptr) {
        m_currentState->setupBackgroundBuffer(this);
    }
}

void Psykg5GameEngine::virtKeyDown(int iKeyCode)
{
    if (m_currentState != nullptr) {
        m_currentState->handleKeyDown(this, iKeyCode);
    }

    if (stateManager.GetCurrentState() == GameState::Playing) {
        if (m_bScrollingEnabled) {
            switch (iKeyCode) {
            case SDLK_LEFT:
                scrollBackground(-5, 0);
                break;
            case SDLK_RIGHT:
                scrollBackground(5, 0);
                break;
            case SDLK_UP:
                scrollBackground(0, -5);
                break;
            case SDLK_DOWN:
                scrollBackground(0, 5);
                break;
            }
        }

        //tree manipulation
        switch (iKeyCode) {
        case SDLK_j: //left
            scrollTrees(-10, 0);
            break;
        case SDLK_l: //right
            scrollTrees(10, 0);
            break;
        case SDLK_i: //up
            scrollTrees(0, -10);
            break;
        case SDLK_k: //down
            scrollTrees(0, 10);
            break;
        case SDLK_EQUALS: //plus key zoom
            zoomTrees(1.1f);
            break;
        case SDLK_MINUS: //minus key zoom
            zoomTrees(0.9f);
            break;
        }
    }
}

int Psykg5GameEngine::virtInitialiseObjects()
{
    drawableObjectsChanged();
    destroyOldObjects(true);
    createObjectArray(20);
    getForegroundSurface()->setDrawPointsFilter(m_pForegroundFilter.get());
    loadHighScores();
    setState(GameState::MainMenu);
    return 0;
}

void Psykg5GameEngine::virtDrawStringsOnTop()
{
    if (m_currentState != nullptr) {
        m_currentState->drawStringsOnTop(this);
    }
}

void Psykg5GameEngine::CreateFlyers(BaseEngine* pEngine, int Quantity)
{
    for (int i = 0; i < Quantity; ++i) {
        int randomXPos = std::rand() % ((getWindowWidth() - 25) + 1) + 25;
        Psykg5Flyer* pFlyer = new Psykg5Flyer(pEngine, randomXPos, 0);
        drawableObjectsChanged();
        appendObjectToArray(pFlyer);
    }
}

void Psykg5GameEngine::checkCollisions()
{
    Psykg5Shield* shield = nullptr;
    Psykg5Cauldron* cauldron = nullptr;

    //get shield and cauldron
    for (int i = 0; i < getContentCount(); i++) {
        DisplayableObject* obj = getDisplayableObject(i);
        if (obj != nullptr) {
            Psykg5Shield* testShield = dynamic_cast<Psykg5Shield*>(obj);
            if (testShield) {
                shield = testShield;
                continue;
            }

            Psykg5Cauldron* testCauldron = dynamic_cast<Psykg5Cauldron*>(obj);
            if (testCauldron) {
                cauldron = testCauldron;
            }
        }
    }

    if (!shield || !cauldron) {
        return;
    }

    //update wave timers and remove obsolete
    for (auto it = m_waveEffectTimers.begin(); it != m_waveEffectTimers.end();) {
        it->second -= 16; //16ms per frame
        if (it->second <= 0) {
            //after timer remove effect and bomb
            if (it->first && !it->first->isDestroyed()) {
                it->first->setBouncing(false);
                removeFlyerAndCreateEffect(it->first, true);
                CreateFlyers(this, 1);
            }
            it = m_waveEffectTimers.erase(it);
        }
        else {
            ++it;
        }
    }

    if (m_pWaveMapping) {
        m_pWaveMapping->updateTime(getModifiedTime() * 0.001); //seconds conversion
    }

    //collision with each flyer check
    for (int i = 0; i < getContentCount(); i++) {
        DisplayableObject* obj = getDisplayableObject(i);
        if (obj != nullptr) {
            Psykg5Flyer* flyer = dynamic_cast<Psykg5Flyer*>(obj);
            if (flyer && !flyer->isDestroyed()) {
                //with shield using pixel perfect
                if (flyer->checkPixelPerfectCollision(
                    shield->getXCentre() - shield->getWidth() / 2,
                    shield->getYCentre() - shield->getHeight() / 2,
                    shield->getWidth(), shield->getHeight())) {

                    shield->playBlockAnimation();

                    if (flyer->getIngredientType() == IngredientType::Bomb) {
                        m_waveEffectTimers[flyer] = WAVE_EFFECT_DURATION;
                        flyer->setWaveEffectActive(true);

                        flyer->setBouncing(true);

                        //bomb bounce
                        int shieldCenterX = shield->getXCentre();
                        int flyerCenterX = flyer->getXCentre();
                        float bounceDirectionX = (flyerCenterX - shieldCenterX) > 0 ? 1.0f : -1.0f;

                        flyer->setVelocity(bounceDirectionX * 6.0f, -8.0f);

                        //move bomb from shield immediately
                        flyer->SetXPosition(flyer->getXCentre() + (int)(bounceDirectionX * 10));
                        flyer->SetYPosition(flyer->getYCentre() - 10);

                        m_score += flyer->getScoreValue();
                        if (m_score > m_highScore) {
                            m_highScore = m_score;
                        }
                    }
                    //regular ingredient
                    else {
                        m_score += flyer->getScoreValue();
                        if (m_score > m_highScore) {
                            m_highScore = m_score;
                        }

                        removeFlyerAndCreateEffect(flyer, true);
                        CreateFlyers(this, 1);
                    }
                }

                //polygon collision with cauldron
                else if (cauldron->isCollided(
                    flyer->getXCentre() - flyer->getDrawWidth() / 2,
                    flyer->getYCentre() - flyer->getDrawHeight() / 2,
                    flyer->getDrawWidth(), flyer->getDrawHeight()))
                {
                    cauldron->setIsAnimating(true);

                    //game over
                    if (!m_isTransitioningToGameOver) {
                        m_isTransitioningToGameOver = true;
                        m_transitionStartTime = getModifiedTime();
                    }
                    return;
                }
            }
        }
    }
}

void Psykg5GameEngine::virtMainLoopPreUpdate() {
    if (m_gameRunning && stateManager.GetCurrentState() == GameState::Playing) {
        if (m_currentState != nullptr) {
            m_currentState->update(this, getModifiedTime());
        }
    }
}

void Psykg5GameEngine::virtMainLoopDoBeforeUpdate()
{
    if ((m_gameRunning && stateManager.GetCurrentState() == GameState::Playing) || m_isTransitioningToGameOver)
    {
        if (m_isTransitioningToGameOver) {
            int currentTime = getModifiedTime();
            redrawDisplay();

            if (currentTime > m_transitionStartTime + m_transitionDuration) {
                m_isTransitioningToGameOver = false;
                endGame();
                return;
            }
        }

        for (auto it = m_waveEffectTimers.begin(); it != m_waveEffectTimers.end(); ++it) {
            Psykg5Flyer* flyer = it->first;
            if (flyer) {
                flyer->setWaveEffectActive(true);

                //wave effect
                flyer->redrawRectangle();
            }
        }
        if (!m_isTransitioningToGameOver) {
            checkCollisions();
        }
        if (m_bScrollingEnabled && m_pScrollingBackground)
        {
            m_pScrollingBackground->update();
            redrawDisplay();
        }
    }
}

void Psykg5GameEngine::removeFlyerAndCreateEffect(Psykg5Flyer* flyer, bool wasCaught)
{
    if (!flyer) return;
    int centerX = flyer->getXCentre();
    int centerY = flyer->getYCentre();
    flyer->destroy();
    drawableObjectsChanged();
    removeDisplayableObject(flyer);
    if (wasCaught) {
        createSparkleEffect(centerX, centerY);
    }
}

void Psykg5GameEngine::virtRenderScreen()
{
    //error check
    if (m_iWindowWidth <= 0 || m_iWindowHeight <= 0) {
        m_iWindowWidth = 800;
        m_iWindowHeight = 900;
    }
    BaseEngine::virtRenderScreen();
    //rendering
    if (m_bScrollingEnabled || m_bAnimationEnabled) {
        //force screen update
        m_pForegroundSurface->mySDLUpdateTexture(m_pSDL2Texture);
        SDL_RenderCopy(m_pSDL2Renderer, m_pSDL2Texture, NULL, NULL);
        SDL_RenderPresent(m_pSDL2Renderer);
    }
}

void Psykg5GameEngine::startNewGame() {
    drawableObjectsChanged();
    destroyOldObjects(true);
    createObjectArray(10);
    drawableObjectsChanged();
    storeObjectInArray(0, new Psykg5Shield(this));
    appendObjectToArray(new Psykg5Cauldron(this));
    drawableObjectsChanged();
    CreateFlyers(this, 3);
    m_gameRunning = true;
    m_score = 0;
    m_gameStartTime = std::chrono::high_resolution_clock::now();
}

void Psykg5GameEngine::endGame() {
    m_gameRunning = false;
    m_isTransitioningToGameOver = false;

    if (m_score > 0) {
        HighScore newScore(m_playerName, m_score, m_elapsedSeconds);
        m_highScores.push_back(newScore);
        std::sort(m_highScores.begin(), m_highScores.end());
        if (m_highScores.size() > 10) {
            m_highScores.resize(10);
        }
        saveHighScores();
        if (m_score > m_highScore) {
            m_highScore = m_score;
        }
    }
    setState(GameState::GameOver);
}

void Psykg5GameEngine::updateDifficulty() {
    //increase difficulty every 10 seconds
    int newDifficulty = 1 + (m_elapsedSeconds / 10);

    if (newDifficulty > m_difficulty) {
        m_difficulty = newDifficulty;

        //20% chance bomb spawn at difficulty increase
        if (std::rand() % 100 < 20) {
            CreateBomb();
        }
    }

    if (m_difficulty > 10) {
        m_difficulty = 10;
    }
}

void Psykg5GameEngine::copyAllBackgroundBuffer()
{
    if (m_bScrollingEnabled && m_pScrollingBackground)
    {
        m_pScrollingBackground->draw(m_pForegroundSurface);
    }
    else
    {
        BaseEngine::copyAllBackgroundBuffer();
    }
}

void Psykg5GameEngine::scrollBackground(int x, int y)
{
    if (m_pScrollingBackground)
    {
        m_pScrollingBackground->changeOffset(static_cast<float>(x), static_cast<float>(y));
        redrawDisplay();
    }
}

void Psykg5GameEngine::createSparkleEffect(int x, int y)
{
    Psykg5SparkleEffect* effect = new Psykg5SparkleEffect(this, x, y);
    drawableObjectsChanged();
    appendObjectToArray(effect);
    redrawRectangle(x - 30, y - 30, 60, 60);
}

void Psykg5GameEngine::loadHighScores() {
    m_highScores.clear();
    std::string data;
    if (Psykg5FileManager::loadDataFromFile("highscores.txt", data)) {
        std::istringstream stream(data);
        std::string line;
        while (std::getline(stream, line)) {
            std::istringstream lineStream(line);
            std::string name;
            int score;
            int time;
            if (lineStream >> name >> score >> time) {
                HighScore hs;
                hs.name = name;
                hs.score = score;
                hs.timeElapsed = time;
                m_highScores.push_back(hs);
            }
        }
        std::sort(m_highScores.begin(), m_highScores.end());
        if (!m_highScores.empty()) {
            m_highScore = m_highScores[0].score;
        }
    }
}

void Psykg5GameEngine::saveHighScores() {
    std::ostringstream stream;
    for (const auto& score : m_highScores) {
        stream << score.name << " " << score.score << " " << score.timeElapsed << std::endl;
    }

    if (Psykg5FileManager::saveDataToFile("highscores.txt", stream.str())) {
        std::cout << "High scores saved successfully" << std::endl;
    }
    else {
        std::cerr << "Failed to save high scores" << std::endl;
    }
}
void Psykg5GameEngine::scrollForeground(int dx, int dy)
{
    if (m_pForegroundFilter) {
        m_pForegroundFilter->changeOffset(dx, dy);
        redrawDisplay();
    }
}

void Psykg5GameEngine::scrollTrees(int dx, int dy) {
    for (auto it = m_trees.begin(); it != m_trees.end();) {
        DisplayableObject* tree = *it;
        if (tree == nullptr) {
            it = m_trees.erase(it);
            continue;
        }

        try {
            Psykg5Tree* pTree = dynamic_cast<Psykg5Tree*>(tree);
            if (pTree) {
                pTree->redrawRectangle();
                pTree->adjustPosition(dx, dy);
                pTree->redrawRectangle();
            }
            ++it;
        }
        catch (...) {
            it = m_trees.erase(it);
        }
    }

    redrawDisplay();
}

void Psykg5GameEngine::addTreeToScrollList(DisplayableObject* tree) {
    if (tree != nullptr) {
        m_trees.push_back(tree);
    }
}
void Psykg5GameEngine::zoomTrees(float scale)
{
    for (auto it = m_trees.begin(); it != m_trees.end();) {
        DisplayableObject* tree = *it;

        if (tree == nullptr) {
            it = m_trees.erase(it);
            continue;
        }

        try {
            Psykg5Tree* pTree = dynamic_cast<Psykg5Tree*>(tree);
            if (pTree) {
                pTree->redrawRectangle();
                pTree->adjustSize(scale);
                pTree->redrawRectangle();
            }
            ++it;
        }
        catch (...) {
            it = m_trees.erase(it);
        }
    }

    redrawDisplay();
}

void Psykg5GameEngine::virtMouseWheel(int iX, int iY, int iDelta, int timestamp)
{
    if (stateManager.GetCurrentState() == GameState::Playing) {
        if (iY != 0) {
            float scaleFactor = (iY > 0) ? 1.1f : 0.9f;
            zoomTrees(scaleFactor);
        }
    }
}

void Psykg5GameEngine::zoomForeground(bool zoomIn)
{
    float scaleFactor = zoomIn ? 1.1f : 0.9f;
    zoomTrees(scaleFactor);
}

void Psykg5GameEngine::CreateBomb()
{
    int randomXPos = std::rand() % ((getWindowWidth() - 25) + 1) + 25;
    Psykg5Flyer* pFlyer = new Psykg5Flyer(this, randomXPos, 0);
    pFlyer->createSpecificIngredient(IngredientType::Bomb);
    drawableObjectsChanged();
    appendObjectToArray(pFlyer);
}