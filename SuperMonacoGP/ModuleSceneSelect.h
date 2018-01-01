#ifndef __MODULESCENESELECT_H__
#define __MODULESCENESELECT_H__


#include "Module.h"
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
	enum mode
	{
		AUTOMATIC = 0,
		MANUAL,
		SUPER
	};

	SDL_Rect background;
	SDL_Rect backgroundSelect;
	SDL_Rect padSelected;
	SDL_Rect padUnselected;
	Animation* perimeterSelected = nullptr;
	mode modeSelect;
	TextFont* textFont;
	Uint32 startTicks;
	Uint32 time;
	Uint32 delay;
	
};

#endif // __MODULESCENESELECT_H__