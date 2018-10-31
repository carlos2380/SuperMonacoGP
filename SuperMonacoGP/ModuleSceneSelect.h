#ifndef __MODULESCENESELECT_H__
#define __MODULESCENESELECT_H__


#include "Module.h"
#include "CtrlCar.h"
#include "Animation.h"

struct SDL_Texture;
class TextFont;
class ModuleSceneSelect : public Module
{

public:
	ModuleSceneSelect(bool active = true);
	~ModuleSceneSelect();

	bool Start();
	update_status Update();
	bool CleanUp();

private:
	void renderSelected();

public:
	SDL_Texture* selectSprites = nullptr;

private:
	

	SDL_Rect background;
	SDL_Rect backgroundSelect;
	SDL_Rect padSelected;
	SDL_Rect padUnselected;
	Animation perimeterSelected;
	mode modeSelect;
	TextFont* textFont;
	Uint32 startTicks;
	Uint32 time;
	Uint32 delay;
	int fx = 0;
};

#endif // __MODULESCENESELECT_H__