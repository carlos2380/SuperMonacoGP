#include "Globals.h"
#include "Application.h"
#include "ModuleTextures.h"
#include "ModuleAudio.h"
#include "ModuleInput.h"
#include "ModuleRender.h"
#include "ModuleFadeToBlack.h"
#include "ModuleSceneRace.h"
#include <SDL.h>
#include "CtrlMap.h"
#include <iostream>
#include "SDL2_gfxPrimitives.h"
#include <string>
#include "json.hpp"
#include <iomanip>
#include "Line.h"
#include <fstream>
using namespace std;
using json = nlohmann::json;

CtrlMap::CtrlMap(bool active) : Module(active)
{
}


CtrlMap::~CtrlMap()
{}

// Load assets
bool CtrlMap::Start()
{
	turnAcceleration = 0;
	skybox = { 0, 0, 320, 200 };
	skyboxMirror = { 6000, 0, 320, 100 };
	sky = { 0, 300, 320, 100 };

	loadRoad();

	sizeMap = mapLines.size();
	playerX = 0.f;
	playerXMirror = 0.f;
	worldPosition = -50000;
	worldPositionMirror = +50000;
	lastMapPosition = mapPosition;
	skySprites = App->textures->Load("Sprites/SkyBox.bmp", 255, 0, 255);
	raceSprites = App->textures->Load("Sprites/RaceSprites.bmp", 255, 0, 255);
	tunnelSprites = App->textures->Load("Sprites/Tunnel.bmp", 255, 0, 255);
	LOG("Loading space intro");

	/*if (fx == 0)
	fx = App->audio->LoadFx("rtype/starting.wav");*/

	return true;
}


// UnLoad assets
bool CtrlMap::CleanUp()
{
	LOG("Unloading space scene");
	App->textures->Unload(raceSprites);
	App->textures->Unload(tunnelSprites);
	App->textures->Unload(skySprites);
	skySprites = nullptr;
	tunnelSprites = nullptr;
	raceSprites = nullptr;
	for (int i = 0; i < spriteVector.size(); ++i)
	{
		delete[] spriteVector[i];
	}
	spriteVector.clear();
	for (int i = 0; i < tunnelVector.size(); ++i)
	{
		delete[] tunnelVector[i];
	}
	tunnelVector.clear();
	return true;
}


