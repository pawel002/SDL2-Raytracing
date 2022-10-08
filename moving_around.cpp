#include <SDL2/SDL.h>
#include <iostream>
#include <stdlib.h>
#include <math.h>
#include <vector>
#include <chrono>
#include "utils/functions.hpp"
#include "utils/vec3.hpp"
#include "utils/ray.hpp"
#include "utils/sphere.hpp"
#include "utils/hittable.hpp"
#include "utils/hittable_list.hpp"
#include "utils/camera.hpp"

using std::endl, std::cout, std::max, std::min;
const double INF = std::numeric_limits<double>::infinity();

color ray_color(const ray &r, const hittable &world, int depth){
	hit_record rec;
	if(depth <=0){
		return color(0, 0, 0);
	}
	if(world.hit(r, 0, INF, rec)){
		point3 target = rec.p + rec.normal + random_in_unit_sphere();
		return 0.5 * ray_color(ray(rec.p, target - rec.p), world, depth - 1);
	}
	auto t = 0.5*(normalised(r.direction()).y() + 1.0);
    return (1.0-t)*color(1.0, 1.0, 1.0) + t*color(0.5, 0.7, 1.0);
}

inline void set_color(const color &c, SDL_Renderer* renderer){
	SDL_SetRenderDrawColor(renderer, static_cast<int>(255.999 * min(0.999, max(c.x(), 0.0))), static_cast<int>(255.999 * min(0.999, max(c.y(), 0.0))), static_cast<int>(255.999 * min(0.999, max(c.z(), 0.0))), 255);
}

inline void draw_pixel(SDL_Renderer* renderer, const color &c, const int i, const int j){
	set_color(c, renderer);
	SDL_RenderDrawPoint(renderer, i, j);
}

// MAIN
const int WIDTH = 640;
const auto aspect_ratio = 16.0 / 9.0;
const int HEIGHT = int(WIDTH / aspect_ratio);
color COLOR_BUFFER[HEIGHT][WIDTH];

int main(int argv, char** args){

	set_seed(125);

	// VARIABLES
	int samples_pp = 1;
	int max_depth = 10;
	const int FPS = 60;

	// DEFINE WORLD
	hittable_list world;
    world.add(make_shared<sphere>(point3(0,0,-1), 0.5));
    world.add(make_shared<sphere>(point3(0,-100.5,-1), 100));

	// CALCULATED VARIABLES
	const int frameDelay = 1000 / FPS;
	bool show_completion = true;
	for(int i=0; i<HEIGHT; i++){
		for(int j=0; j<WIDTH; j++){
			COLOR_BUFFER[i][j] = color(0, 0, 0);
		}
	}
	int p_start = 0;

	// CAMERA
	camera cam;

	// Declared variables
	Uint32 frameStart;
	Uint32 frameTime;
	
	SDL_Init(SDL_INIT_EVERYTHING);

	SDL_Window *window = SDL_CreateWindow("Rendering", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, 0);
	SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, 0);

	bool isRunning = true;
	SDL_Event event;

	auto START = std::chrono::high_resolution_clock::now();

	// RUN LOOP
	while (isRunning)
	{	
		// Handling events
		while (SDL_PollEvent(&event))
		{
			switch(event.type)
			{
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
		}

		// frame start
		frameStart = SDL_GetTicks();

		// RENDERING MODULE
		// CLEAR
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
		SDL_RenderClear(renderer);

		// RENDERING
		for(int p=p_start; p<WIDTH*HEIGHT; p++){
			Uint32 renderTime = SDL_GetTicks() - frameStart;
			if(renderTime > frameDelay){
				p_start = p;
				break;
			}
			int i = p % WIDTH;
			int j = p / WIDTH;
			color pixel_color(0, 0, 0);
			for(int k=0; k<samples_pp; k++){
				auto u = (i + random_double()) / (WIDTH - 1);
				auto v = double(HEIGHT - 1 - j + random_double()) / (HEIGHT - 1);
				ray r = cam.get_ray(u, v);
				pixel_color += ray_color(r, world, max_depth);
			}
			COLOR_BUFFER[j][i] = pixel_color / samples_pp;
		}

		for(int p=0; p<p_start; p++){
			int i = p % WIDTH;
			int j = p / WIDTH;
			draw_pixel(renderer, COLOR_BUFFER[j][i], i, j);
		}

		// Render +  keeping const frames
		SDL_RenderPresent(renderer);
		frameTime = SDL_GetTicks() - frameStart;

		// std::cout << "Ft/Fd: " << frameTime << "/" << frameDelay << std::endl;

		if(frameDelay > frameTime){
			SDL_Delay(frameDelay - frameTime);
		}

		if(p_start == WIDTH*HEIGHT - 1 && show_completion){
			show_completion = false;
			auto END = std::chrono::high_resolution_clock::now();
			cout << "RENDERING TOOK: " << std::chrono::duration_cast<std::chrono::milliseconds>(END - START).count() << "ms." << endl;
		}

		cout << p_start << endl;

	}

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}