#ifndef VEC3_H
#define VEC3_H

#include <cmath>
#include <iostream>
#include <stdlib.h>
#include <cassert>
#include <bits/stdc++.h>
#include "functions.hpp"

#define assertm(exp, msg) assert(((void)msg, exp))

using std::sqrt;

class vec3{
    public:
        vec3() : e{0, 0, 0} {}

        vec3(double x, double y, double z) : e{x, y, z} {}

        double x() const {return e[0]; }
        double y() const {return e[1]; }
        double z() const {return e[2]; }

        vec3 operator-() const {return vec3(-e[0], -e[1], -e[2]); }
        double operator[](int i) const {return e[i]; }
        double& operator[](int i) {return e[i]; }

        vec3& operator+=(const vec3 &v){
            e[0] += v.e[0];
            e[1] += v.e[1];
            e[2] += v.e[2];
            return *this;
        }

        vec3& operator-=(const vec3 &v){
            e[0] -= v.e[0];
            e[1] -= v.e[1];
            e[2] -= v.e[2];
            return *this;
        }

        vec3& operator*=(const double t){
            e[0] *= t;
            e[1] *= t;
            e[2] *= t;
            return *this;
        }

        vec3& operator/=(const double t){
            assertm(t != 0, "Cannot divide by 0.");
            double inv = 1/t;
            e[0] *= inv; 
            e[1] *= inv; 
            e[2] *= inv;
            return *this; 
        }

        double length_squared() const {
            return e[0]*e[0] + e[1]*e[1] + e[2]*e[2]; 
        }

        double length() const {
            return sqrt(length_squared());
        }

        bool near_zero(){
            const auto s = 1e-8;
            return fabs(e[0]) < s && fabs(e[1]) && fabs(e[2]);
        }

    public:
        double e[3];
};

vec3 random_vec(double min, double max){
    return vec3(random(min, max), random(min, max), random(min, max));
}

vec3 random_in_unit_sphere(){
    while(true){
        auto p = random_vec(-1, 1);
        if(p.length_squared() <= 1){
            return p;
        }
    }
}

inline std::ostream& operator<<(std::ostream &out, const vec3 &v) {
    return out << v.e[0] << ' ' << v.e[1] << ' ' << v.e[2];
}

inline vec3 operator+(const vec3 &u, const vec3 &v) {
    return vec3(u.e[0] + v.e[0], u.e[1] + v.e[1], u.e[2] + v.e[2]);
}

inline vec3 operator-(const vec3 &u, const vec3 &v) {
    return vec3(u.e[0] - v.e[0], u.e[1] - v.e[1], u.e[2] - v.e[2]);
}

inline vec3 operator*(const vec3 &u, const vec3 &v) {
    return vec3(u.e[0] * v.e[0], u.e[1] * v.e[1], u.e[2] * v.e[2]);
}

inline vec3 operator*(double t, const vec3 &v) {
    return vec3(t*v.e[0], t*v.e[1], t*v.e[2]);
}

inline vec3 operator*(const vec3 &v, double t) {
    return t * v;
}

inline vec3 operator/(vec3 v, double t) {
    return (1/t) * v;
}

inline double dot(const vec3 &u, const vec3 &v) {
    return u.e[0] * v.e[0]
         + u.e[1] * v.e[1]
         + u.e[2] * v.e[2];
}

inline vec3 cross(const vec3 &u, const vec3 &v) {
    return vec3(u.e[1] * v.e[2] - u.e[2] * v.e[1],
                u.e[2] * v.e[0] - u.e[0] * v.e[2],
                u.e[0] * v.e[1] - u.e[1] * v.e[0]);
}

inline vec3 unit_vector(vec3 v) {
    return v / v.length();
}

inline vec3 normalised(vec3 v){
    assertm(v.length_squared() != 0, "Cannot normalise 0-vector.");
    double inv = 1/v.length();
    return vec3(v.e[0]*inv, v.e[1]*inv, v.e[2]);  
}

vec3 random_unit_vector(){
    return normalised(random_in_unit_sphere());
}

vec3 reflect(const vec3 &v, const vec3 &n){
    return v - 2*dot(v, n)*n;
}

vec3 refract(const vec3 &uv, const vec3 &n, double etai_over_etat){
    auto cos_theta = fmin(dot(-uv, n), 1.0);
    vec3 r_out_perp =  etai_over_etat * (uv + cos_theta*n);
    vec3 r_out_parallel = -sqrt(fabs(1.0 - r_out_perp.length_squared())) * n;
    return r_out_perp + r_out_parallel;
}

using point3 = vec3;
using color = vec3;


#endif