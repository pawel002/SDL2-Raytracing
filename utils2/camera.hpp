#ifndef CAMERA_H
#define CAMERA_H

#include "vec3.hpp"
#include "ray.hpp"

class camera {
    public:
        camera(double vfov, double aspect_ratio, point3 look_from, vec3 vup){
            for(int i=0; i<10; i++){
                KEY_PRESSED[i] = false;
            }
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
                    if(KEY_PRESSED[0] == false && event.key.keysym.sym == SDLK_w)
                        KEY_PRESSED[0] = true;
                    else if(KEY_PRESSED[1] == false && event.key.keysym.sym == SDLK_s)
                        KEY_PRESSED[1] = true;
                    else if(KEY_PRESSED[2] == false && event.key.keysym.sym == SDLK_a)
                        KEY_PRESSED[2] = true;
                    else if(KEY_PRESSED[3] == false && event.key.keysym.sym == SDLK_d)
                        KEY_PRESSED[3] = true;
                    else if(KEY_PRESSED[4] == false && event.key.keysym.sym == SDLK_SPACE)
                        KEY_PRESSED[4] = true;
                    else if(KEY_PRESSED[5] == false && event.key.keysym.sym == SDLK_LCTRL)
                        KEY_PRESSED[5] = true;
                    else if(KEY_PRESSED[6] == false && event.key.keysym.sym == SDLK_UP)
                        KEY_PRESSED[6] = true;
                    else if(KEY_PRESSED[7] == false && event.key.keysym.sym == SDLK_DOWN)
                        KEY_PRESSED[7] = true;
                    else if(KEY_PRESSED[8] == false && event.key.keysym.sym == SDLK_RIGHT)
                        KEY_PRESSED[8] = true;
                    else if(KEY_PRESSED[9] == false && event.key.keysym.sym == SDLK_LEFT)
                        KEY_PRESSED[9] = true;
                
                case SDL_KEYUP:
                    if(KEY_PRESSED[0] == true && event.key.keysym.sym == SDLK_w)
                        KEY_PRESSED[0] = false;
                    else if(KEY_PRESSED[1] == true && event.key.keysym.sym == SDLK_s)
                        KEY_PRESSED[1] = false;
                    else if(KEY_PRESSED[2] == true && event.key.keysym.sym == SDLK_a)
                        KEY_PRESSED[2] = false;
                    else if(KEY_PRESSED[3] == true && event.key.keysym.sym == SDLK_d)
                        KEY_PRESSED[3] = false;
                    else if(KEY_PRESSED[4] == true && event.key.keysym.sym == SDLK_SPACE)
                        KEY_PRESSED[4] = false;
                    else if(KEY_PRESSED[5] == true && event.key.keysym.sym == SDLK_LCTRL)
                        KEY_PRESSED[5] = false;
                    else if(KEY_PRESSED[6] == true && event.key.keysym.sym == SDLK_UP)
                        KEY_PRESSED[6] = false;
                    else if(KEY_PRESSED[7] == true && event.key.keysym.sym == SDLK_DOWN)
                        KEY_PRESSED[7] = false;
                    else if(KEY_PRESSED[8] == true && event.key.keysym.sym == SDLK_RIGHT)
                        KEY_PRESSED[8] = false;
                    else if(KEY_PRESSED[9] == true && event.key.keysym.sym == SDLK_LEFT)
                        KEY_PRESSED[9] = false;
            }

        // handle keyboard in one function and add second function handle_movement for updateing camera
            if(KEY_PRESSED[6]){
                theta += M_PI / 180.f;
                if(theta >= M_PI_2 - 0.01f)
                    theta = M_PI_2 - 0.01f;
            }
            if(KEY_PRESSED[7]){
                theta -= M_PI / 180.f;
                if(theta <= -M_PI_2 + 0.01f)
                    theta = -M_PI_2 + 0.01f;
            }
            if(KEY_PRESSED[9]){
                phi += M_PI / 180.f;
                if(phi >= 2 * M_PI)
                    phi -= 2 * M_PI;
            }
            if(KEY_PRESSED[8]){
                phi -= M_PI / 180.f;
                if(phi <= -2 * M_PI)
                    phi += 2 * M_PI;
            }
            if(KEY_PRESSED[0]){
                origin -= cam_speed * vec3(sinf(phi), 0, cosf(phi));
            }
            if(KEY_PRESSED[1]){
                origin += cam_speed * vec3(sinf(phi), 0, cosf(phi));
            }
            if(KEY_PRESSED[3]){
                origin += cam_speed * vec3(sinf(phi + M_PI_2), 0, cosf(phi + M_PI_2));
            }
            if(KEY_PRESSED[2]){
                origin -= cam_speed * vec3(sinf(phi + M_PI_2), 0, cosf(phi + M_PI_2));
            }
            if(KEY_PRESSED[4]){
                origin += cam_speed * vec3(0, 1, 0);
            }
            if(KEY_PRESSED[5]){
                origin -= cam_speed * vec3(0, 1, 0);
            }

            auto w = vec3(cosf(theta) * sinf(phi), sinf(theta), cosf(theta) * cosf(phi));
            auto u = unit_vector(cross(cam_up, w));
            auto v = cross(w, u);

            horizontal = viewport_width * u;
            vertical = viewport_height * v;
            lower_left_corner = origin - horizontal/2 - vertical/2 - w;
            cout << KEY_PRESSED[0] << " "<< KEY_PRESSED[1] << " "<< KEY_PRESSED[2] << " "<< KEY_PRESSED[3] << " " << endl;
        }

    private:
        bool KEY_PRESSED[10];
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