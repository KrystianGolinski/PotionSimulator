#include "Psykg5SparkleEffect.h"
#include "Psykg5GameEngine.h"
#include "ImageManager.h"
#include <iostream>

Psykg5SparkleEffect::Psykg5SparkleEffect(BaseEngine* pEngine, int x, int y)
    : DisplayableObject(x - 30, y - 30, pEngine, 60, 60, true),
    m_currentFrame(0),
    m_frameDelay(100),
    m_lastFrameTime(0),
    m_isFinished(false)
{
    m_frames.push_back(ImageManager::loadImage("../Images/SparklesAnimation1.png", true));
    m_frames.push_back(ImageManager::loadImage("../Images/SparklesAnimation2.png", true));
    m_frames.push_back(ImageManager::loadImage("../Images/SparklesAnimation3.png", true));
    m_frames.push_back(ImageManager::loadImage("../Images/SparklesAnimation4.png", true));
    m_lastFrameTime = pEngine->getModifiedTime();
}

void Psykg5SparkleEffect::virtDraw()
{
    if (m_isFinished || m_currentFrame >= m_frames.size()) {
        return;
    }
    m_frames[m_currentFrame].renderImageWithMask(
        getEngine()->getForegroundSurface(),
        0, 0, m_iCurrentScreenX, m_iCurrentScreenY, m_iDrawWidth, m_iDrawHeight);
}

void Psykg5SparkleEffect::virtDoUpdate(int iCurrentTime)
{
    if (m_isFinished) {
        return;
    }
    if (iCurrentTime > m_lastFrameTime + m_frameDelay) {
        m_lastFrameTime = iCurrentTime;
        m_currentFrame++;
        if (m_currentFrame >= m_frames.size()) {
            m_isFinished = true;
            return;
        }
        redrawRectangle();
    }
}