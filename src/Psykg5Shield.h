#pragma once
#include "Psykg5GameObject.h"
#include "ImageManager.h"
class Psykg5Shield : public Psykg5GameObject
{
public:
    Psykg5Shield(BaseEngine* pEngine);
    void virtDraw() override;
    void virtDoUpdate(int iCurrentTime) override;
    int getWidth() const { return m_iDrawWidth; }
    int getHeight() const { return m_iDrawHeight; }
    void playBlockAnimation();
private:
    int m_moveSpeed = 5; //movement speed
    bool m_isBlocking = false;
	SimpleImage baseImage = ImageManager::loadImage("../Images/LadleSlim.png", true);
    void handleInput();
};