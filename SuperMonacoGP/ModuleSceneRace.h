#ifndef __MODULESCENERACE_H__
#define __MODULESCENERACE_H__


#include "Module.h"
#include "Animation.h"
#include "CtrlCar.h"
#include "CtrlUI.h"

struct SDL_Texture;
class ModuleSceneRace : public Module
{

public:
	ModuleSceneRace(bool active = true);
	~ModuleSceneRace();

	bool Start();
	update_status Update();
	bool CleanUp();

private:
	void drawQuad(int x1, int y1, int w1, int x2, int y2, int w2, int r, int g, int b);
public:
	SDL_Texture* raceSprites = nullptr;
	CtrlCar ctrlCar;
	CtrlUI ctrlUI;

	int lap = 2;
	int position = 12;
	int positionLimit = 16;

	Uint32 totalTime = 123455;
	Uint32 firstLap = 123455;
	Uint32 secondLap = 123455;
	Uint32 thirdLap = 123455;
	Uint32 timeToFirst = 123455;
	Uint32 distanceTime = 123455;

private:
	SDL_Rect background;
	


};

#endif // __MODULESCENERACE_H__