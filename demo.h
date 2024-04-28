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

// Forward declaration of lTexture_s
typedef struct lTexture lTexture_s;

typedef struct CL_Instance
{
    SDL_Window* gWindow;
    SDL_Renderer* gRenderer;
    lTexture_s* gFooTexture;
    lTexture_s* gBackgroundTexture;  // Pointer to lTexture_s
    lTexture_s* gSpriteSheetTexture;
    SDL_Rect gSpriteClips[ 4 ];
} CL_Instance;

// Definition of lTexture_s
struct lTexture
{
    bool (*loadFromFile)(lTexture_s*, CL_Instance*, char *);
    // void (*render)(lTexture_s*, CL_Instance* , int, int ); // this one is for color
    void (*render)(lTexture_s*, CL_Instance* , int, int, SDL_Rect* );
    int (*getWidth)(lTexture_s*);
    int (*getHeight)(lTexture_s*);
    SDL_Texture* mTexture;
    int mWidth;
    int mHeight;
};



int init_instance(SDL_Instance *);
void draw_stuff(SDL_Instance* );
int poll_events();
bool loadMedia(SDL_Instance *);
bool loadMedia_Texture(SDL_Instance *);
bool loadMedia_Color(CL_Instance *);
bool loadMedia_Sprite(CL_Instance *);
bool loadMedia_Geometry();
void close(SDL_Instance *);
void close_color(CL_Instance *);
void close_sprite(CL_Instance *);
SDL_Surface* loadSurface(char* , SDL_Instance* );
SDL_Texture* loadTexture(char *path, SDL_Instance* );

#endif
