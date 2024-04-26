#include "demo.h"

int main(void)
{
    int quit = 1;
    SDL_Instance instance;
    instance.window = NULL;
    // instance.renderer = NULL;
    instance.gHelloWorld = NULL;

    if (init_instance(&instance))
        return (1);

    if (!loadMedia_Texture(&instance))
    {
        printf("Failed to load Media!\n");
    }
    else
    {
        // Hack to get window to stay up
        SDL_Event e;
		instance.gCurrentSurface = instance.gKeyPressSurfaces[ KEY_PRESS_SURFACE_DEFAULT ];
        while (quit)
        {
            while (SDL_PollEvent(&e))
            {
                if (e.type == SDL_QUIT)
                    quit = 0;
                else if ( e.type == SDL_KEYDOWN)
                {
                     switch(e.key.keysym.sym)       
                     {
                        case SDLK_UP:
                            instance.gCurrentSurface = instance.gKeyPressSurfaces[ KEY_PRESS_SURFACE_UP ];
                        break;
                        
                        case SDLK_DOWN:
                            instance.gCurrentSurface = instance.gKeyPressSurfaces[ KEY_PRESS_SURFACE_DOWN ];
                        break;
    
                        case SDLK_LEFT:
                            instance.gCurrentSurface = instance.gKeyPressSurfaces[ KEY_PRESS_SURFACE_LEFT ];
                        break;
                        
                        case SDLK_RIGHT:
                            instance.gCurrentSurface = instance.gKeyPressSurfaces[ KEY_PRESS_SURFACE_RIGHT ];
                        break;
                        
                        default:
                            instance.gCurrentSurface = instance.gKeyPressSurfaces[ KEY_PRESS_SURFACE_DEFAULT ];
                        break;
                     }
                }
            }
            /* Apply the image stretched
            SDL_Rect stretchRect;
            stretchRect.x = 0;
            stretchRect.y = 0;
            stretchRect.w = SCREEN_WIDTH;
            stretchRect.h = SCREEN_HEIGHT;
            SDL_BlitScaled(instance.gStretchedSurface, NULL, instance.gScreenSurface, &stretchRect);
            */
            //Apply the image
            // SDL_BlitSurface( instance.gCurrentSurface, NULL, instance.gScreenSurface, NULL );
            //printf("Image size: %d x %d\n", instance.gHelloWorld->w, instance.gHelloWorld->h);

            //Update the surface
            // SDL_UpdateWindowSurface( instance.window );
            // Cleanup
            SDL_RenderClear(instance.gRenderer);
            SDL_RenderCopy(instance.gRenderer, instance.gTexture, NULL, NULL);
            SDL_RenderPresent(instance.gRenderer);
            // SDL_DestroyTexture(texture);
        }
    }
    close(&instance);

    return (0);
}
