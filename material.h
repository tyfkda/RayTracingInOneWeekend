#pragma once

#include "hittable.h"
#include "ray.h"

inline vec3 reflect(const vec3& v, const vec3& n) {
  return v - 2 * dot(v, n) * n;
}

class material {
 public:
  virtual bool scatter(
      const ray& r_in, const hit_record& rec, vec3& attenuation, ray& scattered
  ) const = 0;
};

class lambertian : public material {
 public:
  lambertian(const vec3& a) : albedo(a) {}

  virtual bool scatter(
      const ray& r_in, const hit_record& rec, vec3& attenuation, ray& scattered
  ) const override {
    vec3 scatter_direction = rec.normal + random_unit_vector();
    scattered = ray(rec.p, scatter_direction);
    attenuation = albedo;
    return true;
  }

 public:
  color albedo;
};

class metal : public material {
 public:
  metal(const vec3& a, double f) : albedo(a), fuzz(f < 1 ? f : 1) {}

  virtual bool scatter(
      const ray& r_in, const hit_record& rec, vec3& attenuation, ray& scattered
  ) const override {
    vec3 reflected = reflect(unit_vector(r_in.direction()), rec.normal);
    scattered = ray(rec.p, reflected + fuzz * random_in_unit_sphere());
    attenuation = albedo;
    return (dot(scattered.direction(), rec.normal) > 0);
  }

 public:
  vec3 albedo;
  double fuzz;
};
