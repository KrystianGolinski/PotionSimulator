#include "Psykg5Shield.h"
#include "Psykg5GameEngine.h"
#include "ImageManager.h"

Psykg5Shield::Psykg5Shield(BaseEngine* pEngine) :
    Psykg5GameObject(pEngine, 350, 550, 100, 33)
{
    std::vector<SimpleImage> frames;
    frames.push_back(ImageManager::loadImage("../Images/LadleAnimation1.png", true));
    frames.push_back(ImageManager::loadImage("../Images/LadleAnimation2.png", true));
    frames.push_back(ImageManager::loadImage("../Images/LadleAnimation3.png", true));
    frames.push_back(ImageManager::loadImage("../Images/LadleAnimation4.png", true));

    setAnimationFrames(frames, 100);
}

void Psykg5Shield::virtDraw()
{
    //if animating with frames
    if (m_isAnimating && !m_animationFrames.empty()) {
        m_animationFrames[m_currentFrame].renderImageWithMask(
            getEngine()->getForegroundSurface(),
            0, 0, m_iCurrentScreenX, m_iCurrentScreenY, m_iDrawWidth, m_iDrawHeight);
    }
    //base
    else {
		baseImage.renderImageWithMask(
			getEngine()->getForegroundSurface(),
			0, 0, m_iCurrentScreenX, m_iCurrentScreenY, m_iDrawWidth, m_iDrawHeight);
    }
}

void Psykg5Shield::virtDoUpdate(int iCurrentTime)
{
    if (getEngine()->isPaused())
        return;
    int oldX = m_iCurrentScreenX;
    handleInput();
    Psykg5GameObject::virtDoUpdate(iCurrentTime);
    //redraw if changed or animation for performance
    if (oldX != m_iCurrentScreenX || m_isAnimating || m_isBlocking)
        redrawDisplay();
}

void Psykg5Shield::handleInput()
{
    float moveX = 0.0f;

    if (getEngine()->isKeyPressed(SDLK_a))
        moveX -= m_moveSpeed;
    if (getEngine()->isKeyPressed(SDLK_d))
        moveX += m_moveSpeed;

    setVelocity(moveX, 0.0f);

    //error checks
    if (m_iCurrentScreenX < 0)
        m_iCurrentScreenX = 0;
    if (m_iCurrentScreenX + m_iDrawWidth > getEngine()->getWindowWidth())
        m_iCurrentScreenX = getEngine()->getWindowWidth() - m_iDrawWidth;
}

void Psykg5Shield::playBlockAnimation()
{
    m_isBlocking = true;
    playAnimation(false);
}