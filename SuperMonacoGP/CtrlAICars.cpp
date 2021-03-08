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

	int n = 5;
	AICars = vector<AICar*>(n);
	mapLines = &(App->scene_race->ctrlMap->mapLines);
	mapMirror = &(App->scene_race->ctrlMap->mapMirror);
	sizeMap = App->scene_race->ctrlMap->sizeMap;

	for (int i = 0; i < n; ++i) {
		AICars[i] = new AICar();
		AICars[i]->tex = texSprite;
		AICars[i]->linePos = 6 + 6*(i/2);
		(*mapLines)[AICars[i]->linePos].addCar(AICars[i]);
		AICars[i]->line = &(*mapLines)[AICars[i]->linePos];
		AICars[i]->sprite = CarAIVector[i % 2];
		AICars[i]->spriteX = (i % 2) == 0 ? -0.5f : 0.5f;
		AICars[i]->speed = 6+i;
	}
	
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
	int mapPosition = App->scene_race->ctrlMap->mapPosition;
	for (int i = 0; i < AICars.size(); ++i) {
		int nextLine = AICars[i]->getNextLine();

		if (nextLine > 0) {
			if (nextLine >= sizeMap) ++AICars[i]->lap;
			AICars[i]->line->removeCar(AICars[i]);
			AICars[i]->linePos = ((AICars[i]->linePos + 1) % sizeMap);
			if (mapPosition < AICars[i]->linePos && App->scene_race->lap <= AICars[i]->lap) {
				Line* line = &(*mapLines)[AICars[i]->linePos];
				line->addCar(AICars[i]);
				AICars[i]->line = line;
				if (line->curve == 0) {
					AICars[i]->sprite = CarAIVector[0 + AICars[i]->animSprite];
				}
				else if (line->curve > 0 && line->curve < 30) {
					AICars[i]->sprite = CarAIVector[2 + AICars[i]->animSprite];
				}
				else if (line->curve > 0) {
					AICars[i]->sprite = CarAIVector[4 + AICars[i]->animSprite];
				}
				else if (line->curve <= -30) {
					AICars[i]->sprite = CarAIVector[8 + AICars[i]->animSprite];
				}
				else {
					AICars[i]->sprite = CarAIVector[6 + AICars[i]->animSprite];
				}
			}
			else {
				int linePos = (*mapMirror).size() - AICars[i]->linePos - 1;
				Line* line = &(*mapMirror)[linePos];
				line->addCar(AICars[i]);
				AICars[i]->line = line;
				if (line->curve == 0) {
					AICars[i]->sprite = CarAIMirrorVector[0 + AICars[i]->animSprite];
				}
				else if (line->curve > 0) {
					AICars[i]->sprite = CarAIMirrorVector[2 + AICars[i]->animSprite];
				}
				else {
					AICars[i]->sprite = CarAIMirrorVector[4 + AICars[i]->animSprite];
				}
			}
			AICars[i]->animSprite = AICars[i]->animSprite ? 0 : 1;
		}
	}
	
	return UPDATE_CONTINUE;
}
