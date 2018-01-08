#ifndef __RESULTBRIDGESCENES_H__
#define __RESULTBRIDGESCENES_H__
#include <SDL_hints.h>
#include "CtrlCar.h"

class ResultBridgeScenes
{

public:
	~ResultBridgeScenes();
	static ResultBridgeScenes *getInstance();

private:
	ResultBridgeScenes();
	static ResultBridgeScenes* instance;

public:
	mode gearSelected;
	Uint32 totalTime;
	Uint32 lap1Time;
	Uint32 lap2Time;
	Uint32 lap3Time;
};

#endif
