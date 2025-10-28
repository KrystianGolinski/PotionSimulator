#pragma once
#include "Psykg5GameObject.h"
#include "ImageManager.h"

class Psykg5Cauldron : public Psykg5GameObject {
public:
    Psykg5Cauldron(BaseEngine* pEngine);
    void virtDraw() override;
    void virtDoUpdate(int iCurrentTime) override;
    bool isCollided(int flyerX, int flyerY, int flyerWidth, int flyerHeight);
    void setIsAnimating(bool animating) 
    {
        if (animating) {
            playAnimation(true);
        }
        else {
            stopAnimation();
        }
    }
    int getWidth() { return m_iDrawWidth; }
    int getHeight() { return m_iDrawHeight; }

private:
    void setupAnimations();
	SimpleImage m_baseImage = ImageManager::loadImage("../Images/CauldronAnimation1.png", true);
};