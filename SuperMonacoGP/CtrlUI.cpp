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
#include "AICar.h"
#include "TextFont.h"
#include "json.hpp"
#include <fstream>

using namespace std;
using json = nlohmann::json;

CtrlUI::CtrlUI(bool active) : Module(active)
{
}



CtrlUI::~CtrlUI()
{}

// Load assets
bool CtrlUI::Start()
{

	uISprites = App->textures->Load("Sprites/RaceSprites.bmp", 255, 0, 255);

	loadMiniMap();
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
	return true;
}

update_status CtrlUI::Update()
{
	/*int mx, my;
	SDL_GetMouseState(&mx, &my);
	//cout << mx << ": " << my << endl;*/

	++tick;

	printSpeed();
	
	if(App->scene_race->printSemaforo == true)
	{
		App->renderer->Blit(uISprites, 2, 85, semaforo[App->scene_race->semaforoState]);
	}
	printGear();
	printPositions();
	printTimes();
	printMiniMap();
	App->renderer->BlitRotate(uISprites, 17, 88, gearMark, App->scene_race->ctrlCar->revol, centerRotate);
	speedTime->print(288, 70, to_string(App->scene_race->lap));

	return UPDATE_CONTINUE;
}

void CtrlUI::loadMiniMap()
{
	ifstream ifile("Files/MiniMap.json");
	json jsn;
	ifile >> jsn;
	ifile.close();

	list<json> posintionJson = jsn["MiniMapPos"];
	int iter = 0;
	miniMapPos = vector<pair<int, int>>(posintionJson.size());

	for (list<json>::iterator it = posintionJson.begin(); it != posintionJson.end(); ++it) {
		
		miniMapPos[iter].first = (*it).at("x");
		miniMapPos[iter].second = (*it).at("y");
		++iter;
	}
	
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

	carAIMiniMap = new SDL_Rect();
	carAIMiniMap->x = 209;
	carAIMiniMap->y = 825;
	carAIMiniMap->w = 5;
	carAIMiniMap->h = 6;
}


void CtrlUI::printSpeed()
{
	int speed = App->scene_race->ctrlCar->speed;
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
	switch (App->scene_race->ctrlCar->gear)
	{
	case AUTOMATIC:
		App->renderer->Blit(uISprites, 3, 112, automatic);
		break;
	case MANUAL:
		App->renderer->Blit(uISprites, 10, 112, manual[App->scene_race->ctrlCar->gearSelect-1]);
		break;
	case SUPER:
		App->renderer->Blit(uISprites, 10, 112, super[App->scene_race->ctrlCar->gearSelect-1]);
		break;
	}
}

void CtrlUI::printPositions()
{
	string posit = "";
	string positLimit = "";
	intToStringTens(App->scene_race->position, posit);
	intToStringTens(App->scene_race->positionLimit, positLimit);

	if(App->scene_race->position < App->scene_race->positionLimit)
	{
		position->print(139, 101, posit);
		positionLimit->print(144, 76, positLimit);
	} else if(App->scene_race->position == App->scene_race->positionLimit)
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
	millisecTomssdd(App->scene_race->totalTime, time);
	speedTime->print(247, 8, time);
	time = "";
	millisecTossdd(App->scene_race->distanceTime, time);
	speedTime->print(24, 8, time);

	string lap1 = "";
	millisecTomssdd(App->scene_race->firstLap, lap1);
	string lap2 = "";
	millisecTomssdd(App->scene_race->secondLap, lap2);
	string lap3 = "";
	millisecTomssdd(App->scene_race->thirdLap, lap3);
	string bestLap = "";
	millisecTomssdd(App->scene_race->bestLap, bestLap);

	switch (App->scene_race->lap)
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
	delete carAIMiniMap;
	carAIMiniMap = nullptr;
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
	int posit;
	vector<AICar*>* carsAI = &(App->scene_race->ctrlAICars->AICars);
	for (auto carAI : *carsAI) {
		posit = carAI->linePos;
		App->renderer->Blit(uISprites, miniMapPos[posit / 16].first, miniMapPos[posit / 16].second, carAIMiniMap);
	}

	posit = App->scene_race->ctrlMap->mapPosition;
	App->renderer->Blit(uISprites, miniMapPos[posit / 16].first, miniMapPos[posit / 16].second, positionMiniMap);
}