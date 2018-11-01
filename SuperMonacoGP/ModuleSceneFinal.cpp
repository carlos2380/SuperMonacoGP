#include "Globals.h"
#include "Application.h"
#include "ModuleTextures.h"
#include "ModuleAudio.h"
#include "ModuleInput.h"
#include "ModuleRender.h"
#include "ModuleFadeToBlack.h"
#include "ModuleSceneFinal.h"
#include "TextFont.h"
#include <SDL.h>
#include "ResultBridgeScenes.h"


ModuleSceneFinal::ModuleSceneFinal(bool active) : Module(active)
{
	background = { 400, 150, 320, 240 };
	

}

ModuleSceneFinal::~ModuleSceneFinal()
{}

// Load assets
bool ModuleSceneFinal::Start()
{
	LOG("Loading space intro");

	background = { 400, 150, 320, 240 };
	finalSprites = App->textures->Load("Sprites/RaceSprites.bmp", 255, 0, 255);
	textFont = new TextFont(finalSprites, "0123456789\"'", 0, 940, 8, 15);
	lap1 = ResultBridgeScenes::getInstance()->lap1Time;
	lap2 = ResultBridgeScenes::getInstance()->lap2Time;
	lap3 = ResultBridgeScenes::getInstance()->lap3Time;
	total = ResultBridgeScenes::getInstance()->totalTime;
	
	App->renderer->camera.x = App->renderer->camera.y = 0;

	return true;
}

// UnLoad assets
bool ModuleSceneFinal::CleanUp()
{
	LOG("Unloading space scene");
	App->textures->Unload(finalSprites);
	delete textFont;
	textFont = nullptr;
	return true;
}

// Update: draw background
update_status ModuleSceneFinal::Update()
{
	

	App->renderer->Blit(finalSprites, 0, 0, &background);
	
	textFont->print(150, 144, lap1);
	textFont->print(150, 160, lap2);
	textFont->print(150, 176, lap3);
	textFont->print(150, 200, total);

	if (App->input->GetKey(SDL_SCANCODE_RETURN) == KEY_DOWN && App->fade->isFading() == false )
	{
		//ResultBridgeScenes::getInstance()->gearSelected = modeSelect;
	
		App->fade->FadeToBlack((Module*)App->scene_start, this);
		
	}

	return UPDATE_CONTINUE;
}