update_status CtrlMap::Update()
{
	++testInt;
	speed = App->scene_race->ctrlCar->speed*1.6;
	speedMirror = -speed;
	//UPDATE TURN
	if (App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_DOWN || App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT
		|| App->input->GetKey(SDL_SCANCODE_D) == KEY_DOWN || App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT)
	{
		if (turnAcceleration < 0) turnAcceleration = 0;
		if (turnAcceleration < 0.0004) turnAcceleration += 0.000003;
	}
	else if (App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_DOWN || App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT
		|| App->input->GetKey(SDL_SCANCODE_A) == KEY_DOWN || App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT)
	{
		if (turnAcceleration > 0) turnAcceleration = 0;
		if (turnAcceleration > -0.0004) turnAcceleration -= 0.000003;

	}
	else
	{
		if (turnAcceleration < 0) turnAcceleration += 0.0001;
		else if (turnAcceleration > 0) turnAcceleration -= 0.0001;
	}
	playerX += turnAcceleration*speed;
	playerXMirror += turnAcceleration*speed;

	worldPosition += speed;
	while (worldPosition >= sizeMap*segL) worldPosition -= sizeMap*segL;
	while (worldPosition < 0) worldPosition += sizeMap*segL;

	worldPositionMirror += speedMirror;
	while (worldPositionMirror >= sizeMap*segL) worldPositionMirror -= sizeMap*segL;
	while (worldPositionMirror < 0) worldPositionMirror += sizeMap*segL;

	mapPosition = worldPosition / segL;
	int camH = mapLines[mapPosition].worldY + highWordl;

	mapPositionMirror = worldPositionMirror / segL;
	int camHMirror = mapMirror[mapPositionMirror].worldY + highWordlMirror;

	int maxy = height;
	int maxyMirror = height;
	float x = 0, dx = 0;

	if (mapLines[mapPosition%sizeMap].curve != 0) {
		playerX += -mapLines[mapPosition%sizeMap].curve*0.000005*speed;
		playerXMirror += -mapLines[mapPosition%sizeMap].curve*0.000005*speed;
	}
	App->renderer->Blit(skySprites, 0, -20, &skyboxMirror);
	if(mapPositionMirror == 0)
	{
		int x = 3;
	}
	for (int n = mapPositionMirror; n<mapPositionMirror + 40; n++)
	{
		Line &l = mapMirror[n%sizeMap];
		l.projectMirror(playerXMirror*roadW - x, camHMirror, mapPositionMirror*segL - (n >= sizeMap ? sizeMap*segL : 0));
		x += dx;
		dx += l.curve;

		l.clip = maxyMirror;
		if (l.screenY >= maxyMirror) continue;
		maxyMirror = l.screenY;


		if ((n - 1) < 0)continue;
		Line p = mapMirror[(n - 1) % sizeMap]; //previous line
		if (n  > mapPositionMirror + 3) {
			drawPoligonMirror(0, p.screenY, width, 0, l.screenY, width, l.grass.r, l.grass.g, l.grass.b);
			drawPoligonMirror(p.screenX, p.screenY, p.screenW*1.3, l.screenX, l.screenY, l.screenW*1.3, l.border.r, l.border.g, l.border.b);
			drawPoligonMirror(p.screenX, p.screenY, p.screenW, l.screenX, l.screenY, l.screenW, l.road.r, l.road.g, l.road.b);
			drawPoligonMirror(p.screenX, p.screenY, p.screenW*0.9, l.screenX, l.screenY, l.screenW*0.9, 255, 255, 255);
			drawPoligonMirror(p.screenX, p.screenY, p.screenW*0.8, l.screenX, l.screenY, l.screenW*0.8, l.road.r, l.road.g, l.road.b);
			if (n % 6 < 2) drawPoligonMirror(p.screenX, p.screenY, p.screenW*0.05, l.screenX, l.screenY, l.screenW*0.05, 255, 255, 255);
			if (n%sizeMap == sizeMap - 1 || n%sizeMap == sizeMap - 3) {
				drawPoligonMirror(p.screenX, p.screenY, p.screenW*0.8, l.screenX, l.screenY, l.screenW*0.8, 255, 255, 255);
				drawPoligonMirror(p.screenX, p.screenY, p.screenW*0.6, l.screenX, l.screenY, l.screenW*0.6, 0, 0, 0);
				drawPoligonMirror(p.screenX, p.screenY, p.screenW*0.4, l.screenX, l.screenY, l.screenW*0.4, 255, 255, 255);
				drawPoligonMirror(p.screenX, p.screenY, p.screenW*0.2, l.screenX, l.screenY, l.screenW*0.2, 0, 0, 0);
				drawPoligonMirror(p.screenX, p.screenY, p.screenW*0.1, l.screenX, l.screenY, l.screenW*0.1, 255, 255, 255);
			}
			if (n%sizeMap == sizeMap - 2) {
				drawPoligonMirror(p.screenX, p.screenY, p.screenW*0.8, l.screenX, l.screenY, l.screenW*0.8, 0, 0, 0);
				drawPoligonMirror(p.screenX, p.screenY, p.screenW*0.6, l.screenX, l.screenY, l.screenW*0.6, 255, 255, 255);
				drawPoligonMirror(p.screenX, p.screenY, p.screenW*0.4, l.screenX, l.screenY, l.screenW*0.4, 0, 0, 0);
				drawPoligonMirror(p.screenX, p.screenY, p.screenW*0.2, l.screenX, l.screenY, l.screenW*0.2, 255, 255, 255);
				drawPoligonMirror(p.screenX, p.screenY, p.screenW*0.1, l.screenX, l.screenY, l.screenW*0.1, 0, 0, 0);
				drawPoligonMirror(p.screenX, p.screenY, p.screenW*0.05, l.screenX, l.screenY, l.screenW*0.05, 255, 255, 255);
			}
		}
	}
	for (int n = mapPositionMirror + 40; n > mapPositionMirror; n--)
	{

		if (mapMirror[n%sizeMap].sprite != nullptr)
		{
			if (mapMirror[n%sizeMap].isTunnel == true)
			{
				if (n > mapPositionMirror + 36)
				{
					if (mapMirror[n%sizeMap].sprite == tunnelVector[0])
					{
						mapMirror[n%sizeMap].sprite = tunnelVector[1];
						mapMirror[n%sizeMap].drawSpriteMirror(tunnelSprites);
						mapMirror[n%sizeMap].sprite = tunnelVector[0];
					}
					else if(mapMirror[n % sizeMap].sprite == tunnelVector[2])
					{
						mapMirror[n%sizeMap].sprite = tunnelVector[3];
						mapMirror[n%sizeMap].drawSpriteMirror(tunnelSprites);
						mapMirror[n%sizeMap].sprite = tunnelVector[2];
					}
					else 
					{
						mapMirror[n % sizeMap].drawSpriteMirror(tunnelSprites);
					}
				}
				else
				{
					mapMirror[n%sizeMap].drawSpriteMirror(tunnelSprites);
				}
			}
			else
			{
				mapMirror[n%sizeMap].drawSpriteMirror(raceSprites);
			}
		}
	}
	App->renderer->Blit(skySprites, 0, 65, &sky);
	App->renderer->Blit(skySprites, 0, sckyBoxY, &skybox);
	//UPDATE NEXT SKYBOX Y
	if (mapLines[mapPosition%sizeMap].worldY < mapLines[(mapPosition + 1) % sizeMap].worldY) {
		sckyBoxY -= 0.1;
	}
	else if (mapLines[mapPosition%sizeMap].worldY > mapLines[(mapPosition + 1) % sizeMap].worldY) {
		sckyBoxY += 0.1;
	}
	else if (sckyBoxY > 112)
	{
		sckyBoxY -= 0.5;
	}
	else if (sckyBoxY < 108)
	{
		sckyBoxY += 0.5;
	}
	else
	{
		sckyBoxY = 110;
	}
	x = 0;
	dx = 0;
	//UPDATE ROAD
	for (int n = mapPosition; n<mapPosition + 65; n++)
	{
		
		mapLines[n%sizeMap].project(playerX*roadW - x, camH, mapPosition*segL - (n >= sizeMap ? sizeMap*segL : 0));
		Line &l = mapLines[n%sizeMap];
		x += dx;
		dx += l.curve;

		l.clip = maxy;
		if (l.screenY >= maxy) continue;
		maxy = l.screenY;



		Line p = mapLines[(n - 1) % sizeMap]; //previous line

		drawPoligon(0, p.screenY, width, 0, l.screenY, width, l.grass.r, l.grass.g, l.grass.b);
		drawPoligon(p.screenX, p.screenY, p.screenW*1.3, l.screenX, l.screenY, l.screenW*1.3, l.border.r, l.border.g, l.border.b);
		drawPoligon(p.screenX, p.screenY, p.screenW, l.screenX, l.screenY, l.screenW, l.road.r, l.road.g, l.road.b);
		drawPoligon(p.screenX, p.screenY, p.screenW*0.9, l.screenX, l.screenY, l.screenW*0.9, 255, 255, 255);
		drawPoligon(p.screenX, p.screenY, p.screenW*0.8, l.screenX, l.screenY, l.screenW*0.8, l.road.r, l.road.g, l.road.b);
		if (n % 6 < 2) drawPoligon(p.screenX, p.screenY, p.screenW*0.05, l.screenX, l.screenY, l.screenW*0.05, 255, 255, 255);
		if (n%sizeMap == 0 || n%sizeMap == 2) {
			drawPoligon(p.screenX, p.screenY, p.screenW*0.8, l.screenX, l.screenY, l.screenW*0.8, 255, 255, 255);
			drawPoligon(p.screenX, p.screenY, p.screenW*0.6, l.screenX, l.screenY, l.screenW*0.6, 0, 0, 0);
			drawPoligon(p.screenX, p.screenY, p.screenW*0.4, l.screenX, l.screenY, l.screenW*0.4, 255, 255, 255);
			drawPoligon(p.screenX, p.screenY, p.screenW*0.2, l.screenX, l.screenY, l.screenW*0.2, 0, 0, 0);
			drawPoligon(p.screenX, p.screenY, p.screenW*0.1, l.screenX, l.screenY, l.screenW*0.1, 255, 255, 255);
		}
		if (n%sizeMap == 1) {
			drawPoligon(p.screenX, p.screenY, p.screenW*0.8, l.screenX, l.screenY, l.screenW*0.8, 0, 0, 0);
			drawPoligon(p.screenX, p.screenY, p.screenW*0.6, l.screenX, l.screenY, l.screenW*0.6, 255, 255, 255);
			drawPoligon(p.screenX, p.screenY, p.screenW*0.4, l.screenX, l.screenY, l.screenW*0.4, 0, 0, 0);
			drawPoligon(p.screenX, p.screenY, p.screenW*0.2, l.screenX, l.screenY, l.screenW*0.2, 255, 255, 255);
			drawPoligon(p.screenX, p.screenY, p.screenW*0.1, l.screenX, l.screenY, l.screenW*0.1, 0, 0, 0);
			drawPoligon(p.screenX, p.screenY, p.screenW*0.05, l.screenX, l.screenY, l.screenW*0.05, 255, 255, 255);
		}
	}
	for (int n = mapPosition + 64; n>mapPosition; n--)
	{
		if(mapLines[n%sizeMap].sprite != nullptr)
		{
			if (mapLines[n%sizeMap].isTunnel == true)
			{
				if (n > mapPosition + 60)
				{
					if(mapLines[n%sizeMap].sprite == tunnelVector[0])
					{
						mapLines[n%sizeMap].sprite = tunnelVector[1];
						mapLines[n%sizeMap].drawSprite(tunnelSprites);
						mapLines[n%sizeMap].sprite = tunnelVector[0];
					}else if (mapLines[n % sizeMap].sprite == tunnelVector[2])
					{
						mapLines[n%sizeMap].sprite = tunnelVector[3];
						mapLines[n%sizeMap].drawSprite(tunnelSprites);
						mapLines[n%sizeMap].sprite = tunnelVector[2];
					}
					else 
					{
						mapLines[n % sizeMap].drawSprite(tunnelSprites);
					}
				}
				else
				{
					mapLines[n%sizeMap].drawSprite(tunnelSprites);
				}
			}
			else
			{
				mapLines[n%sizeMap].drawSprite(raceSprites);
			}
		}
	}

	


	//UPDATE NEXT SKYBOX X
	if (speed > 0) {
		skyBoxX += mapLines[mapPosition].curve * 0.1f;
		sckyBoxXMirror -= mapLines[mapPosition].curve * 0.1f;
		if (skyBoxX > 5880) skyBoxX = 3200;
		if (sckyBoxXMirror < 0) sckyBoxXMirror = 3200;
		
	}
	if (speed < 0) {
		skyBoxX -= mapLines[mapPosition].curve * 0.1f;
		sckyBoxXMirror += mapLines[mapPosition].curve * 0.1f;
		if (skyBoxX < 0) skyBoxX = 3200;
		if (sckyBoxXMirror > 5880) sckyBoxXMirror = 3200;
	}
	skybox.x = skyBoxX;
	skyboxMirror.x = sckyBoxXMirror;



	if (mapPosition == 0 && lastMapPosition != mapPosition) App->scene_race->nextLap();
	lastMapPosition = mapPosition;
	//App->renderer->Blit(raceSprites, 64, 209, rectFrame, NULL, 1.3);
	if (playerX > 1.6 || playerX < -1.6) {
		if (App->scene_race->ctrlCar->speed > 50)App->scene_race->ctrlCar->speed = 50;
	}
	return UPDATE_CONTINUE;
}

