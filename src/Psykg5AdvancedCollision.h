#pragma once
#include "SimpleImage.h"
#include "BaseEngine.h"
#include <vector>

class Psykg5AdvancedCollision
{
public:
    Psykg5AdvancedCollision() = delete;

    // Simple rectangle collision detection
    static bool checkRectangles(int x1l, int x1r, int y1t, int y1b, int x2l, int x2r, int y2t, int y2b)
    {
        return !((x1r < x2l) || (x2r < x1l) || (y1b < y2t) || (y2b < y1t));
    }

    // Simple circle collision detection
    static bool checkCircles(int x1c, int y1c, int x2c, int y2c, int gap)
    {
        return ((x1c - x2c) * (x1c - x2c) + (y1c - y2c) * (y1c - y2c)) < (gap * gap);
    }

    // Pixel-perfect collision detection between two images
    static bool checkPixelCollision(
        SimpleImage& image1, int x1, int y1, int width1, int height1,
        SimpleImage& image2, int x2, int y2, int width2, int height2,
        int transparencyColor1 = -1, int transparencyColor2 = -1)
    {
        // First do a quick bounding box check
        if (!checkRectangles(x1, x1 + width1, y1, y1 + height1, x2, x2 + width2, y2, y2 + height2))
            return false;

        int overlapLeft = std::max(x1, x2);
        int overlapTop = std::max(y1, y2);
        int overlapRight = std::min(x1 + width1, x2 + width2);
        int overlapBottom = std::min(y1 + height1, y2 + height2);

        //Pixel level checking
        for (int y = overlapTop; y < overlapBottom; y++) {
            for (int x = overlapLeft; x < overlapRight; x++) {
                int localX1 = x - x1;
                int localY1 = y - y1;
                int localX2 = x - x2;
                int localY2 = y - y2;

                int color1 = image1.getPixelColour(localX1, localY1);
                int color2 = image2.getPixelColour(localX2, localY2);

                bool pixel1Transparent = (transparencyColor1 != -1) ? (color1 == transparencyColor1) : false;
                bool pixel2Transparent = (transparencyColor2 != -1) ? (color2 == transparencyColor2) : false;

                if (!pixel1Transparent && !pixel2Transparent) {
                    return true;
                }
            }
        }

        return false;
    }

    // Polygon-based collision detection
    static bool checkConcavePolygonCollision(
        const std::vector<std::pair<int, int>>& polygon1,
        const std::vector<std::pair<int, int>>& polygon2)
    {
        //Form ears
        auto triangles1 = decomposePolygon(polygon1);
        auto triangles2 = decomposePolygon(polygon2);

        for (const auto& tri1 : triangles1) {
            for (const auto& tri2 : triangles2) {
                //Triangle collision with ears
                if (checkTriangleCollision(tri1, tri2)) {
                    return true;
                }
            }
        }

        return false;
    }

private:
    static std::vector<std::vector<std::pair<int, int>>> decomposePolygon(
        const std::vector<std::pair<int, int>>& polygon)
    {
        std::vector<std::vector<std::pair<int, int>>> triangles;

        if (polygon.size() < 3)
            return triangles;

        std::vector<std::pair<int, int>> remaining = polygon;

        while (remaining.size() > 3) {
            bool earFound = false;

            for (size_t i = 0; i < remaining.size(); ++i) {
                if (isEar(remaining, i)) {
                    size_t prev = (i + remaining.size() - 1) % remaining.size();
                    size_t next = (i + 1) % remaining.size();
                    triangles.push_back({ remaining[prev], remaining[i], remaining[next] });
                    remaining.erase(remaining.begin() + i);
                    earFound = true;
                    break;
                }
            }

            if (!earFound) {
                if (remaining.size() >= 3) {
                    for (size_t i = 1; i < remaining.size() - 1; ++i) {
                        triangles.push_back({ remaining[0], remaining[i], remaining[i + 1] });
                    }
                }
                break;
            }
        }

        if (remaining.size() == 3)
            triangles.push_back(remaining);

        return triangles;
    }

