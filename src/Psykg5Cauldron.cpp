#include "Psykg5Cauldron.h"
#include "ImageManager.h"
#include "Psykg5GameEngine.h"
#include "Psykg5AdvancedCollision.h"
#include <vector>

Psykg5Cauldron::Psykg5Cauldron(BaseEngine* pEngine)
    : Psykg5GameObject(pEngine, 300, 800, 200, 245)
{
    setVelocity(0.0f, 0.0f);
    setupAnimations();
}

void Psykg5Cauldron::setupAnimations()
{
    std::vector<SimpleImage> frames;
    frames.push_back(ImageManager::loadImage("../Images/CauldronAnimation1.png", true));
    frames.push_back(ImageManager::loadImage("../Images/CauldronAnimation2.png", true));
    frames.push_back(ImageManager::loadImage("../Images/CauldronAnimation3.png", true));
    frames.push_back(ImageManager::loadImage("../Images/CauldronAnimation4.png", true));
    setAnimationFrames(frames, 100);
}

void Psykg5Cauldron::virtDraw()
{
    //if animating with frames draw frame
    if (m_isAnimating && !m_animationFrames.empty()) {
        m_animationFrames[m_currentFrame].renderImageWithMask(
            getEngine()->getForegroundSurface(),
            0, 0, m_iCurrentScreenX, m_iCurrentScreenY, m_iDrawWidth, m_iDrawHeight);
    }
    //if not draw base
    else {
		m_baseImage.renderImageWithMask(
			getEngine()->getForegroundSurface(),
			0, 0, m_iCurrentScreenX, m_iCurrentScreenY, m_iDrawWidth, m_iDrawHeight);
    }
}

void Psykg5Cauldron::virtDoUpdate(int iCurrentTime)
{
    Psykg5GameEngine* engine = dynamic_cast<Psykg5GameEngine*>(getEngine());
    if (engine && engine->isPaused() && !engine->m_isTransitioningToGameOver)
        return;
    int oldX = m_iCurrentScreenX;
    int oldY = m_iCurrentScreenY;
    Psykg5GameObject::virtDoUpdate(iCurrentTime);
    if (oldX != m_iCurrentScreenX || oldY != m_iCurrentScreenY || m_isAnimating)
        redrawRectangle();
}

bool Psykg5Cauldron::isCollided(int flyerX, int flyerY, int flyerWidth, int flyerHeight)
{
    std::vector<std::pair<int, int>> cauldronShape = {
        {m_iCurrentScreenX + 20, m_iCurrentScreenY + 20},
        {m_iCurrentScreenX + m_iDrawWidth - 20, m_iCurrentScreenY + 20},
        {m_iCurrentScreenX + m_iDrawWidth, m_iCurrentScreenY + m_iDrawHeight - 40},
        {m_iCurrentScreenX + m_iDrawWidth - 30, m_iCurrentScreenY + m_iDrawHeight},
        {m_iCurrentScreenX + 30, m_iCurrentScreenY + m_iDrawHeight},
        {m_iCurrentScreenX, m_iCurrentScreenY + m_iDrawHeight - 40}
    };
    std::vector<std::pair<int, int>> flyerShape = {
        {flyerX, flyerY},
        {flyerX + flyerWidth, flyerY},
        {flyerX + flyerWidth, flyerY + flyerHeight},
        {flyerX, flyerY + flyerHeight}
    };
    return Psykg5AdvancedCollision::checkConcavePolygonCollision(cauldronShape, flyerShape);
}