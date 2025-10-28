#include "Psykg5Flyer.h"
#include "Psykg5Shield.h"
#include "ImageManager.h"
#include "Psykg5Cauldron.h"
#include "Psykg5AdvancedCollision.h"
#include "Psykg5GameEngine.h"
#include <cstdlib>
#include <ctime> 
#include <cmath>

Psykg5Flyer::Psykg5Flyer(BaseEngine* pEngine, int startX, int startY)
    : Psykg5GameObject(pEngine, startX, startY, 40, 40), targetX(0), targetY(0)
{
    float initialSpeed = 5.0f + static_cast<float>(std::rand()) / (static_cast<float>(RAND_MAX / 1.5f)); //flyer speed
    m_baseSpeed = initialSpeed;
    setVelocity(0.0f, initialSpeed);

    Psykg5Cauldron* cauldron = nullptr;
    for (int i = 0; i < getEngine()->getContentCount(); i++) {
        DisplayableObject* obj = getEngine()->getDisplayableObject(i);
        if (dynamic_cast<Psykg5Cauldron*>(obj) != nullptr) {
            cauldron = dynamic_cast<Psykg5Cauldron*>(obj);
            break;
        }
    }

    //use found cauldron
    if (cauldron != nullptr) {
        targetX = cauldron->getXCentre();
        targetY = cauldron->getYCentre();
    }
    else {
        std::cerr << "Error: Cauldron not found" << std::endl;
    }

    //90/10 split bombs and ingredients
    int randomValue = std::rand() % 100;
    if (randomValue < 30) {
        createSpecificIngredient(IngredientType::Banana);
    }
    else if (randomValue < 60) {
        createSpecificIngredient(IngredientType::Strawberry);
    }
    else if (randomValue < 90) {
        createSpecificIngredient(IngredientType::Pineapple);
    }
    else {
        createSpecificIngredient(IngredientType::Bomb);
    }
    //flight patterns
    if (m_ingredientType == IngredientType::Bomb) {
        m_flightPattern = FlightPattern::Straight;
    }
    else {
        int patternRoll = std::rand() % 4;
        m_flightPattern = static_cast<FlightPattern>(patternRoll);
    }
    m_zigzagDirection = (std::rand() % 2 == 0) ? 1 : -1;
    m_zigzagWidth = 40 + (std::rand() % 40);  // 40-80 pixel wide zigzag

    m_circleAngle = std::rand() % 360; //initially random starting angle
    m_circleRadius = 15 + (std::rand() % 20); //initially random circle radius

    //get expected y for arrival
    calculateArrivalPath();
    setupAnimations();
    playAnimation(true);
}

void Psykg5Flyer::createSpecificIngredient(IngredientType type)
{
    m_ingredientType = type;
    switch (m_ingredientType) {
    case IngredientType::Banana:
        m_baseImage = ImageManager::loadImage("../Images/BananaAnimation1.png", true);
        m_iDrawWidth = 33;
        m_iDrawHeight = 40;
        m_scoreValue = 10;
        break;
    case IngredientType::Strawberry:
        m_baseImage = ImageManager::loadImage("../Images/StrawberryAnimation1.png", true);
        m_iDrawWidth = 28;
        m_iDrawHeight = 40;
        m_scoreValue = 20;
        break;
    case IngredientType::Pineapple:
        m_baseImage = ImageManager::loadImage("../Images/PineappleAnimation1.png", true);
        m_iDrawWidth = 26;
        m_iDrawHeight = 40;
        m_scoreValue = 30;
        break;
    case IngredientType::Bomb:
        m_baseImage = ImageManager::loadImage("../Images/BombAnimation1.png", true);
        m_iDrawWidth = 50;
        m_iDrawHeight = 50;
        m_scoreValue = -20;
        m_flightPattern = FlightPattern::Straight;
        break;
    }
}
void Psykg5Flyer::virtDraw()
{
    //bound checking
    if (m_isDestroyed || m_iCurrentScreenX < -m_iDrawWidth ||
        m_iCurrentScreenY < -m_iDrawHeight ||
        m_iCurrentScreenX > getEngine()->getWindowWidth() ||
        m_iCurrentScreenY > getEngine()->getWindowHeight())
        return;

	//use Psykg5GameEngine
    Psykg5GameEngine* engine = dynamic_cast<Psykg5GameEngine*>(getEngine());

    //bomb with effect?
    if (m_isWaveEffectActive && m_ingredientType == IngredientType::Bomb && engine) {
        Psykg5WavePixelMapping* waveMapping = engine->getWaveMapping();

        if (waveMapping) {
            //if animating draw frame
            if (m_isAnimating && !m_animationFrames.empty()) {
                m_animationFrames[m_currentAnimationFrame].renderImageApplyingMapping(
                    getEngine(),getEngine()->getForegroundSurface(),
                    m_iCurrentScreenX, m_iCurrentScreenY,
                    m_iDrawWidth, m_iDrawHeight, waveMapping);
            }
            else {
                //if not draw base
                m_baseImage.renderImageApplyingMapping(
                    getEngine(),getEngine()->getForegroundSurface(),
                    m_iCurrentScreenX, m_iCurrentScreenY,
                    m_iDrawWidth, m_iDrawHeight, waveMapping);
            }
            return;
        }
    }
    //normal animation rendering
    if (m_isAnimating && !m_animationFrames.empty()) {
        m_animationFrames[m_currentAnimationFrame].renderImageWithMask(
            getEngine()->getForegroundSurface(),
            0, 0, m_iCurrentScreenX, m_iCurrentScreenY, m_iDrawWidth, m_iDrawHeight);
    }
    //base images
    else {
        m_baseImage.renderImageWithMask(
            getEngine()->getForegroundSurface(),
            0, 0, m_iCurrentScreenX, m_iCurrentScreenY, m_iDrawWidth, m_iDrawHeight);
    }
}

