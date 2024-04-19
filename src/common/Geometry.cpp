#include "Geometry.h"

#include "common/MapboxWrapper.h"

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
}
}