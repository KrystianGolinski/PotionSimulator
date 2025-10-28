#pragma once
#include "DisplayableObject.h"
#include "SimpleImage.h"
#include "ImageManager.h"

class Psykg5Tree : public DisplayableObject
{
public:
    Psykg5Tree(BaseEngine* pEngine, int xStart, int yStart)
        : DisplayableObject(xStart, yStart, pEngine, 100, 118), m_scaleFactor(1.0f)
    {
        m_treeImage = ImageManager::loadImage("../Images/Tree.png", true);
        if (!m_treeImage.exists()) {
            std::cerr << "Tree image not found" << std::endl;
        }
    }
    float getScaleFactor() const { return m_scaleFactor; }
    void adjustPosition(int dx, int dy) {
        m_iCurrentScreenX += dx;
        m_iCurrentScreenY += dy;
    }
    void adjustSize(float scale) {
        m_scaleFactor *= scale;
        //limits
        if (m_scaleFactor < 0.5f) m_scaleFactor = 0.5f;
        if (m_scaleFactor > 2.0f) m_scaleFactor = 2.0f;
    }
    void virtDraw() override
    {
        if (!isVisible())
            return;
        int scaledWidth = static_cast<int>(m_iDrawWidth * m_scaleFactor);
        int scaledHeight = static_cast<int>(m_iDrawHeight * m_scaleFactor);
        if (m_treeImage.exists()) {
            m_treeImage.renderImageWithMask(
                getEngine()->getForegroundSurface(),
                0, 0,
                m_iCurrentScreenX, m_iCurrentScreenY,
                scaledWidth, scaledHeight
            );
        }
        else {
            std::cerr << "Tree image not found, using fallback drawing" << std::endl;
        }
    }
private:
    SimpleImage m_treeImage;
    float m_scaleFactor;
};