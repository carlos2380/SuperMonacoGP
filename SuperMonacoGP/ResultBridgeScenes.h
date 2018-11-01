#ifndef __RESULTBRIDGESCENES_H__
#define __RESULTBRIDGESCENES_H__
#include <SDL_hints.h>
#include "CtrlCar.h"

class ResultBridgeScenes
{

public:
	~ResultBridgeScenes();
	static ResultBridgeScenes* getInstance();
	static void resetInstance();

private:
	ResultBridgeScenes();
	static ResultBridgeScenes* instance;

public:
	mode gearSelected;
	string totalTime;
	string lap1Time;
	string lap2Time;
	string lap3Time;
};

#endif