void Psykg5Flyer::virtDoUpdate(int iCurrentTime)
{
    if (getEngine()->isPaused() || m_isDestroyed)
        return;

    int oldX = m_iCurrentScreenX;
    int oldY = m_iCurrentScreenY;

    updateAnimation(iCurrentTime);

    if (m_isBouncing) {
        m_iCurrentScreenX += static_cast<int>(m_velocityX);
        m_iCurrentScreenY += static_cast<int>(m_velocityY);

        //wall bounce
        if (m_iCurrentScreenX <= 0 || m_iCurrentScreenX >= getEngine()->getWindowWidth() - m_iDrawWidth) {
            m_velocityX = -m_velocityX * 0.8f;
            //bound checking
            if (m_iCurrentScreenX < 0) m_iCurrentScreenX = 0;
            if (m_iCurrentScreenX > getEngine()->getWindowWidth() - m_iDrawWidth)
                m_iCurrentScreenX = getEngine()->getWindowWidth() - m_iDrawWidth;
        }
    }
    else {
        updateMovement(iCurrentTime);
    }
    if (m_iCurrentScreenY > 750) {
        resetFlyer();
    }
    if (oldX != m_iCurrentScreenX || oldY != m_iCurrentScreenY || m_isAnimating) {
        redrawRectangle();
    }
}

void Psykg5Flyer::resetFlyer()
{
    m_iCurrentScreenX = rand() % getEngine()->getWindowWidth();
    m_iCurrentScreenY = -20;
    m_flightPattern = static_cast<FlightPattern>(rand() % 4);
    m_zigzagDirection = (std::rand() % 2 == 0) ? 1 : -1;
    m_zigzagWidth = 40 + (std::rand() % 40);
    m_circleAngle = std::rand() % 360;
    m_circleRadius = 15 + (std::rand() % 20);
    calculateArrivalPath();
    getEngine()->drawableObjectsChanged();
}

void Psykg5Flyer::updateMovement(int iCurrentTime)
{
    switch (m_flightPattern) {
    case FlightPattern::Straight:
        moveStraight();
        break;
    case FlightPattern::Zigzag:
        moveZigzag();
        break;
    case FlightPattern::Circle:
        moveCircle();
        break;
    case FlightPattern::Homing:
        moveHoming();
        break;
    }
}

void Psykg5Flyer::moveStraight()
{
    if (m_ingredientType == IngredientType::Bomb) {
        m_iCurrentScreenY += static_cast<int>(m_baseSpeed);
        return;
    }
    float deltaX = static_cast<float>(targetX - m_iCurrentScreenX);
    float deltaY = static_cast<float>(targetY - m_iCurrentScreenY);
    float distance = sqrt(deltaX * deltaX + deltaY * deltaY);

    if (distance > 0) {
        float dirX = deltaX / distance;
        float dirY = deltaY / distance;
        float moveX = dirX * m_baseSpeed * 0.5f;
        m_iCurrentScreenX += static_cast<int>(moveX);
        m_iCurrentScreenY += static_cast<int>(m_baseSpeed);
    }
    else {
        m_iCurrentScreenY += static_cast<int>(m_baseSpeed);
    }
}

