#pragma once
#include "BaseEngine.h"
#include "SimpleImage.h"
#include <vector>

class ScrollingLayer {
public:
    ScrollingLayer(SimpleImage image, float speedX, float speedY);
    void update();
    void draw(DrawingSurface* pTarget, int windowWidth, int windowHeight);
    void setOffset(float x, float y) { m_offsetX = x; m_offsetY = y; }
    void changeOffset(float dx, float dy) { m_offsetX += dx; m_offsetY += dy; }
    void setBackAndForthMovement(bool enabled, float minOffset = 0, float maxOffset = 100) {
        m_backAndForth = enabled;
        m_minOffset = minOffset;
        m_maxOffset = maxOffset;
    }
private:
    SimpleImage m_image;
    float m_speedX;
    float m_speedY;
    float m_offsetX;
    float m_offsetY;
    bool m_backAndForth = false;
    float m_minOffset = 0;
    float m_maxOffset = 0;
    bool m_directionRight = true;
};

class Psykg5ScrollingBackground
{
public:
    Psykg5ScrollingBackground(BaseEngine* pEngine);
    Psykg5ScrollingBackground(BaseEngine* pEngine, int width, int height);
    ~Psykg5ScrollingBackground();
    bool loadBackground(const char* imagePath);
    void addLayer(const char* imagePath, float speedX, float speedY);
    void update();
    void draw(DrawingSurface* pTarget);
    void setStationary(bool stationary) { m_isStationary = stationary; }
    void changeOffset(float dx, float dy);
    void setLayerBackAndForth(int layerIndex, bool enabled, float minOffset = 0, float maxOffset = 100);
private:
    BaseEngine* m_pEngine;
    SimpleImage m_baseBackground;
    std::vector<std::unique_ptr<ScrollingLayer>> m_layers;
    bool m_isStationary;
    int m_width;
    int m_height;
};