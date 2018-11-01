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
	App->textures->Unload(uISprites);
	uISprites = nullptr;
	ctrlCar = nullptr;
	return true;
}

update_status CtrlUI::Update()
{
	/*int mx, my;
	SDL_GetMouseState(&mx, &my);
	//cout << mx << ": " << my << endl;*/

	++tick;

	printSpeed();
	
	if(moduleRace->printSemaforo == true)
	{
		App->renderer->Blit(uISprites, 2, 85, semaforo[moduleRace->semaforoState]);
	}
	printGear();
	printPositions();
	printTimes();
	printMiniMap();
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

	semaforo = vector<SDL_Rect*>(3);
	for (int i = 0; i < semaforo.size(); ++i)
	{
		SDL_Rect* rect = new SDL_Rect();
		rect->x = 200;
		rect->y = 350 + (i * 40);
		rect->w = 96;
		rect->h = 40;
		semaforo[i] = rect;
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

	positionMiniMap = new SDL_Rect();
	positionMiniMap->x = 200;
	positionMiniMap->y = 825;
	positionMiniMap->w = 6;
	positionMiniMap->h = 7;
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
	string bestLap = "";
	millisecTomssdd(moduleRace->bestLap, bestLap);

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
	lap->print(28, 31, bestLap);
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
	delete automatic;
	automatic = nullptr;
	delete gearMark;
	gearMark = nullptr;
	delete limitOver;
	limitOver = nullptr;
	delete positionMiniMap;
	positionMiniMap = nullptr;
	delete centerRotate;
	centerRotate = nullptr;
	for (int i = 0; i < manual.size(); ++i)
	{
		delete[] manual[i];
		manual[i] = nullptr;
		
	}

	for (int i = 0; i < super.size(); ++i)
	{
		delete[] super[i];
		super[i] = nullptr;
	}

	for (int i = 0; i < semaforo.size(); ++i)
	{
		delete[] semaforo[i];
		semaforo[i] = nullptr;
	}

}

void CtrlUI::unloadTextFonts()
{
	delete position;
	position = nullptr;
	delete positionRed;
	positionRed = nullptr;
	delete positionLimit;
	positionLimit = nullptr;
	delete positionLimitRed;
	positionLimitRed = nullptr;
	delete speedTime;
	speedTime = nullptr;
	delete lapYellow;
	lapYellow = nullptr;
	delete lap;
	lap = nullptr;
}

void CtrlUI::printMiniMap()
{
	int posit = moduleRace->ctrlMap.mapPosition;
	if (posit < 300) {
		int yMap = ((float)(posit/10 )*(float)(2.0f / 3.0f) + 245)-71;
		App->renderer->Blit(uISprites, 303, yMap, positionMiniMap);
	}
	else if (posit < 350) App->renderer->Blit(uISprites, 302, 199, positionMiniMap);
	else if (posit < 400) App->renderer->Blit(uISprites, 301, 200, positionMiniMap);
	else if (posit < 420) App->renderer->Blit(uISprites, 300, 201, positionMiniMap);
	else if (posit < 450) App->renderer->Blit(uISprites, 298, 200, positionMiniMap);
	else if (posit < 480) App->renderer->Blit(uISprites, 296, 200, positionMiniMap);
	else if (posit < 500) App->renderer->Blit(uISprites, 294, 199, positionMiniMap);
	else if (posit < 520) App->renderer->Blit(uISprites, 293, 198, positionMiniMap);
	else if (posit < 540) App->renderer->Blit(uISprites, 292, 196, positionMiniMap);
	else if (posit < 550) App->renderer->Blit(uISprites, 292, 194, positionMiniMap);
	else if (posit < 600) App->renderer->Blit(uISprites, 292, 192, positionMiniMap);
	else if (posit < 650) App->renderer->Blit(uISprites, 292, 190, positionMiniMap);
	else if (posit < 700) App->renderer->Blit(uISprites, 293, 188, positionMiniMap);
	else if (posit < 750) App->renderer->Blit(uISprites, 293, 185, positionMiniMap);
	else if (posit < 800) App->renderer->Blit(uISprites, 292, 182, positionMiniMap);
	else if (posit < 850) App->renderer->Blit(uISprites, 291, 179, positionMiniMap);
	else if (posit < 900) App->renderer->Blit(uISprites, 291, 176, positionMiniMap);
	else if (posit < 950) App->renderer->Blit(uISprites, 293, 172, positionMiniMap);
	else if (posit < 1000) App->renderer->Blit(uISprites, 295, 172, positionMiniMap);
	else if (posit < 1200) {
		int yMap = (239 - ((posit - 1000)*(14.0f / 200)))-71;
		App->renderer->Blit(uISprites, 297, yMap, positionMiniMap);
	}
	else if (posit < 1230) App->renderer->Blit(uISprites, 297, 152, positionMiniMap);
	else if (posit < 1250) App->renderer->Blit(uISprites, 296, 149, positionMiniMap);
	else if (posit < 1275) App->renderer->Blit(uISprites, 294, 148, positionMiniMap);
	else if (posit < 1300) App->renderer->Blit(uISprites, 292, 147, positionMiniMap);
	else if (posit < 1320) App->renderer->Blit(uISprites, 291, 146, positionMiniMap);
	else if (posit < 1340) App->renderer->Blit(uISprites, 290, 146, positionMiniMap);
	else if (posit < 1370) App->renderer->Blit(uISprites, 287, 144, positionMiniMap);
	else if (posit < 1400) App->renderer->Blit(uISprites, 285, 143, positionMiniMap);
	else if (posit < 1420) App->renderer->Blit(uISprites, 284, 142, positionMiniMap);
	else if (posit < 1440) App->renderer->Blit(uISprites, 283, 142, positionMiniMap);
	else if (posit < 1500) App->renderer->Blit(uISprites, 280, 140, positionMiniMap);
	else if (posit < 1550) App->renderer->Blit(uISprites, 279, 139, positionMiniMap);
	else if (posit < 1600) App->renderer->Blit(uISprites, 274, 138, positionMiniMap);
	else if (posit < 1650) App->renderer->Blit(uISprites, 267, 136, positionMiniMap);
	else if (posit < 1700) App->renderer->Blit(uISprites, 263, 134, positionMiniMap);
	else if (posit < 1750) App->renderer->Blit(uISprites, 258, 131, positionMiniMap);
	else if (posit < 1800) App->renderer->Blit(uISprites, 260, 126, positionMiniMap);
	else if (posit < 1850) App->renderer->Blit(uISprites, 264, 122, positionMiniMap);
	else if (posit < 1870) App->renderer->Blit(uISprites, 268, 122, positionMiniMap);
	else if (posit < 1900) App->renderer->Blit(uISprites, 271, 127, positionMiniMap);
	else if (posit < 1920) App->renderer->Blit(uISprites, 275, 132, positionMiniMap);
	else if (posit < 1950) App->renderer->Blit(uISprites, 280, 136, positionMiniMap);
	else if (posit < 1980) App->renderer->Blit(uISprites, 285, 136, positionMiniMap);
	else if (posit < 2000) App->renderer->Blit(uISprites, 288, 135, positionMiniMap);
	else if (posit < 2025) App->renderer->Blit(uISprites, 289, 134, positionMiniMap);
	else if (posit < 2050) App->renderer->Blit(uISprites, 290, 132, positionMiniMap);
	else if (posit < 2075) App->renderer->Blit(uISprites, 292, 130, positionMiniMap);
	else if (posit < 2100) App->renderer->Blit(uISprites, 292, 128, positionMiniMap);
	else if (posit < 2125) App->renderer->Blit(uISprites, 295, 123, positionMiniMap);
	else if (posit < 2150) App->renderer->Blit(uISprites, 296, 122, positionMiniMap);
	else if (posit < 2175) App->renderer->Blit(uISprites, 299, 122, positionMiniMap);
	else if (posit < 2200) App->renderer->Blit(uISprites, 299, 124, positionMiniMap);
	else if (posit < 2250) App->renderer->Blit(uISprites, 299, 128, positionMiniMap);
	else if (posit < 2300) App->renderer->Blit(uISprites, 299, 133, positionMiniMap);
	else if (posit < 2350) App->renderer->Blit(uISprites, 300, 138, positionMiniMap);
	else if (posit < 2400) App->renderer->Blit(uISprites, 302, 141, positionMiniMap);
	else
	{
		int yMap = (213 + ((posit - 2400)*(29.0f / 300))) - 71;
		App->renderer->Blit(uISprites, 303, yMap, positionMiniMap);
	}
}