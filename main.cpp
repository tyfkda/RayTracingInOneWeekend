#include "rtweekend.h"

#include "camera.h"
#include "color.h"
#include "hittable_list.h"
#include "material.h"
#include "sphere.h"
#include "moving_sphere.h"
#include "bvh.h"
#include "aarect.h"
#include "box.h"
#include "constant_medium.h"

#include <iostream>

color ray_color(const ray& r, const color& background, const hittable& world, int depth) {
  hit_record rec;
  // If we've exceeded the ray bounce limit, no more light is gathered.
  if (depth <= 0)
    return color(0, 0, 0);

  if (!world.hit(r, 0.001, infinity, rec))
    return background;

  ray scattered;
  color attenuation;
  color emitted = rec.mat_ptr->emitted(rec.u, rec.v, rec.p);

  if (!rec.mat_ptr->scatter(r, rec, attenuation, scattered))
    return emitted;

  return emitted + attenuation * ray_color(scattered, background, world, depth - 1);
}

bvh_node random_scene() {
  std::vector<shared_ptr<hittable>> objects;

  auto ground_material = make_shared<lambertian>(make_shared<solid_color>(0.5, 0.5, 0.5));
  auto checker = make_shared<checker_texture>(
      make_shared<solid_color>(0.2, 0.3, 0.1),
      make_shared<solid_color>(0.9, 0.9, 0.9));
  objects.push_back(make_shared<sphere>(point3(0,-1000,0), 1000, make_shared<lambertian>(checker)));

  for (int a = -11; a < 11; a++) {
    for (int b = -11; b < 11; b++) {
      auto choose_mat = random_double();
      point3 center(a + 0.9*random_double(), 0.2, b + 0.9*random_double());

      if ((center - point3(4, 0.2, 0)).length() > 0.9) {
        shared_ptr<material> sphere_material;

        if (choose_mat < 0.8) {
          // diffuse
          auto albedo = make_shared<solid_color>(color::random() * color::random());
          sphere_material = make_shared<lambertian>(albedo);
          auto center2 = center + vec3(0, random_double(0, .5), 0);
          objects.push_back(make_shared<moving_sphere>(
              center, center2, 0.0, 1.0, 0.2, sphere_material));
        } else if (choose_mat < 0.95) {
          // metal
          auto albedo = color::random(0.5, 1);
          auto fuzz = random_double(0, 0.5);
          sphere_material = make_shared<metal>(albedo, fuzz);
          objects.push_back(make_shared<sphere>(center, 0.2, sphere_material));
        } else {
          // glass
          sphere_material = make_shared<dielectric>(1.5);
          objects.push_back(make_shared<sphere>(center, 0.2, sphere_material));
        }
      }
    }
  }

  auto material1 = make_shared<dielectric>(1.5);
  objects.push_back(make_shared<sphere>(point3(0, 1, 0), 1.0, material1));

  auto material2 = make_shared<lambertian>(make_shared<solid_color>(0.4, 0.2, 0.1));
  objects.push_back(make_shared<sphere>(point3(-4, 1, 0), 1.0, material2));

  auto material3 = make_shared<metal>(color(0.7, 0.6, 0.5), 0.0);
  objects.push_back(make_shared<sphere>(point3(4, 1, 0), 1.0, material3));

  return bvh_node(objects, 0, objects.size(), 0, 1);
}

hittable_list two_perlin_spheres() {
  hittable_list objects;

  auto pertext = make_shared<noise_texture>(4);
  objects.add(make_shared<sphere>(point3(0,-1000,0), 1000, make_shared<lambertian>(pertext)));
  objects.add(make_shared<sphere>(point3(0, 2, 0), 2, make_shared<lambertian>(pertext)));

  return objects;
}

hittable_list earth() {
  auto earth_texture = make_shared<image_texture>("earthmap.jpg");
  auto earth_surface = make_shared<lambertian>(earth_texture);
  auto globe = make_shared<sphere>(point3(0,0,0), 2, earth_surface);

  return hittable_list(globe);
}

hittable_list simple_light() {
  hittable_list objects;

  auto pertext = make_shared<noise_texture>(4);
  objects.add(make_shared<sphere>(point3(0,-1000,0), 1000, make_shared<lambertian>(pertext)));
  objects.add(make_shared<sphere>(point3(0,2,0), 2, make_shared<lambertian>(pertext)));

  auto difflight = make_shared<diffuse_light>(make_shared<solid_color>(4,4,4));
  objects.add(make_shared<sphere>(point3(0,7,0), 2, difflight));
  objects.add(make_shared<xy_rect>(3, 5, 1, 3, -2, difflight));

  return objects;
}

