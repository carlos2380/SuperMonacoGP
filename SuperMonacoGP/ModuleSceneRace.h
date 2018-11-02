#ifndef __MODULESCENERACE_H__
#define __MODULESCENERACE_H__


#include "Module.h"
#include "Animation.h"
#include "CtrlCar.h"
#include "CtrlUI.h"
#include "CtrlMap.h"

struct SDL_Texture;
class ModuleSceneRace : public Module
{

public:
	ModuleSceneRace(bool active = true);
	~ModuleSceneRace();

	bool Start();
	update_status Update();
	bool CleanUp();

	void nextLap();
private:
	
public:
	SDL_Texture* raceSprites = nullptr;
	
	CtrlMap* ctrlMap;
	CtrlCar* ctrlCar;
	CtrlUI* ctrlUI;

	int lap = 0;
	int position = 10;
	int positionLimit = 16;
	bool printSemaforo;
	int semaforoState;
	bool stopCar;

	Uint32 totalTime = 0;
	Uint32 firstLap = 0;
	Uint32 secondLap = 0;
	Uint32 thirdLap = 0;
	Uint32 bestLap = 0;
	Uint32 timeToFirst = 0;
	Uint32 distanceTime = 0;
	Uint32 initialTime = 0;

private:
	SDL_Rect background;


};

#endif // __MODULESCENERACE_H__