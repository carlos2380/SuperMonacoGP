#include "Globals.h"
#include "Application.h"
#include "ModuleTextures.h"
#include "ModuleAudio.h"
#include "ModuleInput.h"
#include "ModuleRender.h"
#include "ModuleFadeToBlack.h"
#include "ModuleSceneRace.h"
#include <SDL.h>
#include "SDL2_gfxPrimitives.h"
#include <ostream>
#include <iostream>
#include "ResultBridgeScenes.h"


//Get mouse position

ModuleSceneRace::ModuleSceneRace(bool active) : Module(active)
{
	background = { 0, 72, 320, 240 };
}

ModuleSceneRace::~ModuleSceneRace()
{}

// Load assets
bool ModuleSceneRace::Start()
{
	LOG("Loading space intro");

	raceSprites = App->textures->Load("Sprites/RaceSprites.bmp", 255, 0, 255);

	ctrlMap = new CtrlMap;
	ModuleSceneRace* moduleRace = this;
	ctrlMap.Start(moduleRace);

	ctrlCar = new CtrlCar(raceSprites);
	ctrlCar.Start();

	ctrlUI = new CtrlUI();
	ctrlUI.ctrlCar = &ctrlCar;
	ctrlUI.Start(moduleRace);
	
	lap = 0;
	position = 10;
	positionLimit = 16;
	stopCar = true;

	totalTime = 0;
	firstLap = 0;
	secondLap = 0;
	thirdLap = 0;
	timeToFirst = 0;
	distanceTime = 0;
	bestLap = 90000;
	initialTime = SDL_GetTicks();
	App->renderer->camera.x = App->renderer->camera.y = 0;

	return true;
}

// UnLoad assets
bool ModuleSceneRace::CleanUp()
{
	LOG("Unloading space scene");
	ctrlCar.CleanUp();
	ctrlUI.CleanUp();
	ctrlMap.CleanUp();
	App->textures->Unload(raceSprites);
	return true;
}


update_status ModuleSceneRace::Update()
{
	if(lap == 0)
	{
		if((SDL_GetTicks() - initialTime) < 3000)
		{
			printSemaforo = true;
			semaforoState = 0;
		}else if ((SDL_GetTicks() - initialTime) < 5000)
		{
			printSemaforo = true;
			semaforoState = 1;
		}
		else if ((SDL_GetTicks() - initialTime) < 7000)
		{
			stopCar = false;
			printSemaforo = true;
			semaforoState = 2;
		}
		else { printSemaforo = false; }
	}

	ctrlMap.Update();
	App->renderer->Blit(raceSprites, 0, 0, &background);
	ctrlCar.Update();

	Uint32 now = SDL_GetTicks();
	
	switch (lap)
	{
		case 1:
			firstLap = now - initialTime;
			totalTime = now - initialTime;
		break;
		case 2:
			secondLap = now - initialTime;
			totalTime = firstLap + now - initialTime;
			break;
		case 3:
			thirdLap = now - initialTime;
			totalTime = firstLap + secondLap+ now -initialTime;
			break;
	}

	ctrlUI.Update();

	/*if (App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN && App->fade->isFading() == false)
	{
		App->fade->FadeToBlack((Module*)App->scene_start, this);
		//App->audio->PlayFx(fx);
	}*/

	return UPDATE_CONTINUE;
}

void ModuleSceneRace::nextLap()
{
	
	initialTime = SDL_GetTicks();
	
	++lap;
	switch (lap)
	{
		case 2:
			if (firstLap < bestLap) bestLap = firstLap;
			break;
		case 3:
			if (secondLap < bestLap) bestLap = secondLap;
			break;
	}
	if(lap == 4){
		string s;
		ctrlUI.millisecTomssdd(firstLap, s);
		ResultBridgeScenes::getInstance()->lap1Time = s;
		ctrlUI.millisecTomssdd(secondLap, s);
		ResultBridgeScenes::getInstance()->lap2Time = s;
		ctrlUI.millisecTomssdd(thirdLap, s);
		ResultBridgeScenes::getInstance()->lap3Time = s;
		ctrlUI.millisecTomssdd(totalTime, s);
		ResultBridgeScenes::getInstance()->totalTime = s;
		App->fade->FadeToBlack((Module*)App->scene_final, this);
	}
}