hittable_list cornell_box() {
  hittable_list objects;

  auto red   = make_shared<lambertian>(make_shared<solid_color>(.65, .05, .05));
  auto white = make_shared<lambertian>(make_shared<solid_color>(.73, .73, .73));
  auto green = make_shared<lambertian>(make_shared<solid_color>(.12, .45, .15));
  auto light = make_shared<diffuse_light>(make_shared<solid_color>(15, 15, 15));

  objects.add(make_shared<flip_face>(make_shared<yz_rect>(0, 555, 0, 555, 555, green)));
  objects.add(make_shared<yz_rect>(0, 555, 0, 555, 0, red));
  objects.add(make_shared<xz_rect>(213, 343, 227, 332, 554, light));
  objects.add(make_shared<flip_face>(make_shared<xz_rect>(0, 555, 0, 555, 555, white)));
  objects.add(make_shared<xz_rect>(0, 555, 0, 555, 0, white));
  objects.add(make_shared<flip_face>(make_shared<xy_rect>(0, 555, 0, 555, 555, white)));

  shared_ptr<hittable> box1 = make_shared<box>(point3(0, 0, 0), point3(165, 330, 165), white);
  box1 = make_shared<rotate_y>(box1,  15);
  box1 = make_shared<translate>(box1, vec3(265,0,295));
  objects.add(box1);

  shared_ptr<hittable> box2 = make_shared<box>(point3(0,0,0), point3(165,165,165), white);
  box2 = make_shared<rotate_y>(box2, -18);
  box2 = make_shared<translate>(box2, vec3(130,0,65));
  objects.add(box2);

  return objects;
}

hittable_list cornell_smoke() {
  hittable_list objects;

  auto red   = make_shared<lambertian>(make_shared<solid_color>(.65, .05, .05));
  auto white = make_shared<lambertian>(make_shared<solid_color>(.73, .73, .73));
  auto green = make_shared<lambertian>(make_shared<solid_color>(.12, .45, .15));
  auto light = make_shared<diffuse_light>(make_shared<solid_color>(7, 7, 7));

  objects.add(make_shared<flip_face>(make_shared<yz_rect>(0, 555, 0, 555, 555, green)));
  objects.add(make_shared<yz_rect>(0, 555, 0, 555, 0, red));
  objects.add(make_shared<xz_rect>(113, 443, 127, 432, 554, light));
  objects.add(make_shared<flip_face>(make_shared<xz_rect>(0, 555, 0, 555, 555, white)));
  objects.add(make_shared<xz_rect>(0, 555, 0, 555, 0, white));
  objects.add(make_shared<flip_face>(make_shared<xy_rect>(0, 555, 0, 555, 555, white)));

  shared_ptr<hittable> box1 = make_shared<box>(point3(0,0,0), point3(165,330,165), white);
  box1 = make_shared<rotate_y>(box1,  15);
  box1 = make_shared<translate>(box1, vec3(265,0,295));

  shared_ptr<hittable> box2 = make_shared<box>(point3(0,0,0), point3(165,165,165), white);
  box2 = make_shared<rotate_y>(box2, -18);
  box2 = make_shared<translate>(box2, vec3(130,0,65));

  objects.add(make_shared<constant_medium>(box1, 0.01, make_shared<solid_color>(0,0,0)));
  objects.add(make_shared<constant_medium>(box2, 0.01, make_shared<solid_color>(1,1,1)));

  return objects;
}

int main() {
  const auto aspect_ratio = 16.0 / 9.0;
  const int image_width = 512;
  const int image_height = static_cast<int>(image_width / aspect_ratio);
  const int samples_per_pixel = 100;
  const int max_depth = 50;

  std::cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";

  //auto world = random_scene();
  //auto world = two_perlin_spheres();
  //auto world = earth();
  //auto world = simple_light();
  //auto world = cornell_box();
  auto world = cornell_smoke();

  point3 lookfrom(278, 278, -800);
  point3 lookat(278,278,0);
  vec3 vup(0,1,0);
  auto dist_to_focus = 10.0;
  auto aperture = 0.0;
  auto vfov = 40.0;

  camera cam(lookfrom, lookat, vup, vfov, aspect_ratio, aperture, dist_to_focus, 0.0, 1.0);

  for (int j = image_height - 1; j >= 0; --j) {
    std::cerr << "\rScanlines remaining: " << j << ' ' << std::flush;
    for (int i = 0; i < image_width; ++i) {
      color pixel_color(0, 0, 0);
      for (int s = 0; s < samples_per_pixel; ++s) {
        auto u = (i + random_double()) / (image_width-1);
        auto v = (j + random_double()) / (image_height-1);
        ray r = cam.get_ray(u, v);
        pixel_color += ray_color(r, color(0, 0, 0), world, max_depth);
      }
      write_color(std::cout, pixel_color, samples_per_pixel);
    }
  }

  std::cerr << "\nDone.\n";
}
