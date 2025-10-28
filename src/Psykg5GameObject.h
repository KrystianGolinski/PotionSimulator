#pragma once
#include "DisplayableObject.h"
#include "SimpleImage.h"
#include <vector>

class Psykg5GameObject : public DisplayableObject {
public:
    Psykg5GameObject(BaseEngine* pEngine, int startX, int startY, int width, int height);
    virtual void virtDoUpdate(int iCurrentTime) override;
    void setAnimationFrames(const std::vector<SimpleImage>& frames, int frameDuration);
    void playAnimation(bool loop = false);
    void stopAnimation();
    void setVelocity(float vx, float vy);
    void applyForce(float fx, float fy);
    struct Velocity { float x; float y; };
    Velocity getVelocity() const { return { m_velocityX, m_velocityY }; }
    bool isCollidedWith(Psykg5GameObject* other) const;
    bool isCollidedWith(int x, int y, int width, int height) const;
    SimpleImage* getBaseImage() { return &m_baseImage; }

protected:
    SimpleImage m_baseImage;
    std::vector<SimpleImage> m_animationFrames;
    int m_currentFrame;
    int m_frameDuration;
    int m_lastFrameTime;
    bool m_isAnimating;
    bool m_loopAnimation;
    float m_velocityX;
    float m_velocityY;
    void updatePhysics(int timeDelta);
    void updateAnimation(int iCurrentTime);
};