void Psykg5Flyer::moveZigzag()
{
    m_iCurrentScreenX += static_cast<int>(m_zigzagDirection * 4.0f);
    m_iCurrentScreenY += static_cast<int>(m_baseSpeed * 0.6f);
    int distanceToTarget = targetY - m_iCurrentScreenY;
    if (distanceToTarget > 0) {
        float timeToImpact = distanceToTarget / (m_baseSpeed * 0.6f);
        int targetDeltaX = targetX - m_iCurrentScreenX;
        if (abs(targetDeltaX) > timeToImpact * 4.0f) {
            m_zigzagDirection = (targetDeltaX > 0) ? 1 : -1;
        }
    }
    if (m_zigzagDirection > 0 && m_iCurrentScreenX > targetX + m_zigzagWidth) {
        m_zigzagDirection = -1;
    }
    else if (m_zigzagDirection < 0 && m_iCurrentScreenX < targetX - m_zigzagWidth) {
        m_zigzagDirection = 1;
    }
}

void Psykg5Flyer::moveCircle()
{
    m_circleAngle = (m_circleAngle + 5) % 360;
    float radians = m_circleAngle * 3.14159265358979323846f / 180.0f;
    int distanceToTarget = targetY - m_iCurrentScreenY;
    if (distanceToTarget > 0) {
        float timeToImpact = distanceToTarget / m_baseSpeed;
        int centerX = targetX + static_cast<int>(sin(radians) * m_circleRadius);
        int deltaX = centerX - m_iCurrentScreenX;
        if (abs(deltaX) > 2) {
            m_iCurrentScreenX += (deltaX > 0) ? 2 : -2;
        }
    }
    m_iCurrentScreenY += static_cast<int>(m_baseSpeed);
    m_iCurrentScreenX += static_cast<int>(sin(radians) * 3.0f);
}

void Psykg5Flyer::moveHoming()
{
    int deltaX = targetX - m_iCurrentScreenX;
    int deltaY = targetY - m_iCurrentScreenY;
    float distance = sqrt(static_cast<float>(deltaX * deltaX + deltaY * deltaY));
    if (distance > 0) {
        float moveX = (deltaX / distance) * 3.0f;
        float moveY = (deltaY / distance) * 3.0f;
        if (moveY < m_baseSpeed)
            moveY = m_baseSpeed;
        m_iCurrentScreenX += static_cast<int>(moveX);
        m_iCurrentScreenY += static_cast<int>(moveY);
    }
    else {
        m_iCurrentScreenY += static_cast<int>(m_baseSpeed);
    }
}

void Psykg5Flyer::calculateArrivalPath()
{
    int verticalDistance = targetY - m_iCurrentScreenY;
    if (verticalDistance <= 0) return; //early exit (at target)

    float timeToTarget = verticalDistance / m_baseSpeed;
    switch (m_flightPattern) {
    case FlightPattern::Straight:
        break;
    case FlightPattern::Zigzag:
        m_zigzagWidth = std::min(m_zigzagWidth, abs(targetX - m_iCurrentScreenX) * 2);
        break;
    case FlightPattern::Circle:
        m_circleRadius = std::min(m_circleRadius, abs(targetX - m_iCurrentScreenX) / 2);
        break;
    case FlightPattern::Homing:
        break;
    }
}

bool Psykg5Flyer::isCollided(int x, int y, int width, int height)
{
    return isCollidedWith(x, y, width, height);
}

