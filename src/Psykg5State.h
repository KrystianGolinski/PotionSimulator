#pragma once
#include "BaseEngine.h"
class Psykg5GameEngine;
class Psykg5State {
public:
    virtual ~Psykg5State() = default;
    virtual void enter(Psykg5GameEngine* engine) = 0;
    virtual void exit(Psykg5GameEngine* engine) = 0;
    virtual void handleKeyDown(Psykg5GameEngine* engine, int iKeyCode) = 0;
    virtual void update(Psykg5GameEngine* engine, int iCurrentTime) = 0;
    virtual void setupBackgroundBuffer(Psykg5GameEngine* engine) = 0;
    virtual void drawStringsOnTop(Psykg5GameEngine* engine) = 0;
};
class MainMenuState : public Psykg5State {
public:
    void enter(Psykg5GameEngine* engine) override;
    void exit(Psykg5GameEngine* engine) override;
    void handleKeyDown(Psykg5GameEngine* engine, int iKeyCode) override;
    void update(Psykg5GameEngine* engine, int iCurrentTime) override;
    void setupBackgroundBuffer(Psykg5GameEngine* engine) override;
    void drawStringsOnTop(Psykg5GameEngine* engine) override;
private:
    bool m_showNamePrompt = false;
    int m_nameInputBoxX = 150;
    int m_nameInputBoxY = 450;
    int m_nameInputBoxWidth = 300;
    int m_nameInputBoxHeight = 40;
};
class PlayingState : public Psykg5State {
public:
    void enter(Psykg5GameEngine* engine) override;
    void exit(Psykg5GameEngine* engine) override;
    void handleKeyDown(Psykg5GameEngine* engine, int iKeyCode) override;
    void update(Psykg5GameEngine* engine, int iCurrentTime) override;
    void setupBackgroundBuffer(Psykg5GameEngine* engine) override;
    void drawStringsOnTop(Psykg5GameEngine* engine) override;
};
class PausedState : public Psykg5State {
public:
    void enter(Psykg5GameEngine* engine) override;
    void exit(Psykg5GameEngine* engine) override;
    void handleKeyDown(Psykg5GameEngine* engine, int iKeyCode) override;
    void update(Psykg5GameEngine* engine, int iCurrentTime) override;
    void setupBackgroundBuffer(Psykg5GameEngine* engine) override;
    void drawStringsOnTop(Psykg5GameEngine* engine) override;
};
class GameOverState : public Psykg5State {
public:
    void enter(Psykg5GameEngine* engine) override;
    void exit(Psykg5GameEngine* engine) override;
    void handleKeyDown(Psykg5GameEngine* engine, int iKeyCode) override;
    void update(Psykg5GameEngine* engine, int iCurrentTime) override;
    void setupBackgroundBuffer(Psykg5GameEngine* engine) override;
    void drawStringsOnTop(Psykg5GameEngine* engine) override;
};
class WinState : public Psykg5State {
public:
    void enter(Psykg5GameEngine* engine) override;
    void exit(Psykg5GameEngine* engine) override;
    void handleKeyDown(Psykg5GameEngine* engine, int iKeyCode) override;
    void update(Psykg5GameEngine* engine, int iCurrentTime) override;
    void setupBackgroundBuffer(Psykg5GameEngine* engine) override;
    void drawStringsOnTop(Psykg5GameEngine* engine) override;
};
class InformationState : public Psykg5State {
public:
    void enter(Psykg5GameEngine* engine) override;
    void exit(Psykg5GameEngine* engine) override;
    void handleKeyDown(Psykg5GameEngine* engine, int iKeyCode) override;
    void update(Psykg5GameEngine* engine, int iCurrentTime) override;
    void setupBackgroundBuffer(Psykg5GameEngine* engine) override;
    void drawStringsOnTop(Psykg5GameEngine* engine) override;
};