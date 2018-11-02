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
	std::string totalTime;
	std::string lap1Time;
	std::string lap2Time;
	std::string lap3Time;
};

#endif
