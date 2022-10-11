#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "hittable.hpp"
#include "material.hpp"
#include "vec3.hpp"
#include <memory>

using std::cout, std::endl;

class triangle : public hittable {
    public:
        triangle() {}
        triangle(point3 p0_, point3 p1_, point3 p2_, std::shared_ptr<material> m) : p0(p0_), p1(p1_), p2(p2_), mat_ptr(m) {normal = unit_vector(cross(p1-p0, p2-p0));};

        virtual bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const override;
    

    public:
        point3 p0;
        point3 p1;
        point3 p2;
        vec3 normal;
        std::shared_ptr<material> mat_ptr;
};

bool triangle::hit(const ray& r, double t_min, double t_max, hit_record& rec) const{
    auto t = dot(p0 - r.origin(), normal) / dot(normal, r.direction());
    point3 intersection = r.at(t);

    double e0 = dot(normal, cross(intersection - p0, p1 - p0));
    if(e0 > 0){
        return false;
    }
    double e1 = dot(normal, cross(intersection - p1, p2 - p1));
    if(e1 > 0){
        return false;
    }
    double e2 = dot(normal, cross(intersection - p2, p0 - p2));
    if(e2 > 0){
        return false;
    }

    if(t > t_min &&  t_max > t){
        rec.t = t;
        rec.p = r.at(rec.t);
        vec3 outward_normal = dot(r.direction(), normal) > 0 ? normal : -normal;
        rec.set_face_normal(r, outward_normal);
        rec.mat_ptr = mat_ptr;
        return true;
    }

    return false;
}

#endif