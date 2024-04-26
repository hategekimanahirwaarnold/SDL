#include "demo.h"

int main(void)
{
    int quit = 1;
    SDL_Instance instance;
    instance.window = NULL;
    instance.gHelloWorld = NULL;

    if (init_instance(&instance))
        return (1);

    // Hack to get window to stay up
    SDL_Event e;
    while (quit)
    {
        while (SDL_PollEvent(&e))
        {
            if (e.type == SDL_QUIT)
                quit = 0;
        }
        // clear screen
        SDL_SetRenderDrawColor(instance.gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
        SDL_RenderClear(instance.gRenderer);
        // render red filled quad
        SDL_Rect fillRect = {SCREEN_WIDTH / 4, SCREEN_HEIGHT / 4, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2};
        SDL_SetRenderDrawColor(instance.gRenderer, 0xFF, 0x00, 0x00, 0xFF);
        SDL_RenderFillRect(instance.gRenderer, &fillRect);
        // render green outlined quad
        SDL_Rect outlineRect = {SCREEN_WIDTH / 6, SCREEN_HEIGHT / 6, SCREEN_WIDTH * 2 / 3, SCREEN_HEIGHT * 2 / 3};
        SDL_SetRenderDrawColor(instance.gRenderer, 0x00, 0xFF, 0x00, 0xFF);
        SDL_RenderDrawRect(instance.gRenderer, &outlineRect);
        // Draw blue horizontal line
        SDL_SetRenderDrawColor(instance.gRenderer, 0x00, 0x00, 0xFF, 0xFF);
        SDL_RenderDrawLine(instance.gRenderer, 0, SCREEN_HEIGHT / 2, SCREEN_WIDTH, SCREEN_HEIGHT / 2);
        // Draw vertical line of yellow dots
        SDL_SetRenderDrawColor(instance.gRenderer, 0xFF, 0xFF, 0x00, 0xFF);
        for (int i = 0; i < SCREEN_HEIGHT; i += 4)
        {
            SDL_RenderDrawPoint(instance.gRenderer, SCREEN_WIDTH / 2, i);
        }
        // Update Screen
        SDL_RenderPresent(instance.gRenderer);
    }

    close(&instance);

    return (0);
}
