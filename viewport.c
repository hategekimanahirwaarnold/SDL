#include "demo.h"

int main(void)
{
    int quit = 1;
    SDL_Instance instance;
    instance.window = NULL;
    instance.gHelloWorld = NULL;

    if (init_instance(&instance))
        return (1);

    draw_stuff(&instance);
    if (!loadMedia_Texture(&instance))
    {
        printf("Failed to load Media!\n");
        return (1);
    }
    // Hack to get window to stay up
    SDL_Event e;
    while (quit)
    {
        while (SDL_PollEvent(&e))
        {
            if (e.type == SDL_QUIT)
                quit = 0;
        }
        //Clear screen
        SDL_SetRenderDrawColor(instance.gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
        SDL_RenderClear(instance.gRenderer);

        // top left corner viewport
        SDL_Rect topLeftViewport;
        topLeftViewport.x = 0;
        topLeftViewport.y = 0;
        topLeftViewport.w = SCREEN_WIDTH / 2;
        topLeftViewport.h = SCREEN_HEIGHT / 2;
        SDL_RenderSetViewport (instance.gRenderer, &topLeftViewport);

        //render texture to screen
        SDL_RenderCopy(instance.gRenderer, instance.gTexture, NULL, NULL);
    
        // Top right viewport
        SDL_Rect topRightViewport;
        topRightViewport.x = SCREEN_WIDTH / 2;
        topRightViewport.y = 0;
        topRightViewport.w = SCREEN_WIDTH / 2;
        topRightViewport.h = SCREEN_HEIGHT / 2;
        SDL_RenderSetViewport(instance.gRenderer, &topRightViewport);

        //Render texture to screen
        SDL_RenderCopy(instance.gRenderer, instance.gTexture, NULL, NULL);
        // Bottom viewport
        SDL_Rect bottomViewport;
        bottomViewport.x = 0;
        bottomViewport.y = SCREEN_HEIGHT / 2;
        bottomViewport.w = SCREEN_WIDTH;
        bottomViewport.h = SCREEN_HEIGHT / 2;
        SDL_RenderSetViewport(instance.gRenderer, &bottomViewport);

        //Render texture to screen
        SDL_RenderCopy(instance.gRenderer, instance.gTexture, NULL, NULL);

        //Update screen
        SDL_RenderPresent(instance.gRenderer);

    }

    close(&instance);

    return (0);
}
