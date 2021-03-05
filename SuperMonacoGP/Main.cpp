#include <cstdlib>
#include "Application.h"
#include "Globals.h"
//#include "vld.h"

#include "SDL.h"

enum main_states
{
	MAIN_CREATION,
	MAIN_START,
	MAIN_UPDATE,
	MAIN_FINISH,
	MAIN_EXIT
};

Application* App = nullptr;

int main(int argc, char ** argv)
{

	int main_return = EXIT_FAILURE;
	main_states state = MAIN_CREATION;
	float maxMilsecPerFrame = 1.0f / FPS * 1000;
	Uint32 startTicks;
	Uint32 finishTicks;
	Uint32  elapsedMS;
	while (state != MAIN_EXIT)
	{
		startTicks = SDL_GetTicks();
		switch (state)
		{
		case MAIN_CREATION:

			LOG("Application Creation --------------");
			App = new Application();
			state = MAIN_START;
			break;

		case MAIN_START:

			LOG("Application Init --------------");
			if (App->Init() == false)
			{
				LOG("Application Init exits with error -----");
				state = MAIN_EXIT;
			}
			else
			{
				state = MAIN_UPDATE;
				LOG("Application Update --------------");
			}

			break;

		case MAIN_UPDATE:
		{
			Uint32 startTicks = SDL_GetTicks();
			int update_return = App->Update();

			if (update_return == UPDATE_ERROR)
			{
				//LOG("Application Update exits with error -----");
				state = MAIN_EXIT;
			}

			if (update_return == UPDATE_STOP)
				state = MAIN_FINISH;
		}
		break;

		case MAIN_FINISH:

			LOG("Application CleanUp --------------");
			if (App->CleanUp() == false)
			{
				LOG("Application CleanUp exits with error -----");
			}
			else
				main_return = EXIT_SUCCESS;

			state = MAIN_EXIT;

			break;
		}
		finishTicks = SDL_GetTicks();
		elapsedMS = (finishTicks - startTicks);
		if (maxMilsecPerFrame > elapsedMS)
			SDL_Delay(floor(maxMilsecPerFrame - elapsedMS));
	}

	RELEASE(App);
	LOG("Bye :)\n");
	return main_return;
}