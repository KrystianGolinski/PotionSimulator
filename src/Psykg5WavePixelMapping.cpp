#include "Psykg5WavePixelMapping.h"
#include "DrawingSurface.h"

Psykg5WavePixelMapping::Psykg5WavePixelMapping(double amplitude, double frequency, double speed)
    : m_amplitude(amplitude), m_frequency(frequency), m_speed(speed), m_time(0.0)
{
}

bool Psykg5WavePixelMapping::mapCoordinates(double& x, double& y, const SimpleImage& image)
{
    //bounds check
    if (x < 0 || y < 0 || x >= image.getWidth() || y >= image.getHeight())
        return false;

    double originalX = x;
    double originalY = y;

    //apply wave to y given x for wave effect
    double offsetY = m_amplitude * sin((originalX * m_frequency) + (m_time * m_speed));

    //x distortion given y for increased effect
    double offsetX = (m_amplitude * 0.3) * cos((originalY * m_frequency * 0.5) + (m_time * m_speed * 0.8));
    x += offsetX;
    y += offsetY;

    //bounds check
    if (x < 0 || y < 0 || x >= image.getWidth() || y >= image.getHeight())
        return false;

    return true;
}