#pragma once
#include "SimpleImage.h"
#include "ImagePixelMapping.h"

class DrawingSurface;

class Psykg5WavePixelMapping : public ImagePixelMapping
{
public:
    Psykg5WavePixelMapping(double amplitude = 5.0, double frequency = 0.1, double speed = 0.05);

    virtual bool mapCoordinates(double& x, double& y, const SimpleImage& image) override;
    void updateTime(double timeValue) { m_time = timeValue; }

    //setters getters
    void setAmplitude(double amplitude) { m_amplitude = amplitude; }
    double getAmplitude() const { return m_amplitude; }
    void setFrequency(double frequency) { m_frequency = frequency; }
    double getFrequency() const { return m_frequency; }
    void setSpeed(double speed) { m_speed = speed; }
    double getSpeed() const { return m_speed; }
private:
    double m_amplitude;
    double m_frequency;
    double m_speed;
    double m_time;
};