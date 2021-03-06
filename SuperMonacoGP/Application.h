#ifndef __APPLICATION_CPP__
#define __APPLICATION_CPP__

#include<list>
#include "Globals.h"
#include "Module.h"
#include "ModuleSceneSega.h"
#include "ModuleSceneFinal.h"

class ModuleRender;
class ModuleWindow;
class ModuleTextures;
class ModuleInput;
class ModuleAudio;
class ModuleFadeToBlack;
class ModuleCollision;
class ModuleParticles;

// Game modules ---
class ModulePlayer;
class ModuleSceneStart;
class ModuleSceneSelect;
class ModuleSceneRace;
class Application
{
public:

	Application();
	~Application();

	bool Init();
	update_status Update();
	bool CleanUp();

public:
	ModuleRender* renderer;
	ModuleWindow* window;
	ModuleTextures* textures;
	ModuleInput* input;
	ModuleAudio* audio;
	ModuleFadeToBlack* fade;
	ModuleCollision* collision;
	ModuleParticles* particles;

	// Game modules ---
	ModuleSceneStart* scene_start;
	ModuleSceneSelect* scene_select;
	ModuleSceneRace* scene_race;
	ModuleSceneSega* scene_sega;
	ModuleSceneFinal* scene_final;

private:

	std::list<Module*> modules;
};

extern Application* App;

#endif // __APPLICATION_CPP__