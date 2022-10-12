#ifndef CAMERA_H
#define CAMERA_H

#include "vec3.hpp"
#include "ray.hpp"

class camera {
    public:
        camera(double vfov, double aspect_ratio, point3 look_from, vec3 vup) {
            auto theta = vfov * M_PI / 180;
            auto h = tan(theta/2);
            viewport_height = 2.0 * h;
            viewport_width = aspect_ratio * viewport_height;
            theta = 0.f;
            phi = 0.f;
            cam_up = vup;
            origin = look_from;
            cam_speed = 1.0f;

            auto w = vec3(cosf(theta) * sinf(phi), sinf(theta), cosf(theta) * cosf(phi));
            auto u = unit_vector(cross(cam_up, w));
            auto v = cross(w, u);

            horizontal = viewport_width * u;
            vertical = viewport_height * v;
            lower_left_corner = origin - horizontal/2 - vertical/2 - w;
        }

        ray get_ray(double u, double v) const {
            return ray(origin, lower_left_corner + u*horizontal + v*vertical - origin);
        }

        void handle_inputs(SDL_Event event){
            switch(event.type){
                case SDL_KEYDOWN:
                    if(event.key.keysym.sym == SDLK_DOWN){
                        theta += M_PI / 180.f;
                        if(theta >= M_PI_2 - 0.01f)
                            theta = M_PI_2 - 0.01f;
                    }
                    else if(event.key.keysym.sym == SDLK_UP){
                        theta -= M_PI / 180.f;
                        if(theta <= -M_PI_2 + 0.01f)
                            theta = -M_PI_2 + 0.01f;
                    }
                    else if(event.key.keysym.sym == SDLK_LEFT){
                        phi += M_PI / 180.f;
                        if(phi >= 2 * M_PI)
                            phi -= 2 * M_PI;
                    }
                    else if(event.key.keysym.sym == SDLK_RIGHT){
                        phi -= M_PI / 180.f;
                        if(phi <= -2 * M_PI)
                            phi += 2 * M_PI;
                    }
                    else if(event.key.keysym.sym == SDLK_w){
                        origin -= cam_speed * vec3(sinf(phi), 0, cosf(phi));
                    }
                    else if(event.key.keysym.sym == SDLK_s){
                        origin += cam_speed * vec3(sinf(phi), 0, cosf(phi));
                    }
                    else if(event.key.keysym.sym == SDLK_d){
                        origin += cam_speed * vec3(sinf(phi + M_PI_2), 0, cosf(phi + M_PI_2));
                    }
                    else if(event.key.keysym.sym == SDLK_a){
                        origin -= cam_speed * vec3(sinf(phi + M_PI_2), 0, cosf(phi + M_PI_2));
                    }
                    else if(event.key.keysym.sym == SDLK_SPACE){
                        origin += cam_speed * vec3(0, 1, 0);
                    }
                    else if(event.key.keysym.sym == SDLK_LCTRL){
                        origin -= cam_speed * vec3(0, 1, 0);
                    }
            }

            auto w = vec3(cosf(theta) * sinf(phi), sinf(theta), cosf(theta) * cosf(phi));
            auto u = unit_vector(cross(cam_up, w));
            auto v = cross(w, u);

            horizontal = viewport_width * u;
            vertical = viewport_height * v;
            lower_left_corner = origin - horizontal/2 - vertical/2 - w;
        }

    private:
        point3 origin;
        point3 lower_left_corner;
        vec3 horizontal;
        vec3 vertical;
        vec3 cam_up;
        float theta;
        float phi;
        float viewport_height;
        float viewport_width;
        float cam_speed;
};


#endif