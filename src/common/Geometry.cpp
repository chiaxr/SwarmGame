#include "Geometry.h"

#include "common/MapboxWrapper.h"

#include "clipper2/clipper.h"
#include "raymath.h"

namespace swarmgame
{
namespace common
{
std::vector<Triangle<Vector2>> triangulate(const std::vector<Vector2>& polygon)
{
    using N = uint16_t; // index type - max number of vertices
    std::vector<std::vector<Vector2>> polylines { polygon };
    std::vector<N> indices = mapbox::earcut<N>(polylines);

    std::vector<Triangle<Vector2>> triangles;
    for (size_t i = 0; i < indices.size(); i += 3)
    {
        triangles.emplace_back(
            polygon.at(indices.at(i)),
            polygon.at(indices.at(i + 1)),
            polygon.at(indices.at(i + 2))
        );
    }
    return triangles;
}

Vector2 getCentroidConcave(const std::vector<Vector2>& polygon)
{
    std::vector<std::vector<Vector2>> polylines { polygon };
    Vector2 centroid = mapbox::polylabel<Vector2, float>(polylines);
    return centroid;
}

void sortVerticesClockwise(std::vector<Vector2>& polygon)
{
    // Find pivot point within polygon
    // Polygon may not be convex
    Vector2 pivot = getCentroidConcave(polygon);
    
    // Translate polygon vertices to origin
    for (Vector2& v : polygon)
    {
        v.x -= pivot.x;
        v.y -= pivot.y;
    }

    auto getClockwiseAngle = [](Vector2 p) 
    {
        return -atan2(p.x, -p.y);
    };

    std::sort(
        polygon.begin(),
        polygon.end(),
        [&](Vector2 p1, Vector2 p2) 
        {
            return getClockwiseAngle(p1) < getClockwiseAngle(p2);
        });

    // Translate polygon vertices back to original location
    for (Vector2& v : polygon)
    {
        v.x += pivot.x;
        v.y += pivot.y;
    }
}

void sortVerticesCounterclockwise(std::vector<Vector2>& polygon)
{
    sortVerticesClockwise(polygon);
    std::reverse(polygon.begin(), polygon.end());
}

std::tuple<float, Vector3> getAxisAngle(const Vector3& a, const Vector3& b)
{
    float angle = 0.0f;
    Vector3 axis = a;

    Vector3 unitA = Vector3Normalize(a);
    Vector3 unitB = Vector3Normalize(b);
    float cosTheta = Vector3DotProduct(unitA, unitB);
    Vector3 crossProd = Vector3CrossProduct(unitA, unitB);

    // Vectors are parallel and in same direction
    if (cosTheta > 1.0f - EPSILON)
    {
        angle = 0.0f;
        axis = unitA;
    }
    // Vectors are parallel and in opposite direction
    else if (cosTheta < -1.0f + EPSILON)
    {
        angle = 180.0f;
        axis = unitA;
    }
    // Vectors are non-parallel
    else
    {
        angle = RAD2DEG * -acos(cosTheta);
        axis = crossProd;
    }

    return {angle, crossProd};
}

bool pointInPolygon(const std::vector<Vector2>& polygon, const Vector2& point)
{
    using namespace Clipper2Lib;

    Point<float> cPoint {point.x, point.y};
    Path<float> cPolygon;
    for (const Vector2& v : polygon)
    {
        cPolygon.emplace_back(v.x, v.y);
    }
    
    PointInPolygonResult result = PointInPolygon(cPoint, cPolygon);
    
    return result == PointInPolygonResult::IsInside; // point on edge of polygon considered outside
}

bool pointInExtrudedPolygon(const std::vector<Vector2>& polygon, const float minHeight, const float maxHeight, const Vector3& point)
{
    bool withinHeights = point.z > minHeight && point.z < maxHeight;
    bool withinPolygon = pointInPolygon(polygon, {point.x, point.y});
    return withinHeights && withinPolygon;
}
}
}