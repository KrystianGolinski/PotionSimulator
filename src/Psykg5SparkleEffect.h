#pragma once
#include "DisplayableObject.h"
#include "SimpleImage.h"
#include <vector>

class Psykg5SparkleEffect : public DisplayableObject
{
public:
    Psykg5SparkleEffect(BaseEngine* pEngine, int x, int y);
    virtual ~Psykg5SparkleEffect() = default;
    virtual void virtDraw() override;
    virtual void virtDoUpdate(int iCurrentTime) override;
    bool isFinished() const { return m_isFinished; }
private:
    std::vector<SimpleImage> m_frames;
    int m_currentFrame;
    int m_frameDelay;
    int m_lastFrameTime;
    bool m_isFinished;
};