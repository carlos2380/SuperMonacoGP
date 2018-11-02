#include "Globals.h"
#include "Application.h"
#include "ModuleTextures.h"
#include "ModuleInput.h"
#include "ModuleRender.h"
#include "ModuleFadeToBlack.h"
#include <SDL.h>
#include "ModuleSceneSega.h"
#include "Point.h"
#include <ppl.h>
#include <windows.h>
#include <fstream>
using namespace std;

ModuleSceneSega::ModuleSceneSega(bool active) : Module(active)
{
	background = { 0, 0, 320, 240 };
	backgroundBlack = { 0, 0, 320, 240 };
}

ModuleSceneSega::~ModuleSceneSega()
{}

// Load assets
bool ModuleSceneSega::Start()
{
	LOG("Loading space intro");
	selectSprites = App->textures->Load("Sprites/SegaSprites.bmp", 255, 0, 255);
	blackSprite = App->textures->Load("Sprites/BlackSprite.bmp", 255, 0, 255);

	ifstream ifs("Files/SegaRed.txt");
	string content((std::istreambuf_iterator<char>(ifs)), (std::istreambuf_iterator<char>()));
	matrixBorder = vector<vector<bool*>>(320, vector<bool*>(240));
	for (int i = 0; i < content.size(); ++i)
	{
		if (content[i] == '1')
		{
			matrixBorder[i / matrixBorder[0].size()][i%matrixBorder[0].size()] = new bool(false);
		}
	}
	ifs.close();

	Point<int> firstPoint = { 114, 98 };
	pointsToCheck.push_back(firstPoint);

	ifstream ifsa("Files/SegaBlue.txt");
	string contentInside((std::istreambuf_iterator<char>(ifsa)), (std::istreambuf_iterator<char>()));
	ifsa.close();

	for (int i = 0; i < contentInside.size(); ++i)
	{
		if (contentInside[i] == '1')
		{
			int x = i / 240; int y = i % 240;
			Point<int> pointContent = { x, y-1};
			pointsToPrintContent.push_back(pointContent);
		}
	}

	contentColor.r = 0;
	contentColor.g = 0;
	contentColor.b = 0;
	contentColor.a = 255;
	

	App->renderer->camera.x = App->renderer->camera.y = 0;
	
	return true;
}

// UnLoad assets
bool ModuleSceneSega::CleanUp()
{
	LOG("Unloading space scene");
	App->textures->Unload(selectSprites);
	for(int i = 0; i < matrixBorder.size(); ++i)
	{
		for (int j = 0; j < matrixBorder[i].size(); ++j)
		{
			delete [] matrixBorder[i][j];
		}
		matrixBorder[i].clear();
	}
	matrixBorder.clear();
	
	vector<Point<int>>().swap(pointsToPrintBorder);
	vector<Point<int>>().swap(pointsToPrintContent);
	pointsToCheck.clear();
	App->textures->Unload(blackSprite);
	return true;
}

// Update: draw background
update_status ModuleSceneSega::Update()
{
	bool printContent = false;
	//Print sega lines then fill color then print background
	if(pointsToCheck.empty() == false)
	{
		//DFS for each frame the loop is for increase the speed
		
		int numToCheck = pointsToCheck.size();
		pointsToCheck.begin();
		for (int i = 0; i < numToCheck; ++i)
		{
			pointsToPrintBorder.push_back(*pointsToCheck.begin());
			checkNextsPoints();
			pointsToCheck.pop_front();
		}
	}
	else if(contentColor.r < 200)
	{
		printContent = true;
		timer = 0;
		contentColor.r += 4;
		contentColor.g += 4;
		contentColor.b += 4;
	}

	if(contentColor.r < 200)
	{
		App->renderer->Blit(blackSprite, 0, 0, &background);
		
		if(printContent == true)
		{
			App->renderer->DrawPointsScalables(pointsToPrintContent, contentColor.r, contentColor.g, contentColor.b, contentColor.a);
		}

		App->renderer->DrawPointsScalables(pointsToPrintBorder, 255, 255, 255, 255);
	}
	else
	{
		++timer;
		App->renderer->Blit(selectSprites, 0, 0, &background);
	}

	

	if (App->input->GetKey(SDL_SCANCODE_RETURN) == KEY_DOWN && App->fade->isFading() == false || timer == 100)
	{
		App->fade->FadeToBlack((Module*)App->scene_start, this);
	}
	
	return UPDATE_CONTINUE;
}

void ModuleSceneSega::checkNextsPoints() {
	for(int i = -1; i < 2; ++i)
	{
		for (int j = -1; j < 2; ++j)
		{
			if (matrixBorder[pointsToCheck.begin()->x + i][pointsToCheck.begin()->y + j] != nullptr)
			{
				bool visited = *(matrixBorder[pointsToCheck.begin()->x + i][pointsToCheck.begin()->y + j]);
				if (visited == false)
				{
					*(matrixBorder[pointsToCheck.begin()->x + i][pointsToCheck.begin()->y + j]) = true;
					Point<int> nextPoint = { pointsToCheck.begin()->x + i, pointsToCheck.begin()->y + j};
					pointsToCheck.push_back(nextPoint);
				}
			}
		}
	}
	
}