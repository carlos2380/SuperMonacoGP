#include "Globals.h"
#include "Application.h"
#include "ModuleTextures.h"
#include "ModuleAudio.h"
#include "ModuleInput.h"
#include "ModuleRender.h"
#include "ModuleFadeToBlack.h"
#include "ModuleSceneRace.h"
#include <SDL.h>
#include <iostream>
#include "CtrlUI.h"
#include "TextFont.h"


CtrlUI::CtrlUI(bool active) : Module(active)
{
}



CtrlUI::~CtrlUI()
{}

// Load assets
bool CtrlUI::Start(ModuleSceneRace* &moduleRace)
{

	uISprites = App->textures->Load("Sprites/RaceSprites.bmp", 255, 0, 255);

	this->moduleRace = moduleRace;

	loadTextFonts();
	loadRects();

	centerRotate = new SDL_Point();
	centerRotate->x = 0;
	centerRotate->y = 3;
	tick = 1;
	/*if (fx == 0)
	fx = App->audio->LoadFx("rtype/starting.wav");*/

	return true;
}


// UnLoad assets
bool CtrlUI::CleanUp()
{
	LOG("Unloading space scene");
	unloadRects();
	unloadTextFonts();
	uISprites = nullptr;
	return true;
}

update_status CtrlUI::Update()
{
	/*int mx, my;
	SDL_GetMouseState(&mx, &my);
	//cout << mx << ": " << my << endl;*/

	++tick;

	printSpeed();
	printGear();
	printPositions();
	printTimes();

	App->renderer->BlitRotate(uISprites, 17, 88, gearMark, ctrlCar->revol, centerRotate);
	speedTime->print(288, 70, to_string(moduleRace->lap));

	return UPDATE_CONTINUE;
}

void CtrlUI::loadTextFonts()
{
	position = new TextFont(uISprites, "0123456789", 0, 850, 18, 22);
	positionRed = new TextFont(uISprites, "0123456789", 0, 875, 18, 22);
	positionLimit = new TextFont(uISprites, "0123456789", 0, 900, 13, 16);
	positionLimitRed = new TextFont(uISprites, "0123456789", 0, 920, 13, 16);
	speedTime = new TextFont(uISprites, "0123456789\"'", 0, 940, 8, 15);
	lapYellow = new TextFont(uISprites, "0123456789\"'", 0, 960, 8, 8);
	lap = new TextFont(uISprites, "0123456789\"'", 0, 970, 8, 8);
}

void CtrlUI::loadRects()
{
	automatic = new SDL_Rect();
	automatic->x = 200;
	automatic->y = 520;
	automatic->w = 60;
	automatic->h = 16;

	manual = vector<SDL_Rect*>(4);
	for (int i = 0; i < manual.size(); ++i)
	{
		SDL_Rect* rect = new SDL_Rect();
		rect->x = 200;
		rect->y = 540 + (i * 16);
		rect->w = 44;
		rect->h = 16;
		manual[i] = rect;
	}

	super = vector<SDL_Rect*>(7);
	for (int i = 0; i < super.size(); ++i)
	{
		SDL_Rect* rect = new SDL_Rect();
		rect->x = 200;
		rect->y = 625 + (i * 24);
		rect->w = 44;
		rect->h = 24;
		super[i] = rect;
	}

	gearMark = new SDL_Rect();
	gearMark->x = 200;
	gearMark->y = 500;
	gearMark->h = 5;
	gearMark->w = 63;

	limitOver = new SDL_Rect();
	limitOver->x = 200;
	limitOver->y = 800;
	limitOver->h = 16;
	limitOver->w = 55;
}


void CtrlUI::printSpeed()
{
	int speed = ctrlCar->speed;
	if(speed > 99)
	{
		speedTime->print(265, 215, to_string(speed));
	}
	else if (speed > 9) {
		speedTime->print(273, 215, to_string(speed));
	}
	else
	{
		speedTime->print(281, 215, to_string(speed));
	}
}

