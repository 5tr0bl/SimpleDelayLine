/*
  ==============================================================================

    Vec2.h
    Created: 5 Dec 2023 4:22:46pm
    Author:  Micha

  ==============================================================================
*/

#pragma once
#include <cmath>

class Vec2 {
public:
    float x, y;

    Vec2() { x = 0; y = 0; }
    Vec2(float x, float y) : x(x), y(y) {}

    Vec2 operator+(const Vec2& other) const {
        return Vec2(x + other.x, y + other.y);
    }

    Vec2 operator-(const Vec2& other) const {
        return Vec2(x - other.x, y - other.y);
    }

    float distanceTo(const Vec2& other) const {
        return std::sqrt(std::pow(other.x - x, 2) + std::pow(other.y - y, 2));
    }

    float angleBetween(const Vec2& a, const Vec2& b) {
        float dotProduct = a.x * b.x + a.y * b.y;

        float magnitudeA = std::sqrt(a.x * a.x + a.y * a.y);
        float magnitudeB = std::sqrt(b.x * b.x + b.y * b.y);

        float cosTheta = dotProduct / (magnitudeA * magnitudeB);
        cosTheta = std::clamp(cosTheta, -1.0f, 1.0f); // Ensure that cosTheta is within the valid range [-1, 1]

        // angle in radians
        return std::acos(cosTheta);
    }
};
