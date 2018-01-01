#ifndef __MODULESCENESTART_H__
#define __MODULESCENESTART_H__


#include "Module.h"
#include "Animation.h"
struct SDL_Texture;

class ModuleSceneStart : public Module
{
public:
	ModuleSceneStart(bool active = true);
	~ModuleSceneStart();

	bool Start();
	update_status Update();
	bool CleanUp();

private:
	void renderCars();

public:
	SDL_Texture* startSprites = nullptr;

private:
	SDL_Rect background;
	SDL_Rect sega;
	SDL_Rect girl;
	SDL_Rect poster;
	Animation* start = nullptr;
	vector<SDL_Rect> carsRect;
	uint fx = 0;

	struct car
	{
		int x;
		int y;
		SDL_Rect *carSprite;
		int speed;
	};

	vector<car*> cars;
};

#endif // __MODULESCENESTART_H__