void CtrlUI::printGear()
{
	switch (ctrlCar->gear)
	{
	case AUTOMATIC:
		App->renderer->Blit(uISprites, 3, 112, automatic);
		break;
	case MANUAL:
		App->renderer->Blit(uISprites, 10, 112, manual[ctrlCar->gearSelect-1]);
		break;
	case SUPER:
		App->renderer->Blit(uISprites, 10, 112, super[ctrlCar->gearSelect-1]);
		break;
	}
}

void CtrlUI::printPositions()
{
	string posit = "";
	string positLimit = "";
	intToStringTens(moduleRace->position, posit);
	intToStringTens(moduleRace->positionLimit, positLimit);

	if(moduleRace->position < moduleRace->positionLimit)
	{
		position->print(139, 101, posit);
		positionLimit->print(144, 76, positLimit);
	} else if(moduleRace->position == moduleRace->positionLimit)
	{
		positionRed->print(139, 101, posit);
		positionLimitRed->print(144, 76, positLimit);
	} else
	{
		if(tick%30 < 15)
		{
			positionRed->print(139, 101, posit);
			positionLimitRed->print(144, 76, positLimit);
			App->renderer->Blit(uISprites, 187, 107, limitOver);
		}
	}
}

void CtrlUI::printTimes()
{
	string time="";
	millisecTomssdd(moduleRace->totalTime, time);
	speedTime->print(247, 8, time);
	time = "";
	millisecTossdd(moduleRace->distanceTime, time);
	speedTime->print(24, 8, time);

	string lap1 = "";
	millisecTomssdd(moduleRace->firstLap, lap1);
	string lap2 = "";
	millisecTomssdd(moduleRace->secondLap, lap2);
	string lap3 = "";
	millisecTomssdd(moduleRace->thirdLap, lap3);

	switch (moduleRace->lap)
	{
		case 1:
			lapYellow->print(247, 33, lap1);
			lap->print(247, 41, "0'00\"00");
			lap->print(247, 49, "0'00\"00");
		break;
		case 2:
			lapYellow->print(247, 33, lap1);
			lapYellow->print(247, 41, lap2);
			lap->print(247, 49, "0'00\"00");
			break;
		case 3:
			lapYellow->print(247, 33, lap1);
			lapYellow->print(247, 41, lap2);
			lapYellow->print(247, 49, lap3);
			break;
	}
}

void CtrlUI::intToStringTens(int i, string &result)
{
	if (i < 10)
	{
		result = " " + to_string(i);
	}
	else 
	{
		result = to_string(i);
	}
}

void CtrlUI::millisecTomssdd(Uint32 i, string& result)
{
	Uint32 decim =  (i % 1000) / 10;
	Uint32 seconds = (i / 1000) % 60;
	Uint32 minutes = ((i / (1000 * 60)) % 60);
	string d = "";
	string s = "";
	if (decim < 10) d = "0" + to_string(decim);
	else d = to_string(decim);
	if (seconds < 10) s = "0" + to_string(seconds);
	else s = to_string(seconds);

	result = to_string(minutes) + "'" + s + "\"" + d;

}


void CtrlUI::millisecTossdd(Uint32 i, string& result)
{
	Uint32 decim = (i % 1000) / 10;
	Uint32 seconds = (i / 1000) % 60;
	string d = "";
	string s = "";
	if (decim < 10) d = "0" + to_string(decim);
	else d = to_string(decim);
	if (seconds < 10) s = "0" + to_string(seconds);
	else s = to_string(seconds);
	result =  s + "\"" + d;

}

void CtrlUI::unloadRects()
{
	automatic = nullptr;
	delete automatic;
	for (int i = 0; i < manual.size(); ++i)
	{
		manual[i] = nullptr;
		delete manual[i];
	}

	for (int i = 0; i < super.size(); ++i)
	{
		super[i] = nullptr;
		delete super[i];
	}

}

void CtrlUI::unloadTextFonts()
{
	position = nullptr;
	delete position;
	positionRed = nullptr;
	delete positionRed;
	positionLimit = nullptr;
	delete positionLimit;
	positionLimitRed = nullptr;
	delete positionLimitRed;
	speedTime = nullptr;
	delete speedTime;
	lapYellow = nullptr;
	delete lapYellow;
	lap = nullptr;
	delete lap;
}