	//error checking
    static bool isEar(const std::vector<std::pair<int, int>>& polygon, size_t index)
    {
        size_t prev = (index + polygon.size() - 1) % polygon.size();
        size_t next = (index + 1) % polygon.size();

        const auto& a = polygon[prev];
        const auto& b = polygon[index];
        const auto& c = polygon[next];

        if (direction(a, b, c) <= 0) return false;

        for (size_t i = 0; i < polygon.size(); ++i) {
            if (i == prev || i == index || i == next) continue;
            if (isPointInTriangle(polygon[i], a, b, c)) return false;
        }

        return true;
    }

    static bool isPointInTriangle(
        const std::pair<int, int>& pt,
        const std::pair<int, int>& v1,
        const std::pair<int, int>& v2,
        const std::pair<int, int>& v3)
    {
        float d1 = sign(pt, v1, v2);
        float d2 = sign(pt, v2, v3);
        float d3 = sign(pt, v3, v1);

        bool hasNeg = (d1 < 0) || (d2 < 0) || (d3 < 0);
        bool hasPos = (d1 > 0) || (d2 > 0) || (d3 > 0);

        return !(hasNeg && hasPos);
    }

    static float sign(const std::pair<int, int>& p1,
        const std::pair<int, int>& p2,
        const std::pair<int, int>& p3)
    {
        return static_cast<float>((p1.first - p3.first) * (p2.second - p3.second) -
            (p2.first - p3.first) * (p1.second - p3.second));
    }

    static bool checkTriangleCollision(
        const std::vector<std::pair<int, int>>& tri1,
        const std::vector<std::pair<int, int>>& tri2)
    {
        for (const auto& vertex : tri1)
            if (isPointInTriangle(vertex, tri2[0], tri2[1], tri2[2]))
                return true;

        for (const auto& vertex : tri2)
            if (isPointInTriangle(vertex, tri1[0], tri1[1], tri1[2]))
                return true;

        for (size_t i = 0; i < 3; ++i) {
            size_t i2 = (i + 1) % 3;
            for (size_t j = 0; j < 3; ++j) {
                size_t j2 = (j + 1) % 3;
                if (doLinesIntersect(tri1[i], tri1[i2], tri2[j], tri2[j2]))
                    return true;
            }
        }

        return false;
    }

    static bool doLinesIntersect(
        const std::pair<int, int>& p1, const std::pair<int, int>& p2,
        const std::pair<int, int>& p3, const std::pair<int, int>& p4)
    {
        int d1 = direction(p3, p4, p1);
        int d2 = direction(p3, p4, p2);
        int d3 = direction(p1, p2, p3);
        int d4 = direction(p1, p2, p4);

        if (((d1 > 0 && d2 < 0) || (d1 < 0 && d2 > 0)) &&
            ((d3 > 0 && d4 < 0) || (d3 < 0 && d4 > 0))) {
            return true;
        }

        if (d1 == 0 && onSegment(p3, p4, p1)) return true;
        if (d2 == 0 && onSegment(p3, p4, p2)) return true;
        if (d3 == 0 && onSegment(p1, p2, p3)) return true;
        if (d4 == 0 && onSegment(p1, p2, p4)) return true;

        return false;
    }

    static int direction(
        const std::pair<int, int>& p1,
        const std::pair<int, int>& p2,
        const std::pair<int, int>& p3)
    {
        return (p3.first - p1.first) * (p2.second - p1.second) -
            (p2.first - p1.first) * (p3.second - p1.second);
    }

    static bool onSegment(
        const std::pair<int, int>& p1,
        const std::pair<int, int>& p2,
        const std::pair<int, int>& p3)
    {
        return p3.first <= std::max(p1.first, p2.first) &&
            p3.first >= std::min(p1.first, p2.first) &&
            p3.second <= std::max(p1.second, p2.second) &&
            p3.second >= std::min(p1.second, p2.second);
    }
};