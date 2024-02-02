#include <SDL2/SDL.h>

#include "structs.h"
#include "input.h"

static void input_on_key_down(App * app)
{
    SDL_KeyboardEvent *event = &app->event.key;
	if (event->repeat == 0)
	{
		if (event->repeat == 0 && event->keysym.scancode < MAX_KEYBOARD_KEYS)
        {
            app->keyboard[event->keysym.scancode] = 1;
        }
	}
}

static void input_on_key_up(App * app)
{
    SDL_KeyboardEvent *event = &app->event.key;
	if (event->repeat == 0)
	{
		if (event->repeat == 0 && event->keysym.scancode < MAX_KEYBOARD_KEYS)
        {
            app->keyboard[event->keysym.scancode] = 0;
        }
	}
}

void input_handle(App * app)
{
	
	while (SDL_PollEvent(&app->event))
	{
		switch (app->event.type)
		{
			case SDL_QUIT:
				exit(0);
				break;

			case SDL_KEYDOWN:
				input_on_key_down(app);
				break;

			case SDL_KEYUP:
				input_on_key_up(app);
				break;

			default:
				break;
		}
	}
}