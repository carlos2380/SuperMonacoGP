#include "CtrlAICars.h"
#include "Globals.h"
#include <fstream>
#include "AICar.h"
#include "Line.h"
#include "ModuleTextures.h"
#include <list>
#include "json.hpp"
#include "ModuleSceneRace.h"
#include "Application.h"
using namespace std;
using json = nlohmann::json;

CtrlAICars::CtrlAICars(bool active) : Module(active)
{
}


CtrlAICars::~CtrlAICars()
{}


bool CtrlAICars::Start() {

	texSprite = App->textures->Load("Sprites/RaceSprites.bmp", 255, 0, 255);

	ifstream fileCarsAI("Files/CarsAI.json");
	json jsnCarsAI;
	fileCarsAI >> jsnCarsAI;
	fileCarsAI.close();
	list<json> carsAIJson = jsnCarsAI["AICar"];
	list<json> carsAIMirrorJson = jsnCarsAI["AICarMirror"];
	CarAIVector = vector<SDL_Rect*>(carsAIJson.size());
	CarAIMirrorVector = vector<SDL_Rect*>(carsAIMirrorJson.size());
	int j = 0;
	for (list<json>::iterator it = carsAIJson.begin(); it != carsAIJson.end(); ++it)
	{
		CarAIVector[j] = new SDL_Rect();
		CarAIVector[j]->x = (*it).at("x");
		CarAIVector[j]->y = (*it).at("y");
		CarAIVector[j]->w = (*it).at("w");
		CarAIVector[j]->h = (*it).at("h");
		++j;
	}
	j = 0;
	for (list<json>::iterator it = carsAIMirrorJson.begin(); it != carsAIMirrorJson.end(); ++it)
	{
		CarAIMirrorVector[j] = new SDL_Rect();
		CarAIMirrorVector[j]->x = (*it).at("x");
		CarAIMirrorVector[j]->y = (*it).at("y");
		CarAIMirrorVector[j]->w = (*it).at("w");
		CarAIMirrorVector[j]->h = (*it).at("h");
		++j;
	}
	AICars = vector<AICar*>(1);
	AICars[0] = new AICar();
	
	mapLines = &(App->scene_race->ctrlMap->mapLines);
	mapMirror = &(App->scene_race->ctrlMap->mapMirror);
	sizeMap = App->scene_race->ctrlMap->sizeMap;

	AICars[0]->tex = texSprite;
	AICars[0]->linePos = 0;
	(*mapLines)[AICars[0]->linePos].addCar(AICars[0]);
	AICars[0]->line = &(*mapLines)[AICars[0]->linePos];
	AICars[0]->sprite = CarAIVector[0];

	return true;
}

// UnLoad assets
bool CtrlAICars::CleanUp()
{
	App->textures->Unload(texSprite);
	texSprite = nullptr;

	LOG("Unloading space scene");
	for (int i = 0; i < CarAIVector.size(); ++i)
	{
		delete[] CarAIVector[i];
	}
	CarAIVector.clear();
	for (int i = 0; i < CarAIMirrorVector.size(); ++i)
	{
		delete[] CarAIMirrorVector[i];
	}
	CarAIMirrorVector.clear();
	return true;
}

int speed = 0;
update_status CtrlAICars::Update() {
	++speed;
	if(speed%3 == 0) {
		AICars[0]->line->removeCar(AICars[0]);
		++AICars[0]->linePos%sizeMap;

		if (App->scene_race->ctrlMap->mapPosition < AICars[0]->linePos && App->scene_race->lap <= AICars[0]->lap) {
			(*mapLines)[AICars[0]->linePos].addCar(AICars[0]);
			AICars[0]->line = &(*mapLines)[AICars[0]->linePos];
			AICars[0]->sprite = CarAIVector[AICars[0]->linePos % 2];
		} else {
			int linePos = (*mapMirror).size() - AICars[0]->linePos - 1;
			(*mapMirror)[linePos].addCar(AICars[0]);
			AICars[0]->line = &(*mapMirror)[linePos];
			AICars[0]->sprite = CarAIMirrorVector[AICars[0]->linePos % 2];
		}
	}
	
	return UPDATE_CONTINUE;
}
