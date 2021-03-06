#include "Globals.h"
#include "Application.h"
#include "ModuleRender.h"
#include "ModuleWindow.h"
#include "ModuleInput.h"
#include "SDL.h"
#include <vector>
using namespace std;

ModuleRender::ModuleRender()
{
	camera.x = camera.y = 0;
	camera.w = SCREEN_WIDTH * SCREEN_SIZE;
	camera.h = SCREEN_HEIGHT* SCREEN_SIZE;
}

// Destructor
ModuleRender::~ModuleRender()
{}

// Called before render is available
bool ModuleRender::Init()
{
	LOG("Creating Renderer context");
	bool ret = true;
	Uint32 flags = 0;

	if (VSYNC == true)
	{
		flags |= SDL_RENDERER_PRESENTVSYNC;
	}

	renderer = SDL_CreateRenderer(App->window->window, -1, flags);

	if (renderer == nullptr)
	{
		LOG("Renderer could not be created! SDL_Error: %s\n", SDL_GetError());
		ret = false;
	}

	return ret;
}

update_status ModuleRender::PreUpdate()
{
	SDL_SetRenderDrawColor(renderer, 170, 238, 238, SDL_ALPHA_OPAQUE);
	SDL_RenderClear(renderer);
	return UPDATE_CONTINUE;
}

// Called every draw update
update_status ModuleRender::Update()
{
	// debug camera
	/*int speed = 1;

	if (App->input->GetKey(SDL_SCANCODE_UP) == KEY_REPEAT)
		App->renderer->camera.y += speed;

	if (App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_REPEAT)
		App->renderer->camera.y -= speed;

	if (App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT)
		App->renderer->camera.x += speed;

	if (App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT)
		App->renderer->camera.x -= speed;*/

	return UPDATE_CONTINUE;
}

update_status ModuleRender::PostUpdate()
{
	SDL_RenderPresent(renderer);
	return UPDATE_CONTINUE;
}

// Called before quitting
bool ModuleRender::CleanUp()
{
	LOG("Destroying renderer");

	//Destroy window
	if (renderer != nullptr)
	{
		SDL_DestroyRenderer(renderer);
	}

	return true;
}

// Blit to screen
bool ModuleRender::Blit(SDL_Texture* texture, int x, int y, SDL_Rect* section, float speed, float scale)
{
	bool ret = true;
	SDL_Rect rect;
	rect.x = (int)(camera.x * speed) + x * SCREEN_SIZE;
	rect.y = (int)(camera.y * speed) + y * SCREEN_SIZE;

	if (section != NULL)
	{
		rect.w = section->w;
		rect.h = section->h;
	}
	else
	{
		SDL_QueryTexture(texture, NULL, NULL, &rect.w, &rect.h);
	}

	rect.w *= SCREEN_SIZE * scale;
	rect.h *= SCREEN_SIZE * scale;

	if (SDL_RenderCopy(renderer, texture, section, &rect) != 0)
	{
		LOG("Cannot blit to screen. SDL_RenderCopy error: %s", SDL_GetError());
		ret = false;
	}

	return ret;
}

bool ModuleRender::BlitScaled(SDL_Texture* texture, int x, int y, SDL_Rect* section, float speed, float scale, int scaledW, int scaledH)
{
	bool ret = true;
	SDL_Rect rect;
	rect.x = x;
	rect.y = y;
	rect.w = scaledW; rect.h = scaledH;



	if (SDL_RenderCopy(renderer, texture, section, &rect) != 0)
	{
		LOG("Cannot blit to screen. SDL_RenderCopy error: %s", SDL_GetError());
		ret = false;
	}

	return ret;
}

// Blit to screen
bool ModuleRender::BlitInQuad(SDL_Texture* texture, SDL_Rect* from, SDL_Rect* to, float speed, float scale)
{
	bool ret = true;
	SDL_Rect rect =  SDL_Rect();
	//rect.x = (int)(camera.x * speed) + to->x ;
	//rect.y = (int)(camera.y * speed) + to->y ;


	//rect.w = to->w * SCREEN_SIZE * scale;
	//rect.h = to->h *SCREEN_SIZE * scale;

	if (SDL_RenderCopy(renderer, texture, from, &rect) != 0)
	{
		LOG("Cannot blit to screen. SDL_RenderCopy error: %s", SDL_GetError());
		ret = false;
	}

	return ret;
}

