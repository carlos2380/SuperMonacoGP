#include "Globals.h"
#include "Application.h"
#include "ModuleTextures.h"
#include "ModuleAudio.h"
#include "ModuleInput.h"
#include "ModuleRender.h"
#include "ModuleFadeToBlack.h"
#include "ModuleSceneSelect.h"
#include "TextFont.h"
#include <SDL.h>
#include "ResultBridgeScenes.h"


ModuleSceneSelect::ModuleSceneSelect(bool active) : Module(active)
{
	background = { 0, 0, 320, 240 };
	backgroundSelect = { 0, 240, 320, 240 };
	padSelected = { 20, 124, 72, 64 };
	padUnselected = { 124, 124, 72, 64 };

	perimeterSelected = new Animation();
	perimeterSelected->frames.push_back({ 0, 480, 102, 156 });
	perimeterSelected->frames.push_back({ 102, 480, 102, 156 });
	perimeterSelected->frames.push_back({ 204, 480, 102, 156 });
	perimeterSelected->frames.push_back({ 306, 480, 102, 156 });
	perimeterSelected->speed = 0.5f;

	modeSelect = AUTOMATIC;
	
}

ModuleSceneSelect::~ModuleSceneSelect()
{}

// Load assets
bool ModuleSceneSelect::Start()
{
	LOG("Loading space intro");

	startTicks = SDL_GetTicks();
	time = 30;
	delay = 1000;
	selectSprites = App->textures->Load("Sprites/SelectSprites.png", 255, 0, 255);
	textFont = new TextFont(selectSprites, "0123456789", 0, 636, 8, 15);

	App->audio->PlayMusic("Sounds/SelectMusic.wav", 0.0f);
	if (fx == 0) fx = App->audio->LoadFx("Sounds/SelectEffect.wav");

	App->renderer->camera.x = App->renderer->camera.y = 0;

	return true;
}

// UnLoad assets
bool ModuleSceneSelect::CleanUp()
{
	LOG("Unloading space scene");
	App->textures->Unload(selectSprites);
	return true;
}

// Update: draw background
update_status ModuleSceneSelect::Update()
{
	if (App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_DOWN
		|| App->input->GetKey(SDL_SCANCODE_A) == KEY_DOWN)
	{
		if (modeSelect == MANUAL) modeSelect = AUTOMATIC;
		else if (modeSelect == SUPER) modeSelect = MANUAL;
	}
	if (App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_DOWN
		|| App->input->GetKey(SDL_SCANCODE_D) == KEY_DOWN)
	{
		if (modeSelect == AUTOMATIC) modeSelect = MANUAL;
		else if (modeSelect == MANUAL) modeSelect = SUPER;
	}


	App->renderer->Blit(selectSprites, 0, 0, &background);
	App->renderer->Blit(selectSprites, 20, 124, &padUnselected);
	renderSelected();
	App->renderer->Blit(selectSprites, 0, 0, &backgroundSelect);

	if((SDL_GetTicks() - startTicks) > delay)
	{
		--time;
		startTicks = SDL_GetTicks();
	}
	textFont->print(285, 14, to_string(time));

	if (App->input->GetKey(SDL_SCANCODE_RETURN) == KEY_DOWN && App->fade->isFading() == false || time < 0)
	{
		ResultBridgeScenes::getInstance()->gearSelected = modeSelect;
		App->audio->StopMusic();
		App->fade->FadeToBlack((Module*)App->scene_race, this);
		App->audio->PlayFx(fx);
	}

	return UPDATE_CONTINUE;
}

void ModuleSceneSelect::renderSelected()
{
	switch (modeSelect)
	{
		case AUTOMATIC:
			App->renderer->Blit(selectSprites, 20, 124, &padSelected);
			App->renderer->Blit(selectSprites, 5, 36, &(perimeterSelected->GetCurrentFrame()));
			break;
		case MANUAL:
			App->renderer->Blit(selectSprites, 124, 124, &padSelected);
			App->renderer->Blit(selectSprites, 110, 36, &(perimeterSelected->GetCurrentFrame()));
			break;
		case SUPER:
			App->renderer->Blit(selectSprites, 228, 124, &padSelected);
			App->renderer->Blit(selectSprites, 213, 36, &(perimeterSelected->GetCurrentFrame()));
			break;
		default:
			App->renderer->Blit(selectSprites, 20, 124, &padSelected);
			App->renderer->Blit(selectSprites, 5, 36, &(perimeterSelected->GetCurrentFrame()));
	}
}
