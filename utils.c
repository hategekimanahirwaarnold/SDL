#include "demo.h"

int init_instance(SDL_Instance *instance)
{
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        fprintf(stderr, "Unable to initialize SDL: %s\n", SDL_GetError());
        return (1);
    }
    /* create a new window instance */
    instance->window = SDL_CreateWindow("SDL Example", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (instance->window == NULL)
    {
        fprintf(stderr, "SDL_CreateWindow Error: %s\n", SDL_GetError());
        SDL_Quit();
        return (1);
    }
    /* Create a new Renderer instance linked to the window*/
    instance->gRenderer = SDL_CreateRenderer(instance->window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (instance->gRenderer == NULL)
    {
        fprintf(stderr, "SDL_CreateRenderer Error: %s\n", SDL_GetError());
        return (1);
    }
    draw_stuff(instance);
    // Get window surface
    // instance->gScreenSurface = SDL_GetWindowSurface( instance->window );
    return (0);
}

SDL_Texture* loadTexture (char* path, SDL_Instance *instance)
{
    SDL_Texture* newTexture = NULL;
    /*Load image at specific path*/

    SDL_Surface* loadedSurface = IMG_Load(path);
    if (loadedSurface == NULL)
    {
        printf("Unable to load image %s! SDL_image Error: %s\n", path, IMG_GetError());
    } else
    {
        //create texture from surface pixels
        newTexture = SDL_CreateTextureFromSurface(instance->gRenderer, loadedSurface);
        if (newTexture == NULL)
        {
            printf("Unable to create texture from %s! SDL Error: %s\n", path, SDL_GetError() );
        }
        //Get rid of old loaded surface
        SDL_FreeSurface( loadedSurface );
    }

    return newTexture;
}

bool loadMedia_Texture(SDL_Instance* instance)
{
    bool success = true;

    instance->gTexture = loadTexture("img/viewport.png", instance);
    if (instance->gTexture == NULL)
    {
        printf("Failed to load texture image!\n");
        success = false;
    }
    return success;
}

void draw_stuff(SDL_Instance* instance)
{
    SDL_SetRenderDrawColor(instance->gRenderer, 0xFF, 0XFF, 0XFF, 0xFF);
    // SDL_RenderDrawLine(instance.renderer, 10, 10, 100, 100);
    /*Initialize PNG loading*/
    int imgFlags = IMG_INIT_PNG;
    if ( !(IMG_Init(imgFlags) & imgFlags ))
    {
        printf("SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError() );
    }
}

int poll_events()
{
    SDL_Event event;
    SDL_KeyboardEvent key;

    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
        case SDL_QUIT:
            return (1);
        case SDL_KEYDOWN:
            key = event.key;
            if (key.keysym.scancode == 0x29)
                return (1);
            break;
        }
    }
    return (0);
}

bool loadMedia(SDL_Instance *instance)
{
    bool success = true;

    instance->gStretchedSurface = loadSurface("img/stretch.bmp", instance);
    if (instance->gStretchedSurface == NULL)
    {
        printf("Failed to load stretching image!\n");
        success = false;
    }

    instance->gKeyPressSurfaces[ KEY_PRESS_SURFACE_DEFAULT ] = loadSurface("img/press.bmp", instance);
    if (instance->gKeyPressSurfaces[ KEY_PRESS_SURFACE_DEFAULT ] == NULL)
    {
        printf("Failed to load default image!\n");
        success = false;
    }
    //load up surface
    instance->gKeyPressSurfaces[ KEY_PRESS_SURFACE_UP ] = loadSurface("img/up.bmp", instance);
    if ( instance->gKeyPressSurfaces[ KEY_PRESS_SURFACE_UP ] == NULL )
    {
        printf("Failed to load up image!\n");
        success = false;
    }
    //load down surface
    instance->gKeyPressSurfaces[ KEY_PRESS_SURFACE_DOWN ] = loadSurface("img/down.bmp", instance);
    if ( instance->gKeyPressSurfaces[ KEY_PRESS_SURFACE_DOWN ] == NULL )
    {
        printf("Failed to load down image!\n");
        success = false;
    }
    //load left surface
    instance->gKeyPressSurfaces[ KEY_PRESS_SURFACE_LEFT ] = loadSurface("img/left.bmp", instance);
    if ( instance->gKeyPressSurfaces[ KEY_PRESS_SURFACE_LEFT ] == NULL )
    {
        printf("Failed to load left image!\n");
        success = false;
    }

    //load right surface
    instance->gKeyPressSurfaces[ KEY_PRESS_SURFACE_RIGHT ] = loadSurface("img/right.bmp", instance);
    if ( instance->gKeyPressSurfaces[ KEY_PRESS_SURFACE_RIGHT ] == NULL )
    {
        printf("Failed to load right image!\n");
        success = false;
    }
 
    return success;
}


void close(SDL_Instance *instance)
{
    // SDL_DestroyRenderer( instance->renderer );
    for (int i = 0; i < KEY_PRESS_SURFACE_TOTAL; ++i)
    {
        SDL_FreeSurface( instance->gKeyPressSurfaces[ i ]);
        instance->gKeyPressSurfaces[ i ] = NULL;
    }
    SDL_DestroyWindow(instance->window);
    SDL_FreeSurface(instance->gHelloWorld);
    // SDL_FreeSurface(instance->gStretchedSurface);
    // instance->gStretchedSurface = NULL;
    instance->gHelloWorld = NULL;
    instance->window = NULL;
    IMG_Quit();
    SDL_Quit();
}

SDL_Surface* loadSurface( char* path, SDL_Instance* instance)
{
    /*Load optimized image*/
    SDL_Surface* optimizedSurface = NULL;

    /*Load image at specified path*/
    SDL_Surface* loadedSurface = SDL_LoadBMP( path );
    if( loadedSurface == NULL)
    {
        printf("Unable to load image %s! SDL Error: %s\n", path, SDL_GetError() );
    } else
    {
        /*convert surface to screen format*/
        optimizedSurface = SDL_ConvertSurface( loadedSurface, instance->gScreenSurface->format, 0);
        if (optimizedSurface == NULL)
        {
            printf("Unable to optimize image %s! SDL Error: %s\n", path, SDL_GetError() );
        }
        /*Get rid of old loaded surface*/
        SDL_FreeSurface( loadedSurface );
    }

    return optimizedSurface;
}

bool loadMedia_Geometry()
{
    bool success = true;
    return success;
}

void free_texture(lTexture* texture)
{
    if (texture->mTexture != NULL)
    {
        SDL_DestroyTexture(texture->mTexture);
        texture->mTexture = NULL;
        texture->mWidth = 0;
        texture->mHeight = 0;
    }
}
