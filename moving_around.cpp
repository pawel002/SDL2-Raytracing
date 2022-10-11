// compiled using g++ -I src/include -I src/SDL2_IMG/ -L src/lib -o moving moving_around.cpp -lmingw32 -lSDL2main -lSDL2 -lSDL2_image

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <iostream>
#include <stdlib.h>
#include <math.h>
#include <vector>
#include <memory>
#include <typeinfo>
#include <chrono>
#include "utils2/functions.hpp"
#include "utils2/vec3.hpp"
#include "utils2/ray.hpp"
#include "utils2/sphere.hpp"
#include "utils2/plane.hpp"
#include "utils2/triangle.hpp"
#include "utils2/hittable.hpp"
#include "utils2/hittable_list.hpp"
#include "utils2/camera.hpp"
#include "utils2/material.hpp"

using std::endl, std::cout, std::max, std::min;
const double INF = std::numeric_limits<double>::infinity();

color GetPixelColor(const SDL_Surface* pSurface, const int X, const int Y){
	const Uint8 Bpp = pSurface->format->BytesPerPixel;
	Uint8* pPixel = (Uint8*)pSurface->pixels + Y * pSurface->pitch + X * Bpp;
	Uint32 PixelData = *(Uint32*)pPixel;
	SDL_Color Color = {0x00, 0x00, 0x00, SDL_ALPHA_OPAQUE};
	SDL_GetRGB(PixelData, pSurface->format, &Color.r, &Color.g, &Color.b);
	return color(1.0 * int(Color.r) / 255, 1.0 * int(Color.g) / 255, 1.0 * int(Color.b) / 255);
}

color skybox_color(const ray &r, const SDL_Surface* skybox){
	vec3 v = r.direction();
	double l = v.length();
	double theta = -asin(v.y()/l) + M_PI_2;
	double phi = v.x() < 0 ? atan(v.z()/v.x()) + M_PI_2 : atan(v.z()/v.x()) + 3*M_PI/2;
	int x = int(skybox->w * phi / (2 * M_PI));
	int y = int(skybox->h * theta / M_PI);
	return GetPixelColor(skybox, x, y);
}

color ray_color(const ray &r, const hittable &world, const SDL_Surface* skybox, int depth, bool depth_map){
	hit_record rec;
	if(depth <=0){
		return color(0, 0, 0);
	}
	if(world.hit(r, 0.001, INF, rec)){
		ray scattered;
		color attenuation;
		if(depth_map){
			float inv_t = min(1.0f / float(rec.t), 1.0f);
			return color(inv_t, inv_t, inv_t);
		}
		else if(rec.mat_ptr->scatter(r, rec, attenuation, scattered)){
			return attenuation*ray_color(scattered, world, skybox, depth-1, depth_map);
		}
		return color(0,0,0);
	}
	if(depth_map)
		return color(0,0,0);
	return skybox_color(r, skybox);
	// NO SKYBOX
	// auto t = 0.5*(normalised(r.direction()).y() + 1.0);
    // return (1.0-t)*color(1.0, 1.0, 1.0) + t*color(0.5, 0.7, 1.0);
}

inline void set_color(const color &c, SDL_Renderer* renderer){
	SDL_SetRenderDrawColor(renderer, static_cast<int>(255.999 * min(0.999, max(sqrt(c.x()), 0.0))), static_cast<int>(255.999 * min(0.999, max(sqrt(c.y()), 0.0))), static_cast<int>(255.999 * min(0.999, max(sqrt(c.z()), 0.0))), 255);
}

inline void draw_pixel(SDL_Renderer* renderer, const int resolution, const color &c, const int i, const int j){
	set_color(c, renderer);
	SDL_Rect r;
	r.x = i;
	r.y = j;
	r.w = resolution;
	r.h = resolution;
	SDL_RenderFillRect(renderer, &r);
}

// MAIN

