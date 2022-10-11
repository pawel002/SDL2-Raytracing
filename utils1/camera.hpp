#ifndef CAMERA_H
#define CAMERA_H

#include "vec3.hpp"
#include "ray.hpp"

class camera {
    public:
        camera(double vfov, double aspect_ratio, point3 look_from, point3 look_at, vec3 vup) {
            auto theta = vfov * M_PI / 180;
            auto h = tan(theta/2);
            auto viewport_height = 2.0 * h;
            auto viewport_width = aspect_ratio * viewport_height;

            auto focal_length = 1.0;

            auto w = unit_vector(look_from - look_at);
            auto u = unit_vector(cross(vup, w));
            auto v = cross(w, u);

            origin = look_from;
            horizontal = viewport_width * u;
            vertical = viewport_height * v;
            lower_left_corner = origin - horizontal/2 - vertical/2 - w;
        }

        ray get_ray(double u, double v) const {
            return ray(origin, lower_left_corner + u*horizontal + v*vertical - origin);
        }

    private:
        point3 origin;
        point3 lower_left_corner;
        vec3 horizontal;
        vec3 vertical;
};


#endif