#ifndef __MODULESCENEFINAL_H__
#define __MODULESCENEFINAL_H__


#include "Module.h"
#include "CtrlCar.h"
#include "Animation.h"

struct SDL_Texture;
class TextFont;
class ModuleSceneFinal : public Module
{

public:
	ModuleSceneFinal(bool active = true);
	~ModuleSceneFinal();

	bool Start();
	update_status Update();
	bool CleanUp();

private:
	

public:
	SDL_Texture* finalSprites = nullptr;

private:

	string lap1, lap2, lap3, total;
	SDL_Rect background;
	TextFont* textFont;

};

#endif // __MODULESCENEFINAL_H__