#ifndef HITTABLE_H
#define HITTABLE_H

#include "vec3.hpp"
#include "ray.hpp"
#include <memory>

class material;

struct hit_record {
    point3 p;
    vec3 normal;
    std::shared_ptr<material> mat_ptr;
    double t;
    bool front_face;

    inline void set_face_normal(const ray &r, const vec3 &n){
        front_face = dot(r.direction(), n) < 0;
        normal = front_face ? n : -n;
    }
};

class hittable {
    public:
        virtual bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const = 0;
};

#endif