// Blit to screen
bool ModuleRender::BlitRotate(SDL_Texture* texture, int x, int y, SDL_Rect* section, const double& angle, const SDL_Point* center, float speed, float scale)
{
	bool ret = true;
	SDL_Rect rect;
	rect.x = (int)(camera.x * speed) + x * SCREEN_SIZE;
	rect.y = (int)(camera.y * speed) + y * SCREEN_SIZE;

	if (section != NULL)
	{
		rect.w = section->w;
		rect.h = section->h;
	}
	else
	{
		SDL_QueryTexture(texture, NULL, NULL, &rect.w, &rect.h);
	}

	rect.w *= SCREEN_SIZE * scale;
	rect.h *= SCREEN_SIZE * scale;

	int resut = 0;

	if (SDL_RenderCopyEx(renderer, texture, section, &rect, angle, center, SDL_FLIP_NONE) != 0)
	{
		LOG("Cannot blit to screen. SDL_RenderCopy error: %s", SDL_GetError());
		ret = false;
	}

	return ret;
}


bool ModuleRender::DrawQuad(const SDL_Rect& rect, Uint8 r, Uint8 g, Uint8 b, Uint8 a, bool use_camera)
{
	bool ret = true;

	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
	SDL_SetRenderDrawColor(renderer, r, g, b, a);

	SDL_Rect rec(rect);
	if (use_camera)
	{
		rec.x = (int)(camera.x + rect.x * SCREEN_SIZE);
		rec.y = (int)(camera.y + rect.y * SCREEN_SIZE);
		rec.w *= SCREEN_SIZE;
		rec.h *= SCREEN_SIZE;
	}

	if (SDL_RenderFillRect(renderer, &rec) != 0)
	{
		LOG("Cannot draw quad to screen. SDL_RenderFillRect error: %s", SDL_GetError());
		ret = false;
	}

	return ret;
}

// Draw a point scalated to screen
bool ModuleRender::DrawPointScalable(int x, int y, Uint8 r, Uint8 g, Uint8 b, Uint8 a)
{

	bool ret = true;

	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
	SDL_SetRenderDrawColor(renderer, r, g, b, a);
	


	x = x*SCREEN_SIZE;
	y = y*SCREEN_SIZE;

	vector<SDL_Point> points;
	for(int i = 0; i < SCREEN_SIZE; ++i)
	{
		for (int j = 0; j < SCREEN_SIZE; ++j)
		{
			SDL_Point point = { x + i, y + j };
			points.push_back(point);
		}
	}

	if (SDL_RenderDrawPoints(renderer, &(points[0]), SCREEN_SIZE*SCREEN_SIZE*points.size()));
	{
		LOG("Cannot draw point to screen. SDL_RenderDrawPoints error: %s", SDL_GetError());
		ret = false;
	}

	return ret;
}

bool ModuleRender::DrawPointsScalables(vector<Point<int>>& pointsToPrint, Uint8 r, Uint8 g, Uint8 b, Uint8 a)
{

	bool ret = true;

	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
	SDL_SetRenderDrawColor(renderer, r, g, b, a);

	vector<SDL_Point> points;


	for (int i = 0; i < pointsToPrint.size(); ++i)
	{
		int x = pointsToPrint[i].x;
		x = x * SCREEN_SIZE;
		int y = pointsToPrint[i].y;
		y = y * SCREEN_SIZE;

		for (int i = 0; i < SCREEN_SIZE; ++i)
		{
			for (int j = 0; j < SCREEN_SIZE; ++j)
			{
				SDL_Point point = { x + i, y + j };
				points.push_back(point);
			}
		}
	}	

	if (SDL_RenderDrawPoints(renderer, &(points[0]), points.size()));
	{
		LOG("Cannot draw point to screen. SDL_RenderDrawPoints error: %s", SDL_GetError());
		ret = false;
	}

	return ret;
}
