#pragma once
#include "ExampleFilterPointClasses.h"
#include "BaseEngine.h"

class Psykg5FilterPoints : public FilterPoints
{
public:
    Psykg5FilterPoints(BaseEngine* pEngine)
        : m_pEngine(pEngine),
        m_dZoomFactor(1.0),
        m_iXOffset(0), m_iYOffset(0)
    {
    }

    virtual bool filter(DrawingSurface* surface, int& x, int& y, unsigned int& uiColour, bool setting) override
    {
        int centerX = m_pEngine->getWindowWidth() / 2;
        int centerY = m_pEngine->getWindowHeight() / 2;
        x -= centerX;
        y -= centerY;
        //zoom
        x = static_cast<int>(x * m_dZoomFactor);
        y = static_cast<int>(y * m_dZoomFactor);
        x += centerX;
        y += centerY;
        //translate
        x += m_iXOffset;
        y += m_iYOffset;
        return true;
    }

    virtual int filterConvertVirtualToRealXPosition(int xVirtual) override
    {
        int centerX = m_pEngine->getWindowWidth() / 2;
        xVirtual -= centerX;
        xVirtual = static_cast<int>(xVirtual * m_dZoomFactor);
        xVirtual += centerX;
        return xVirtual + m_iXOffset;
    }

    virtual int filterConvertVirtualToRealYPosition(int yVirtual) override
    {
        int centerY = m_pEngine->getWindowHeight() / 2;
        yVirtual -= centerY;
        yVirtual = static_cast<int>(yVirtual * m_dZoomFactor);
        yVirtual += centerY;
        return yVirtual + m_iYOffset;
    }
    virtual int filterConvertRealToVirtualXPosition(int x) override
    {
        //undo translation
        x -= m_iXOffset;
        //undo zoom
        int centerX = m_pEngine->getWindowWidth() / 2;
        x -= centerX;
        if (m_dZoomFactor != 0)
            x = static_cast<int>(x / m_dZoomFactor);
        x += centerX;
        return x;
    }
    virtual int filterConvertRealToVirtualYPosition(int y) override
    {
        //undo translation
        y -= m_iYOffset;
        //undo zoom
        int centerY = m_pEngine->getWindowHeight() / 2;
        y -= centerY;
        if (m_dZoomFactor != 0)
            y = static_cast<int>(y / m_dZoomFactor);
        y += centerY;
        return y;
    }
    void zoomIn(double factor = 1.1)
    {
        m_dZoomFactor *= factor;
        //zoom limit
        if (m_dZoomFactor > 3.0) m_dZoomFactor = 3.0;
    }
    void zoomOut(double factor = 1.1)
    {
        m_dZoomFactor /= factor;
        //zoom limit
        if (m_dZoomFactor < 0.3) m_dZoomFactor = 0.3; // Limit min zoom
    }

    //set translation offset
    void setOffset(int x, int y)
    {
        m_iXOffset = x;
        m_iYOffset = y;
    }
    //change translation offset
    void changeOffset(int dx, int dy)
    {
        m_iXOffset += dx;
        m_iYOffset += dy;
    }
    double getZoomFactor() const { return m_dZoomFactor; }
private:
    BaseEngine* m_pEngine;
    double m_dZoomFactor;  // Current zoom factor
    int m_iXOffset;        // X translation offset
    int m_iYOffset;        // Y translation offset
};