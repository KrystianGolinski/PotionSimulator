#include "Psykg5GameObject.h"
#include "Psykg5AdvancedCollision.h"
#include "BaseEngine.h"
#include <cmath>

Psykg5GameObject::Psykg5GameObject(BaseEngine* pEngine, int startX, int startY, int width, int height)
    : DisplayableObject(startX, startY, pEngine, width, height),
    m_currentFrame(0),
    m_frameDuration(100),
    m_lastFrameTime(0),
    m_isAnimating(false),
    m_loopAnimation(false),
    m_velocityX(0.0f),
    m_velocityY(0.0f)
{
}

void Psykg5GameObject::virtDoUpdate(int iCurrentTime)
{
    static int lastTime = iCurrentTime;
    int timeDelta = iCurrentTime - lastTime;
    lastTime = iCurrentTime;
    if (timeDelta > 100)
        timeDelta = 100;
    updatePhysics(timeDelta);
    updateAnimation(iCurrentTime);
    redrawDisplay();
}

void Psykg5GameObject::setAnimationFrames(const std::vector<SimpleImage>& frames, int frameDuration)
{
    m_animationFrames = frames;
    m_frameDuration = frameDuration;
    m_currentFrame = 0;
}

void Psykg5GameObject::playAnimation(bool loop)
{
    if (m_animationFrames.size() == 0)
        return;
    m_isAnimating = true;
    m_loopAnimation = loop;
    m_currentFrame = 0;
    m_lastFrameTime = getEngine()->getModifiedTime();
}

void Psykg5GameObject::stopAnimation()
{
    m_isAnimating = false;
}

void Psykg5GameObject::setVelocity(float vx, float vy)
{
    m_velocityX = vx;
    m_velocityY = vy;
}

void Psykg5GameObject::applyForce(float fx, float fy)
{
    m_velocityX += fx;
    m_velocityY += fy;
}

bool Psykg5GameObject::isCollidedWith(Psykg5GameObject* other) const
{
    if (!other)
        return false;
    return Psykg5AdvancedCollision::checkRectangles(
        m_iCurrentScreenX, m_iCurrentScreenX + m_iDrawWidth,
        m_iCurrentScreenY, m_iCurrentScreenY + m_iDrawHeight,
        other->m_iCurrentScreenX, other->m_iCurrentScreenX + other->m_iDrawWidth,
        other->m_iCurrentScreenY, other->m_iCurrentScreenY + other->m_iDrawHeight
    );
}

bool Psykg5GameObject::isCollidedWith(int x, int y, int width, int height) const
{
    return Psykg5AdvancedCollision::checkRectangles(
        m_iCurrentScreenX, m_iCurrentScreenX + m_iDrawWidth,
        m_iCurrentScreenY, m_iCurrentScreenY + m_iDrawHeight,
        x, x + width,
        y, y + height
    );
}

void Psykg5GameObject::updatePhysics(int timeDelta)
{
    m_iCurrentScreenX += static_cast<int>(m_velocityX * timeDelta * 0.1f);
    m_iCurrentScreenY += static_cast<int>(m_velocityY * timeDelta * 0.1f);
    if (m_iCurrentScreenX < 0)
        m_iCurrentScreenX = 0;
    if (m_iCurrentScreenY < 0)
        m_iCurrentScreenY = 0;
    if (m_iCurrentScreenX + m_iDrawWidth > getEngine()->getWindowWidth())
        m_iCurrentScreenX = getEngine()->getWindowWidth() - m_iDrawWidth;
    if (m_iCurrentScreenY + m_iDrawHeight > getEngine()->getWindowHeight())
        m_iCurrentScreenY = getEngine()->getWindowHeight() - m_iDrawHeight;
}

void Psykg5GameObject::updateAnimation(int iCurrentTime)
{
    if (!m_isAnimating || m_animationFrames.size() == 0)
        return;
    static int lastFrame = -1;
    if (m_currentFrame != lastFrame) {
        lastFrame = m_currentFrame;
    }
    if (iCurrentTime > m_lastFrameTime + m_frameDuration) {
        m_lastFrameTime = iCurrentTime;
        m_currentFrame++;

        if (m_currentFrame >= m_animationFrames.size()) {
            if (m_loopAnimation)
                m_currentFrame = 0;
            else {
                m_currentFrame = m_animationFrames.size() - 1;
                m_isAnimating = false;
            }
        }
        redrawRectangle();
    }
}