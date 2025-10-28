#ifndef PSYKG5FLYER_H
#define PSYKG5FLYER_H
#include "Psykg5GameObject.h"

enum class IngredientType { Banana, Strawberry, Pineapple, Bomb };
enum class FlightPattern { Straight, Zigzag, Circle, Homing };

class Psykg5Flyer : public Psykg5GameObject
{
public:
    Psykg5Flyer(BaseEngine* pEngine, int startX, int startY);
    void virtDraw() override;
    void virtDoUpdate(int iCurrentTime) override;
    bool checkPixelPerfectCollision(int shieldX, int shieldY, int shieldWidth, int shieldHeight);
    bool isCollided(int x, int y, int width, int height);
    IngredientType getIngredientType() const { return m_ingredientType; }
    int getScoreValue() const { return m_scoreValue; }
    bool isDestroyed() const { return m_isDestroyed; }
    void destroy();
    void SetFlightPattern(FlightPattern pattern) { m_flightPattern = pattern; }
    void SetYPosition(int y) { m_iCurrentScreenY = y; }
    void SetXPosition(int x) { m_iCurrentScreenX = x; }
    void createSpecificIngredient(IngredientType type);
    bool isWaveEffectActive() const { return m_isWaveEffectActive; }
    bool isBouncing() const { return m_isBouncing; }
    void setBouncing(bool bouncing) { m_isBouncing = bouncing; }
    void setWaveEffectActive(bool active) { m_isWaveEffectActive = active; }
private:
    int targetX;
    int targetY;
    FlightPattern m_flightPattern = FlightPattern::Straight;
    IngredientType m_ingredientType = IngredientType::Banana;
    int m_circleAngle = 0;
    int m_zigzagDirection = 1;
    int m_zigzagWidth = 30;
    int m_circleRadius = 20;
    bool m_isDestroyed = false;
    int m_scoreValue = 10;
    float m_baseSpeed = 1.0f;
    bool m_isAnimating = false;
    bool m_loopAnimation = false;
    int m_currentAnimationFrame = 0;
    int m_lastAnimationTime = 0;
    int m_frameDuration = 100;
    std::vector<SimpleImage> m_animationFrames;
    void updateMovement(int iCurrentTime);
    void moveStraight();
    void moveZigzag();
    void moveCircle();
    void moveHoming();
    void calculateArrivalPath();
    void resetFlyer();
    void setupAnimations();
    void updateAnimation(int iCurrentTime);
    void setAnimationFrames(const std::vector<SimpleImage>& frames);
    void stopAnimation();
    void playAnimation(bool loop);
    bool m_isWaveEffectActive = false;
    bool m_isBouncing = false;
};
#endif