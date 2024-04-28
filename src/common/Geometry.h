#pragma once

#include "raylib.h"

#include <vector>

namespace swarmgame
{
namespace common
{
template <typename T>
struct Triangle
{
    // Vertices specified in counter-clockwise manner
    T v0, v1, v2;
};

std::vector<Triangle<Vector2>> triangulate(const std::vector<Vector2>& polygon);

// Computes the visual center of polygon
Vector2 getCentroidConcave(const std::vector<Vector2>& polygon);

void sortVerticesClockwise(std::vector<Vector2>& polygon);
void sortVerticesCounterclockwise(std::vector<Vector2>& polygon);

std::tuple<float, Vector3> getAxisAngle(const Vector3& a, const Vector3& b);
}
}