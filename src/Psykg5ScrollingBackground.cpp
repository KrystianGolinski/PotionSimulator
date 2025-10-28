#include "Psykg5ScrollingBackground.h"
#include "ImageManager.h"
#include <iostream>

ScrollingLayer::ScrollingLayer(SimpleImage image, float speedX, float speedY)
    : m_image(image), m_speedX(speedX), m_speedY(speedY), m_offsetX(0), m_offsetY(0)
{
}

void ScrollingLayer::update()
{
    if (std::abs(m_speedX) < 0.001f && std::abs(m_speedY) < 0.001f)
        return;
    
    //back and forth clouds
    if (m_backAndForth) {
        if (m_directionRight) {
            m_offsetX += m_speedX;
            if (m_offsetX >= m_maxOffset) {
                m_directionRight = false;
            }
        }
        else {
            m_offsetX -= m_speedX;
            if (m_offsetX <= m_minOffset) {
                m_directionRight = true;
            }
        }

        //update y if needed for performance
        if (std::abs(m_speedY) >= 0.001f) {
            m_offsetY += m_speedY;
        }
    }
    else {
        m_offsetX += m_speedX;
        m_offsetY += m_speedY;
    }
}

void ScrollingLayer::draw(DrawingSurface* pTarget, int windowWidth, int windowHeight)
{
    std::string filename = m_image.getImageURL();
    int imgWidth = m_image.getWidth();
    int imgHeight = m_image.getHeight();
    int offsetX = static_cast<int>(m_offsetX);
    int offsetY = static_cast<int>(m_offsetY);

    if (filename.find("BackgroundLayer3") != std::string::npos) {
        //sky
        m_image.renderImage(
            pTarget,
            0, 0,
            0, 0,
            windowWidth, windowHeight
        );
    }
    else if (filename.find("BackgroundLayer2") != std::string::npos) {
        //multiple cloud sets
        for (int x = offsetX % (windowWidth * 2); x < windowWidth + imgWidth; x += imgWidth) {
            m_image.renderImageWithMask(
                pTarget,
                0, 0,
                x, 50,
                imgWidth, imgHeight
            );
        }
    }
    else if (filename.find("BackgroundLayer1") != std::string::npos) {
        //grass
        m_image.renderImageWithMask(
            pTarget,
            0, 0,
            0, windowHeight - imgHeight,
            imgWidth, imgHeight,0
        );
    }
}

Psykg5ScrollingBackground::Psykg5ScrollingBackground(BaseEngine* pEngine)
    : m_pEngine(pEngine), m_isStationary(false), m_width(800), m_height(900)
{
    m_baseBackground = ImageManager::loadImage("../Images/Background.png", true);
    if (!m_baseBackground.exists()) {
        std::cerr << "Failed to load base background" << std::endl;
    }
}

Psykg5ScrollingBackground::Psykg5ScrollingBackground(BaseEngine* pEngine, int width, int height)
    : m_pEngine(pEngine), m_isStationary(false), m_width(width), m_height(height)
{
    m_baseBackground = ImageManager::loadImage("../Images/Background.png", true);
    if (!m_baseBackground.exists()) {
        std::cerr << "Failed to load base background" << std::endl;
    }
}

Psykg5ScrollingBackground::~Psykg5ScrollingBackground()
{
}

bool Psykg5ScrollingBackground::loadBackground(const char* imagePath)
{
    m_baseBackground = ImageManager::loadImage(imagePath, true);
    if (!m_baseBackground.exists()) {
        std::cerr << "Failed to load background image: " << imagePath << std::endl;
        return false;
    }
    std::cout << "Successfully loaded background: " << imagePath << std::endl;
    return true;
}

void Psykg5ScrollingBackground::addLayer(const char* imagePath, float speedX, float speedY)
{
    SimpleImage layerImage = ImageManager::loadImage(imagePath, true);
    if (!layerImage.exists()) {
        std::cerr << "Failed to load layer image: " << imagePath << std::endl;
        return;
    }

    m_layers.push_back(std::make_unique<ScrollingLayer>(layerImage, speedX, speedY));
}

void Psykg5ScrollingBackground::update()
{
    if (m_isStationary) return;

    for (auto& layer : m_layers) {
        layer->update();
    }
}

void Psykg5ScrollingBackground::changeOffset(float dx, float dy)
{
    for (auto& layer : m_layers) {
        float x = 0, y = 0;

        x += dx;
        y += dy;

        layer->setOffset(x, y);
    }
}

void Psykg5ScrollingBackground::draw(DrawingSurface* pTarget)
{
    if (!pTarget) return;

    int windowWidth = m_width;
    int windowHeight = m_height;

    if (m_pEngine->getWindowWidth() > 0) windowWidth = m_pEngine->getWindowWidth();
    if (m_pEngine->getWindowHeight() > 0) windowHeight = m_pEngine->getWindowHeight();

    if (m_baseBackground.exists()) {
        int imgWidth = m_baseBackground.getWidth();
        int imgHeight = m_baseBackground.getHeight();
        for (int y = 0; y < windowHeight; y += imgHeight) {
            for (int x = 0; x < windowWidth; x += imgWidth) {
                m_baseBackground.renderImage(
                    pTarget,
                    0, 0,
                    x, y,
                    imgWidth, imgHeight
                );
            }
        }
    }
    for (auto& layer : m_layers) {
        layer->draw(pTarget, windowWidth, windowHeight);
    }
}

void Psykg5ScrollingBackground::setLayerBackAndForth(int layerIndex, bool enabled, float minOffset, float maxOffset)
{
    if (layerIndex >= 0 && layerIndex < m_layers.size()) {
        m_layers[layerIndex]->setBackAndForthMovement(enabled, minOffset, maxOffset);
    }
}