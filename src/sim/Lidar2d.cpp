#include "sim/Lidar2d.h"

#include "raylib.h"
#include "raymath.h"

#include <iostream>

namespace swarmgame
{
namespace sim
{
std::vector<Vector3> Lidar2d::simulate(
    const int32_t id,
    const Vector3& pos,
    const std::vector<common::Uav>& uavs,
    const std::vector<common::Triangle<Vector3>>& triangles,
    const float angleStep,
    const float maxRange)
{
    std::vector<Vector3> hitPoints;

    for (float a = 0.0f; a < 360.0f; a += angleStep)
    {
        float angle = 90.0f - a; // clockwise direction from front
        Vector3 dir { cos(angle * DEG2RAD), sin(angle * DEG2RAD), 0.0f };
        Ray ray {pos, dir};
        
        float minHitDist = std::numeric_limits<float>::max();
        Vector3 hitPoint = Vector3Add(pos, Vector3Scale(dir, maxRange)); // point at max range
        
        // Check hit with UAVs
        for (const auto& uav: uavs)
        {
            // Ignore self
            if (id == uav.mId)
            {
                continue;
            }

            RayCollision result = GetRayCollisionSphere(ray, uav.getPos(), uav.mRadius);

            if (result.hit && result.distance < minHitDist && result.distance < maxRange)
            {
                minHitDist = result.distance;
                hitPoint = result.point;
            }
        }

        // Check hit with triangles
        for (const auto& triangle : triangles)
        {
            RayCollision result = GetRayCollisionTriangle(ray, triangle.v0, triangle.v1, triangle.v2);

            if (result.hit && result.distance < minHitDist && result.distance < maxRange)
            {
                minHitDist = result.distance;
                hitPoint = result.point;
            }
        }

        // Points are in North-aligned body frame
        // Adds point at max range if no hit from environment
        hitPoints.emplace_back(Vector3Subtract(hitPoint, pos));
    }

    return hitPoints;
}
}
}