#include "demo.h"

int main(void)
{
    int quit = 1;
    SDL_Instance instance;
    if (init_instance(&instance))
        return (1);
    while (quit)
    {
        if (poll_events() == 1)
        {
            break;
        }
        SDL_SetRenderDrawColor(instance.renderer, 0, 0, 0, 255);
        SDL_RenderClear(instance.renderer);
        draw_stuff(instance);
        SDL_RenderPresent(instance.renderer);
    }
    close(&instance);

    return (0);
}
