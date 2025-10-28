#pragma once
#include "BaseEngine.h"   
#include "Psykg5StateManager.h" 
#include "Psykg5TemplateUtils.h"
#include "Psykg5Shield.h"
#include "Psykg5ScrollingBackground.h"
#include "Psykg5FileManager.h"
#include "ExampleFilterPointClasses.h"
#include "Psykg5FilterPoints.h"
#include "Psykg5WavePixelMapping.h"
#include <sstream>
#include <chrono>
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>

class Psykg5Cauldron;
class Psykg5Flyer;
class Psykg5State;
class Psykg5Effect;
class Psykg5SparkleEffect;

struct HighScore {
    std::string name;
    int score;
    int timeElapsed;

    HighScore() : name(""), score(0), timeElapsed(0) {}
    HighScore(const std::string& n, int s, int t = 0) : name(n), score(s), timeElapsed(t) {}

    //overloading for sorting
    bool operator<(const HighScore& other) const {
        return score > other.score; 
    }
};

class Psykg5GameEngine : public BaseEngine
{
public:
    Psykg5GameEngine(Psykg5StateManager& stateManager);
    ~Psykg5GameEngine();
    void lockAndSetupBackground(int StateKey);
    void virtSetupBackgroundBuffer(int StateKey);
    void virtKeyDown(int iKeyCode) override;
    int virtInitialiseObjects() override;
    void virtDrawStringsOnTop() override;
    void CreateFlyers(BaseEngine* pEngine, int Quantity);
    void checkCollisions();
    void virtMainLoopDoBeforeUpdate() override;
    void virtMainLoopPreUpdate() override;
    void setState(GameState newState);
    void resetGameData();
    void removeFlyerAndCreateEffect(Psykg5Flyer* flyer, bool wasCaught);
    void virtRenderScreen() override;
    void createSparkleEffect(int x, int y);
    void loadHighScores();
    void saveHighScores();
    void startNewGame();
    void endGame();
    void updateDifficulty();
    void CreateBomb();
    void copyAllBackgroundBuffer() override;
    void enableScrolling(bool enable) { m_bScrollingEnabled = enable; }
    void enableAnimation(bool enable) { m_bAnimationEnabled = enable; }
    void scrollBackground(int x, int y);
    void clearGameObjects();
    void clearEnteredText() { m_currentEnteredText = ""; }
    void scrollForeground(int dx, int dy);
    void scrollTrees(int dx, int dy);
    void addTreeToScrollList(DisplayableObject* tree);
    void clearTreeList() { m_trees.clear(); }
    void zoomTrees(float scale);
    void virtMouseWheel(int x, int y, int which, int timestamp) override;
    void zoomForeground(bool zoomIn);
    void setPlayerName(const std::string& name) {
        m_playerName = name.empty() ? "NoName" : name;
    }
    std::string getPlayerName() const { return m_playerName; }
    bool isEnteringName() const { return m_isEnteringName; }
    void setEnteringName(bool entering) { m_isEnteringName = entering; }
    void appendToEnteredText(char c) { m_currentEnteredText += c; }
    void removeLastCharFromEnteredText() {
        if (!m_currentEnteredText.empty())
            m_currentEnteredText.pop_back();
    }
    std::string getCurrentEnteredText() const { return m_currentEnteredText; }
    bool m_gameRunning = false;
    std::chrono::time_point<std::chrono::high_resolution_clock> m_gameStartTime;
    int m_elapsedSeconds = 0;
    int m_score = 0;
    int m_highScore = 0;
    int m_difficulty = 1;
    int m_flyerSpawnTimer = 0;
    std::vector<HighScore> m_highScores;
    bool m_isTransitioningToGameOver = false;
    int m_transitionStartTime = 0;
    int m_transitionDuration = 1000;
    std::string m_playerName = "NoName";  // Default player name
    bool m_isEnteringName = false;
    std::string m_currentEnteredText = "";
    Psykg5WavePixelMapping* getWaveMapping() { return m_pWaveMapping.get(); }
    void clearWaveEffects() {
        m_waveEffectTimers.clear();
    }
private:
    Psykg5StateManager& stateManager;
    Psykg5State* m_currentState = nullptr;
    std::unique_ptr<Psykg5State> m_mainMenuState;
    std::unique_ptr<Psykg5State> m_playingState;
    std::unique_ptr<Psykg5State> m_pausedState;
    std::unique_ptr<Psykg5State> m_gameOverState;
    std::unique_ptr<Psykg5State> m_winState;
    std::unique_ptr<Psykg5State> m_informationState;
    std::unique_ptr<Psykg5ScrollingBackground> m_pScrollingBackground;
    std::unique_ptr<Psykg5FilterPoints> m_pForegroundFilter;
    std::vector<DisplayableObject*> m_trees;
    bool m_bScrollingEnabled;
    bool m_bAnimationEnabled;
    std::unique_ptr<Psykg5WavePixelMapping> m_pWaveMapping;
    std::unordered_map<Psykg5Flyer*, int> m_waveEffectTimers; //duration for each flyer
    const int WAVE_EFFECT_DURATION = 100;  // ms 
};