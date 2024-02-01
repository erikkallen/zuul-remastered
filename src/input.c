#include <SDL2/SDL.h>

#include "structs.h"

void doInput(App * app)
{
	
	while (SDL_PollEvent(&app->event))
	{
		switch (app->event.type)
		{
			case SDL_QUIT:
				exit(0);
				break;

			default:
				break;
		}
	}
}