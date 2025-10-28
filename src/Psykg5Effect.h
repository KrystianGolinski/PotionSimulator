#pragma once
#include "Psykg5GameObject.h"

class Psykg5Effect : public Psykg5GameObject {
public:
    Psykg5Effect(BaseEngine* pEngine, int x, int y, int width, int height);
    virtual void virtDoUpdate(int iCurrentTime) override;
    bool isFinished() const { return m_isFinished; }

protected:
    bool m_isFinished = false;
};