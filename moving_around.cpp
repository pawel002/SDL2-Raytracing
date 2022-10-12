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

Uint8 UINT8_LOOK_UP[] = {0, 15, 22, 27, 31, 35, 39, 42, 45, 47,
50, 52, 55, 57, 59, 61, 63, 65, 67, 69,
71, 73, 74, 76, 78, 79, 81, 82, 84, 85,
87, 88, 90, 91, 93, 94, 95, 97, 98, 99,
100, 102, 103, 104, 105, 107, 108, 109, 110, 111,
112, 114, 115, 116, 117, 118, 119, 120, 121, 122,
123, 124, 125, 126, 127, 128, 129, 130, 131, 132,
133, 134, 135, 136, 137, 138, 139, 140, 141, 141,
142, 143, 144, 145, 146, 147, 148, 148, 149, 150,
151, 152, 153, 153, 154, 155, 156, 157, 158, 158,
159, 160, 161, 162, 162, 163, 164, 165, 165, 166, 
167, 168, 168, 169, 170, 171, 171, 172, 173, 174,
174, 175, 176, 177, 177, 178, 179, 179, 180, 181,
182, 182, 183, 184, 184, 185, 186, 186, 187, 188,
188, 189, 190, 190, 191, 192, 192, 193, 194, 194,
195, 196, 196, 197, 198, 198, 199, 200, 200, 201,
201, 202, 203, 203, 204, 205, 205, 206, 206, 207,
208, 208, 209, 210, 210, 211, 211, 212, 213, 213,
214, 214, 215, 216, 216, 217, 217, 218, 218, 219,
220, 220, 221, 221, 222, 222, 223, 224, 224, 225,
225, 226, 226, 227, 228, 228, 229, 229, 230, 230,
231, 231, 232, 233, 233, 234, 234, 235, 235, 236,
236, 237, 237, 238, 238, 239, 240, 240, 241, 241, 
242, 242, 243, 243, 244, 244, 245, 245, 246, 246,
247, 247, 248, 248, 249, 249, 250, 250, 251, 251,
252, 252, 253, 253, 254, 255};

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

inline void draw_pixel(SDL_Renderer* renderer, SDL_Rect r, const int resolution, const color &c, const int i, const int j){
	set_color(c, renderer);
	if(resolution == 1){
		SDL_RenderDrawPoint(renderer, i, j);
		return;
	}
	r.x = i;
	r.y = j;
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
	camera cam(fov, aspect_ratio, vec3(10, 0, 10), vec3(0, 1, 0));

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
	world.add(make_shared<sphere>(point3( 20.0, 10.0 * random_double(), -1.0),   2.0 * random_double(), material_left));
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
	Uint32 timeMeasure;
	
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
	SDL_Rect rect;
	rect.w = resolution;
	rect.h = resolution;

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

		timeMeasure = Uint32(0);

		for(int j=0; j<HEIGHT; j+=resolution){
			for(int i=0; i<WIDTH; i+=resolution){
				auto u = i * 1.f / (WIDTH - 1);
				auto v = float(HEIGHT - 1 - j) / (HEIGHT - 1);
				ray r = cam.get_ray(u, v);
				auto time1 = SDL_GetTicks();
				color ray_c = ray_color(r, world, skybox, max_depth, true);
				timeMeasure += SDL_GetTicks() - time1;
				// DEPTH_BUFFER[j*WIDTH + i] = ray_c[0];

				// draw_pixel(renderer, rect, resolution, ray_c, i, j);
				SDL_SetRenderDrawColor(renderer, UINT8_LOOK_UP[static_cast<int>(ray_c[0] * 255)], UINT8_LOOK_UP[static_cast<int>(ray_c[0] * 255)], UINT8_LOOK_UP[static_cast<int>(ray_c[0] * 255)], 255);
				rect.x = i;
				rect.y = j;
				SDL_RenderFillRect(renderer, &rect);
			}
		}

		cout << "Drawing time " << timeMeasure << endl;


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
		cout << frameTime << endl;

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