bool Psykg5Flyer::checkPixelPerfectCollision(int shieldX, int shieldY, int shieldWidth, int shieldHeight)
{
    if (!isCollidedWith(shieldX, shieldY, shieldWidth, shieldHeight))
        return false;
    int overlapLeft = std::max(m_iCurrentScreenX, shieldX);
    int overlapTop = std::max(m_iCurrentScreenY, shieldY);
    int overlapRight = std::min(m_iCurrentScreenX + m_iDrawWidth, shieldX + shieldWidth);
    int overlapBottom = std::min(m_iCurrentScreenY + m_iDrawHeight, shieldY + shieldHeight);

    DrawingSurface* surface = getEngine()->getForegroundSurface();

    const int stride = 2; //every second pixel for performance
    for (int y = overlapTop; y < overlapBottom; y += stride) {
        for (int x = overlapLeft; x < overlapRight; x += stride) {
            //image-relative coords
            int flyerRelX = x - m_iCurrentScreenX;
            int flyerRelY = y - m_iCurrentScreenY;
            int shieldRelX = x - shieldX;
            int shieldRelY = y - shieldY;
            //skip transparent
            if (flyerRelX < 0 || flyerRelX >= m_iDrawWidth || flyerRelY < 0 || flyerRelY >= m_iDrawHeight)
                continue;
            if (shieldRelX < 0 || shieldRelX >= shieldWidth || shieldRelY < 0 || shieldRelY >= shieldHeight)
                continue;
            //collision if 2 non-transparent overlap
            if (m_baseImage.getPixelColour(flyerRelX, flyerRelY) != 0 &&
                surface->rawGetPixel(x, y) != 0) {
                return true;
            }
        }
    }
    return false;
}

void Psykg5Flyer::destroy()
{
    if (!m_isDestroyed) {
        m_isDestroyed = true;
        redrawRectangle();
        getEngine()->drawableObjectsChanged();
    }
}

void Psykg5Flyer::setupAnimations()
{
    std::vector<SimpleImage> frames;

    switch (m_ingredientType) {
    case IngredientType::Strawberry:
        frames.push_back(ImageManager::loadImage("../Images/StrawberryAnimation1.png", true));
        frames.push_back(ImageManager::loadImage("../Images/StrawberryAnimation2.png", true));
        frames.push_back(ImageManager::loadImage("../Images/StrawberryAnimation3.png", true));
        frames.push_back(ImageManager::loadImage("../Images/StrawberryAnimation4.png", true));
        frames.push_back(ImageManager::loadImage("../Images/StrawberryAnimation5.png", true));
        m_frameDuration = 120;
        break;

    case IngredientType::Banana:
        frames.push_back(ImageManager::loadImage("../Images/BananaAnimation1.png", true));
        frames.push_back(ImageManager::loadImage("../Images/BananaAnimation2.png", true));
        m_frameDuration = 90;
        break;

    case IngredientType::Pineapple:
        frames.push_back(ImageManager::loadImage("../Images/PineappleAnimation1.png", true));
        frames.push_back(ImageManager::loadImage("../Images/PineappleAnimation2.png", true));
        frames.push_back(ImageManager::loadImage("../Images/PineappleAnimation3.png", true));
        frames.push_back(ImageManager::loadImage("../Images/PineappleAnimation4.png", true));
        frames.push_back(ImageManager::loadImage("../Images/PineappleAnimation5.png", true));
        frames.push_back(ImageManager::loadImage("../Images/PineappleAnimation6.png", true));
        m_frameDuration = 150;
        break;

    case IngredientType::Bomb:
        frames.push_back(ImageManager::loadImage("../Images/BombAnimation1.png", true));
        frames.push_back(ImageManager::loadImage("../Images/BombAnimation2.png", true));
        frames.push_back(ImageManager::loadImage("../Images/BombAnimation3.png", true));
        frames.push_back(ImageManager::loadImage("../Images/BombAnimation4.png", true));
        m_frameDuration = 100;
        break;
    }

    setAnimationFrames(frames);
}
void Psykg5Flyer::updateAnimation(int iCurrentTime)
{
    if (!m_isAnimating || m_animationFrames.empty())
        return;

    if (iCurrentTime > m_lastAnimationTime + m_frameDuration) {
        m_lastAnimationTime = iCurrentTime;
        m_currentAnimationFrame++;

        if (m_currentAnimationFrame >= m_animationFrames.size()) {
            if (m_loopAnimation)
                m_currentAnimationFrame = 0;
            else {
                m_currentAnimationFrame = m_animationFrames.size() - 1;
                m_isAnimating = false;
            }
        }
        redrawRectangle();
    }
}

void Psykg5Flyer::setAnimationFrames(const std::vector<SimpleImage>& frames)
{
    m_animationFrames = frames;
    m_currentAnimationFrame = 0;
}

void Psykg5Flyer::stopAnimation()
{
    m_isAnimating = false;
}

void Psykg5Flyer::playAnimation(bool loop)
{
    if (m_animationFrames.size() == 0)
        return;
    m_isAnimating = true;
    m_loopAnimation = loop;
    m_currentAnimationFrame = 0;
    m_lastAnimationTime = getEngine()->getModifiedTime();
}