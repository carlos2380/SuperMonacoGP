#ifndef __CTRLUI_H__
#define __CTRLUI_H__


#include "Module.h"
#include "Animation.h"

class TextFont;
class CtrlUI : public Module
{

public:
	CtrlUI(bool active = true);
	CtrlUI(SDL_Texture* raceSprites, bool active = true);
	~CtrlUI();

	bool Start(ModuleSceneRace* &moduleRace);
	update_status Update();
	bool CleanUp();
	void setGear(mode gear);

private:
	void loadTextFonts();
	void loadRects();

	void printSpeed();
	void printGear();
	void printPositions();
	void printTimes();
	void unloadTextFonts();
	void unloadRects();
	
	void intToStringTens(int i, string &result);
	void millisecTomssdd(Uint32 i, string &result);
	void millisecTossdd(Uint32 i, string &result);

public:
	SDL_Texture* uISprites = nullptr;
	CtrlCar* ctrlCar;
	ModuleSceneRace* moduleRace;

private:
	int tick;
	TextFont* position = nullptr;
	TextFont* positionRed = nullptr;
	TextFont* positionLimit = nullptr;
	TextFont* positionLimitRed = nullptr;
	TextFont* speedTime = nullptr;
	TextFont* lapYellow = nullptr;
	TextFont* lap = nullptr;

	SDL_Rect* automatic;
	vector<SDL_Rect*> manual;
	vector<SDL_Rect*> super;

	SDL_Rect* gearMark;
	SDL_Rect* limitOver;

	SDL_Point* centerRotate;
};

#endif // __CTRLUI_H__