int main(int argv, char** args){

	set_seed(125);

	// VARIABLES
	const int WIDTH = 720;
	const auto aspect_ratio = 16.0 / 9.0;
	const auto fov = 60;
	int samples_pp = 10;
	int max_depth = 1;
	int resolution = 3;
	const int FPS = 60;

	// CAMERA
	camera cam(fov, aspect_ratio, vec3(0, 0, 0), vec3(0, 1, 0));

	// CALCULATED VARIABLES
	const int frameDelay = 1000 / FPS;
	const int HEIGHT = int(WIDTH / aspect_ratio);
	float *DEPTH_BUFFER = new float[HEIGHT * WIDTH];

	// DEFINE WORLD
	hittable_list world;

	auto material_ground = make_shared<lambertian>(color(1.0, 1.0, 1.0));
	auto material_center = make_shared<lambertian>(color(0.1, 0.2, 0.5));
	auto material_left   = make_shared<metal>(color(0.8, 0.8, 0.8), 0.0);
	auto material_right  = make_shared<metal>(color(0.8, 0.6, 0.2), 0.0);
	auto material_walls =  make_shared<metal>(color(0.8, 0.8, 0.8), 0.0);

	// SPHERE
	world.add(make_shared<sphere>(point3(  20.0, 1.0, -1.0),   1.0, material_right));
	world.add(make_shared<sphere>(point3( 20.0, 10.0 * random_double(), -10.0 * random_double()),   1.0, material_right));
	world.add(make_shared<sphere>(point3( 20.0, 10.0 * random_double(), 10.0 * random_double()),   1.0, material_ground));
	world.add(make_shared<sphere>(point3( 20.0, 10.0 * random_double(), -1.0),   10.0 * random_double(), material_left));
	world.add(make_shared<plane>(point3(0, -1.0f, 0), vec3(0, 1, 0), material_ground));

	// LOAD SKYBOX
	SDL_Surface* skybox = IMG_Load("textures/castle1.jpg");
	if(skybox == NULL)
		cout << "Failed to load skybox." << endl;
	else
		cout << "Skybox loaded successfully. Size:" << skybox->w << "x" << skybox->h << "." << endl; 


	// Declared variables
	Uint32 frameStart;
	Uint32 frameTime;
	
	// Initializing SDL2
	if(SDL_Init(SDL_INIT_EVERYTHING) == 0)
		cout << "SDL2 Initialized successfully." << endl;
	else{
		cout << "Couldn't initialize SDL2." << endl;
		return  -1;
	}

	// Creating window
	SDL_Window *window = SDL_CreateWindow("Rendering", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, 0);
	if(window)
		cout << "SDL2 window created." << endl;
	else{
		cout << "Couldn't create SDL2 window" << endl;
		return -1;
	}

	// Attaching a renderer
	SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, 0);
	if(renderer)
		cout << "SDL2 renderer attached successfully." << endl;
	else{
		cout << "Couldn't attach the renderer to window." << endl;
		return -1;
	}

	bool isRunning = true;
	SDL_Event event;

	auto START = std::chrono::high_resolution_clock::now();

	// RUN LOOP
	while(isRunning){

		// Handling events
		while(SDL_PollEvent(&event)){
			switch(event.type){
			case SDL_QUIT:
				isRunning = false;
				break;

			case SDL_KEYDOWN:
				// quiting programm
				if(event.key.keysym.sym == SDLK_ESCAPE)
				{
					isRunning = false;
				}

				// fullscreen
				if(event.key.keysym.sym == SDLK_f){
					SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);
				}
			}

			cam.handle_inputs(event);
		}

		// frame start
		frameStart = SDL_GetTicks();

		// RENDERING

		// depth buffer

		for(int j=0; j<HEIGHT; j+=resolution){
			for(int i=0; i<WIDTH; i+=resolution){
				auto u = i * 1.f / (WIDTH - 1);
				auto v = float(HEIGHT - 1 - j) / (HEIGHT - 1);
				ray r = cam.get_ray(u, v);
				color ray_c = ray_color(r, world, skybox, max_depth, true);
				DEPTH_BUFFER[j*WIDTH + i] = ray_c[0];
				draw_pixel(renderer, resolution, ray_c, i, j);
			}
		}



		// while(p < WIDTH * HEIGHT){
		// 	Uint32 renderTime = SDL_GetTicks() - frameStart;
		// 	if(renderTime > frameDelay){
		// 		break;
		// 	}
		// 	int i = p % WIDTH;
		// 	int j = p / WIDTH * resolution;
		// 	color pixel_color(0, 0, 0);
		// 	for(int k=0; k<samples_pp; k++){
		// 		auto u = (i + random_double()) / (WIDTH - 1);
		// 		auto v = double(HEIGHT - 1 - j + random_double()) / (HEIGHT - 1);
		// 		ray r = cam.get_ray(u, v);
		// 		pixel_color += ray_color(r, world, skybox, max_depth, false);
		// 	}
		// 	draw_pixel(renderer, resolution, pixel_color / samples_pp, i, j);
		// 	p += resolution;
		// }

		// Render
		SDL_RenderPresent(renderer);
		frameTime = SDL_GetTicks() - frameStart;

		// std::cout << "Ft/Fd: " << frameTime << "/" << frameDelay << std::endl;

		// if(p == WIDTH*HEIGHT && show_completion){
		// 	show_completion = false;
		// 	auto END = std::chrono::high_resolution_clock::now();
		// 	cout << "RENDERING TOOK: " << std::chrono::duration_cast<std::chrono::milliseconds>(END - START).count() << "ms." << endl;
		// }

	}
	

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}