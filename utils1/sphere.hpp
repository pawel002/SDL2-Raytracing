#ifndef SPHERE_H
#define SPHERE_H

#include "hittable.hpp"
#include "material.hpp"
#include "vec3.hpp"
#include <memory>

using std::cout, std::endl;

class sphere : public hittable {
    public:
        sphere() {}
        sphere(point3 cen, double r, std::shared_ptr<material> m) : center(cen), radius(r), mat_ptr(m) {};

        virtual bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const override;
    

    public:
        point3 center;
        double radius;
        std::shared_ptr<material> mat_ptr;
};

bool sphere::hit(const ray& r, double t_min, double t_max, hit_record& rec) const{
    vec3 oc = r.origin() - center;
    auto a = r.direction().length_squared();
    auto b_h = dot(oc, r.direction());
    auto c = oc.length_squared() - radius*radius;
    auto discriminant = b_h*b_h - a*c;
    if(discriminant < 0){
        return false;
    }

    auto sqrtd = sqrt(discriminant);
    auto t_min_a = t_min * a;
    auto t_max_a = t_max * a;
    auto pseudo_root = -b_h - sqrtd;
    if(pseudo_root < t_min_a || t_max_a < pseudo_root){
        auto pseudo_root = -b_h + sqrtd;
        if(pseudo_root < t_min_a || t_max_a < pseudo_root){
            return false;
        }
    }

    rec.t = pseudo_root / a;
    rec.p = r.at(rec.t);
    vec3 outward_normal = (rec.p - center) / radius;
    rec.set_face_normal(r, outward_normal);
    rec.mat_ptr = mat_ptr;

    return true;
}

#endif