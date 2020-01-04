#pragma once

#include "vec3.h"

class ray {
 public:
  ray() {}
  ray(const point3& origin, const vec3& direction): orig(origin), dir(direction) {}

  const vec3& origin() const  { return orig; }
  const vec3& direction() const  { return dir; }

  const vec3 at(double t) const {
    return orig + t * dir;
  }

 public:
  point3 orig;
  vec3 dir;
};
