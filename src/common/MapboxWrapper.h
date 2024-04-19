#pragma once

#include "mapbox/earcut.hpp"
#include "mapbox/polylabel.hpp"

namespace mapbox {
namespace util {

template <>
struct nth<0, Vector2> {
    inline static auto get(const Vector2 &t) {
        return t.x;
    };
};
template <>
struct nth<1, Vector2> {
    inline static auto get(const Vector2 &t) {
        return t.y;
    };
};

} // namespace util
} // namespace mapbox