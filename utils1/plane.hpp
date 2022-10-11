#ifndef PLANE_H
#define PLANE_H

#include "hittable.hpp"
#include "vec3.hpp"
#include "material.hpp"

using std::cout, std::endl;

class plane : public hittable {
    public:
        plane() {}
        plane(point3 cen, vec3 n, std::shared_ptr<material> m) : center(cen), normal(n), mat_ptr(m) {};

        virtual bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const override;
    

    public:
        point3 center;
        vec3 normal;
        std::shared_ptr<material> mat_ptr;
};

bool plane::hit(const ray& r, double t_min, double t_max, hit_record& rec) const{
    auto t = dot(center - r.origin(), normal) / dot(normal, r.direction());
    if(t > t_min &&  t_max > t){
        rec.t = t;
        rec.p = r.at(rec.t);
        rec.set_face_normal(r, normal);
        rec.mat_ptr = mat_ptr;
        return true;
    }
    return false;
}

#endif