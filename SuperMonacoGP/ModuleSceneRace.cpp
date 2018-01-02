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

int roadW = SCREEN_WIDTH;
int segL = 200;
float camD = 0.84;

struct Line
{
	float x, y, z;
	float X, Y, W;
	float scale;

	Line() { x = y = z = 0; }

	//from world to screen coordinates
	void project(int camX, int camY, int camZ)
	{
		scale = camD / (z - camZ);
		X = (1 + scale*(x - camX)) * SCREEN_WIDTH/ 2;
		Y = (1 + scale*(Y - camY)) * SCREEN_HEIGHT / 2;
		W = scale * roadW * SCREEN_WIDTH / 2;
	}
};

vector<Line> lines;
int N;
//Get mouse position

ModuleSceneRace::ModuleSceneRace(bool active) : Module(active)
{
}

ModuleSceneRace::~ModuleSceneRace()
{}

// Load assets
bool ModuleSceneRace::Start()
{
	LOG("Loading space intro");

	for (int i = 0; i<1600; ++i)
	{
		Line line;
		line.z = i*segL;

		lines.push_back(line);
	}

	N = lines.size();
	selectSprites = App->textures->Load("Sprites/SelectSprites.png", 255, 0, 255);

	/*if (fx == 0)
	fx = App->audio->LoadFx("rtype/starting.wav");*/

	App->renderer->camera.x = App->renderer->camera.y = 0;

	return true;
}

// UnLoad assets
bool ModuleSceneRace::CleanUp()
{
	LOG("Unloading space scene");
	App->textures->Unload(selectSprites);
	return true;
}

// Update: draw background
update_status ModuleSceneRace::Update()
{

	/*for(int n = 1; n < 300; ++n)
	{
		Line &l = lines[n%N];
		l.project(0, 1500, 0);
		
		SDL_Color grass;
		(n / 2) % 2 ? grass = { 16, 200, 16, 255 } : grass = { 0, 154, 0, 255 };
		SDL_Color rumble;
		(n / 2) % 2 ? rumble = { 255, 255, 255, 255 } : rumble = { 0, 0, 0, 255 };
		SDL_Color road;
		(n / 2) % 2 ? road = { 107, 107, 107, 255 } : road = { 105, 105, 105, 255 };

		Line p = lines[(n-1) % N]; //previous line

		drawQuad(0, SCREEN_HEIGHT - p.Y, SCREEN_WIDTH, 0, SCREEN_HEIGHT - l.Y, SCREEN_WIDTH, grass.r, grass.g, grass.b);
		drawQuad(p.X, SCREEN_HEIGHT - p.Y, p.W*1.2, l.X, SCREEN_HEIGHT - l.Y, p.W*1.2, rumble.r, rumble.g, rumble.b);
		drawQuad(p.X, SCREEN_HEIGHT - p.Y, p.W, l.X, l.Y, SCREEN_HEIGHT - l.W, road.r, road.g, road.b);
	}*/

	App->renderer->DrawPointScalable(100, 100, 255, 255, 255, 255);
	if (App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN && App->fade->isFading() == false)
	{
		App->fade->FadeToBlack((Module*)App->scene_start, this);
		//App->audio->PlayFx(fx);
	}

	return UPDATE_CONTINUE;
}

void ModuleSceneRace::drawQuad(int x1, int y1, int w1, int x2, int y2, int w2, int r, int g, int b)
{
	short x[4] = { x1-w1, x2-w2, x2+w2, x1+w1 };
	short y[4] = { y1, y2, y2, y1 };
	filledPolygonRGBA(App->renderer->renderer, x, y, 4, r, g, b, 255);
}