void CtrlMap::drawPoligon(int x1, int y1, int w1, int x2, int y2, int w2, int r, int g, int b)
{
	int desp = (SCREEN_HEIGHT / 2)*SCREEN_SIZE;
	short x[4] = { x1 - w1, x2 - w2, x2 + w2, x1 + w1 };
	short y[4] = { y1 + desp, y2 + desp, y2 + desp, y1 + desp };

	filledPolygonRGBA(App->renderer->renderer, x, y, 4, r, g, b, 255);
}

void CtrlMap::drawPoligonMirror(int x1, int y1, int w1, int x2, int y2, int w2, int r, int g, int b)
{
	int desp = 31 * SCREEN_SIZE;
	short x[4] = { x1 - w1, x2 - w2, x2 + w2, x1 + w1 };
	short y[4] = { y1 + desp, y2 + desp, y2 + desp, y1 + desp };

	filledPolygonRGBA(App->renderer->renderer, x, y, 4, r, g, b, 255);
}

void CtrlMap::loadRoad()
{
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

	ifstream fileSprite("Files/SpritesRoad.json");
	json jsnSprite;
	fileSprite >> jsnSprite;
	fileSprite.close();
	list<json> spritesJson = jsnSprite;
	spriteVector = vector<SDL_Rect*>(spritesJson.size());
	j = 0;
	for (list<json>::iterator it = spritesJson.begin(); it != spritesJson.end(); ++it)
	{
		spriteVector[j] = new SDL_Rect();
		spriteVector[j]->x = (*it).at("x");
		spriteVector[j]->y = (*it).at("y");
		spriteVector[j]->w = (*it).at("w");
		spriteVector[j]->h = (*it).at("h");
		++j;
	}

	ifstream fileTunnel("Files/Tunnel.json");
	json jsnTunnel	;
	fileTunnel >> jsnTunnel;
	fileSprite.close();
	list<json> tunnelJson = jsnTunnel;
	tunnelVector = vector<SDL_Rect*>(tunnelJson.size());
	j = 0;
	for (list<json>::iterator it = tunnelJson.begin(); it != tunnelJson.end(); ++it)
	{
		tunnelVector[j] = new SDL_Rect();
		tunnelVector[j]->x = (*it).at("x");
		tunnelVector[j]->y = (*it).at("y");
		tunnelVector[j]->w = (*it).at("w");
		tunnelVector[j]->h = (*it).at("h");
		++j;
	}

	ifstream ifile("Files/Map.json");
	json jsn;
	ifile >> jsn;
	ifile.close();

	list<json> linesJson = jsn;
	mapLines = vector<Line>(linesJson.size());
	mapMirror = vector<Line>(linesJson.size());
	
	int i = 0;
	for (list<json>::iterator it = linesJson.begin(); it != linesJson.end(); ++it) {
		Line line;

		line.curve = (*it).at("curve");
		line.worldY = (*it).at("worldY");
		line.worldZ = (*it).at("worldZ");
		line.isTunnel = (*it).at("isTunnel");
		line.road.r = (*it).at("road").at("r");
		line.road.g = (*it).at("road").at("g");
		line.road.b = (*it).at("road").at("b");
		line.grass.r = (*it).at("grass").at("r");
		line.grass.g = (*it).at("grass").at("g");
		line.grass.b = (*it).at("grass").at("b");
		line.border.r = (*it).at("border").at("r");
		line.border.g = (*it).at("border").at("g");
		line.border.b = (*it).at("border").at("b");
		line.spriteX = (*it).at("spriteX");
		if((*it).at("sprite") == -1)
		{
			line.sprite = nullptr;
		}else
		{
			if(line.isTunnel == false)
			{
				line.sprite = spriteVector[(*it).at("sprite")];
			}else
			{
				line.sprite = tunnelVector[(*it).at("sprite")];
			}
			
		}

		line.width = width;
		line.height = height;
		line.heightMirror = heightMirror;
		line.roadW = roadW;
		line.camD = camD;


		mapLines[i] = line;
		line.worldY = -line.worldY;
		line.worldZ = linesJson.size()*segL - line.worldZ;
		mapMirror[mapLines.size() - i -1] = line;
		++i;
	}

}
