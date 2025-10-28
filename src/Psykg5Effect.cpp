#include "Psykg5Effect.h"

Psykg5Effect::Psykg5Effect(BaseEngine* pEngine, int x, int y, int width, int height)
    : Psykg5GameObject(pEngine, x, y, width, height)
{
    setVelocity(0.0f, 0.0f);
}

void Psykg5Effect::virtDoUpdate(int iCurrentTime)
{
    Psykg5GameObject::virtDoUpdate(iCurrentTime);
    if (!m_isAnimating && !m_loopAnimation) {
        m_isFinished = true;
    }
    redrawRectangle();
}