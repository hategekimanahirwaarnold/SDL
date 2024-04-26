#ifndef _DEMO_H_
#define _DEMO_H_

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <stdbool.h>

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480

enum KeyPressSurfaces
{
    KEY_PRESS_SURFACE_DEFAULT,
    KEY_PRESS_SURFACE_UP,
    KEY_PRESS_SURFACE_DOWN,
    KEY_PRESS_SURFACE_LEFT,
    KEY_PRESS_SURFACE_RIGHT,
    KEY_PRESS_SURFACE_TOTAL
};

typedef struct SDL_Instance
{
    SDL_Window* window;
    SDL_Texture* gTexture;
    SDL_Renderer* gRenderer;
    SDL_Surface* gScreenSurface;
    SDL_Surface* gHelloWorld;
    SDL_Surface* gKeyPressSurfaces[ KEY_PRESS_SURFACE_TOTAL ];
    SDL_Surface* gCurrentSurface;
    SDL_Surface* gStretchedSurface;
} SDL_Instance;

typedef struct lTexture
{
    bool (*loadFromFile)(char *);
    void (*render)(int , int );
    int (*getWidth)();
    int (*getHeight)();
    SDL_Texture* mTexture;
    int mWidth;
    int mHeight;
} lTexture;

typedef struct CL_Instance
{
    SDL_Window* gWindow;
    SDL_Renderer* gRenderer;
    lTexture gFooTexture;
    lTexture gBackgroundTexture;
} CL_Instance;

int init_instance(SDL_Instance *);
void draw_stuff(SDL_Instance* );
int poll_events();
bool loadMedia(SDL_Instance *);
bool loadMedia_Texture(SDL_Instance *);
bool loadMedia_Geometry();
void close(SDL_Instance *);
SDL_Surface* loadSurface(char* , SDL_Instance* );
SDL_Texture* loadTexture(char *path, SDL_Instance* );

#endif
