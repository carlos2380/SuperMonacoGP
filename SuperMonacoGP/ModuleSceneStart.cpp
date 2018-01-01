#include "Globals.h"
#include "Application.h"
#include "ModuleTextures.h"
#include "ModuleAudio.h"
#include "ModuleInput.h"
#include "ModuleRender.h"
#include "ModuleFadeToBlack.h"
#include "ModuleSceneStart.h"
#include <SDL.h>


ModuleSceneStart::ModuleSceneStart(bool active) : Module(active)
{
	background = { 0, 0, 320, 240 };
	sega = { 0, 248, 120, 8 };
	girl = { 200, 240, 100, 199 };
	poster = { 56, 40, 128, 172 };

	start = new Animation();
	start->frames.push_back({ 0, 240, 144, 8 });
	start->frames.push_back({ 0, 0, 0, 0 });
	start->speed = 0.03f;

	carsRect = vector<SDL_Rect>(5);
	cars = vector<car*>(2);

	for(int i = 0; i < carsRect.size(); ++i)
	{
		carsRect[i].x = 0;
		carsRect[i].y = 256 + (i * 43);
		carsRect[i].w = 160;
		carsRect[i].h = 43;
	}

}

ModuleSceneStart::~ModuleSceneStart()
{}

// Load assets
bool ModuleSceneStart::Start()
{
	LOG("Loading space intro");

	startSprites = App->textures->Load("Sprites/StartSprites.png", 255, 0 , 255);
	App->audio->PlayMusic("Sounds/StartMusic.wav", 0.0f);
	/*if (fx == 0)
		fx = App->audio->LoadFx("rtype/starting.wav");*/

	App->renderer->camera.x = App->renderer->camera.y = 0;

	return true;
}

// UnLoad assets
bool ModuleSceneStart::CleanUp()
{
	LOG("Unloading space scene");
	//delete start;
	App->textures->Unload(startSprites);

	return true;
}

// Update: draw background
update_status ModuleSceneStart::Update()
{
	App->renderer->Blit(startSprites, 0, 0, &background);
	renderCars();
	App->renderer->Blit(startSprites, 184, 12, &girl);
	App->renderer->Blit(startSprites, 56, 40, &poster);
	App->renderer->Blit(startSprites, 88, 128, &(start->GetCurrentFrame()));
	App->renderer->Blit(startSprites, 176, 192, &sega);
	if (App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN && App->fade->isFading() == false)
	{
		App->fade->FadeToBlack((Module*)App->scene_select, this);
		//App->audio->PlayFx(fx);
	}

	return UPDATE_CONTINUE;
}

void ModuleSceneStart::renderCars()
{
	Uint32 ticks = SDL_GetTicks();
	for (int i = 0; i < cars.size(); ++i)
	{
		if(cars[i] == nullptr)
		{
			if(rand()%100 == 0)
			{
				cars[i] = new car();
				cars[i]->x = 320;
				cars[i]->y = 150 + (i*15);
				cars[i]->carSprite = &(carsRect[(rand() % 5)]);
				cars[i]->speed = rand()%10 +8;
			}
		}else
		{
				if(cars[i]->x < -160)
				{
					cars[i] = nullptr;
				}else
				{
						(cars[i]->x) -= cars[i]->speed;
						App->renderer->Blit(startSprites, cars[i]->x, cars[i]->y, cars[i]->carSprite);
				}
		}
	}
	
}
