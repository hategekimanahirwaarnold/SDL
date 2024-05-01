#ifndef _DEMO_H_
#define _DEMO_H_

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
// #include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include <stdbool.h>
#include <math.h>

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480
#define WALKING_ANIMATION_FRAMES 4
#define BUTTON_WIDTH 300
#define BUTTON_HEIGHT 200
#define TOTAL_BUTTONS 4
#define JOYSTICK_DEAD_ZONE 8000

enum LButtonSprite
{
    BUTTON_SPRITE_MOUSE_OUT,
    BUTTON_SPRITE_MOUSE_OVER_MOTION,
    BUTTON_SPRITE_MOUSE_DOWN,
    BUTTON_SPRITE_MOUSE_UP,
    BUTTON_SPRITE_TOTAL,
};

enum KeyPressSurfaces
{
    KEY_PRESS_SURFACE_DEFAULT,
    KEY_PRESS_SURFACE_UP,
    KEY_PRESS_SURFACE_DOWN,
    KEY_PRESS_SURFACE_LEFT,
    KEY_PRESS_SURFACE_RIGHT,
    KEY_PRESS_SURFACE_TOTAL
};


// Forward declaration of lTexture_s
typedef struct lTexture lTexture_s;
typedef struct LButton lButton;
typedef struct CL_Instance CL_Instance;
struct LButton
{
    void (*setPosition)(lButton*, int x, int y);
    void (*handleEvent)(SDL_Event* , struct LButton* );
    void (*render)(struct LButton* , CL_Instance *);
    SDL_Point mPosition;
    int mCurrentSprite;
};

struct CL_Instance
{
    SDL_Window* gWindow;
    SDL_Renderer* gRenderer;
    lTexture_s* gTextTexture;
    lTexture_s* gArrowTexture;
    lTexture_s* gFooTexture;
    lTexture_s* gBackgroundTexture;  // Pointer to lTexture_s
    lTexture_s* gSpriteSheetTexture;
    lTexture_s* gButtonSpriteSheetTexture;
    lTexture_s* gModulatedTexture;
    lTexture_s* gPressTexture;
    lTexture_s* gUpTexture;
    lTexture_s* gDownTexture;
    lTexture_s* gLeftTexture;
    lTexture_s* gRightTexture;
    lTexture_s* gSplashTexture;
    lTexture_s* gPromptTexture;
    // TTF_Font* gFont;
    SDL_Rect gSpriteClips[ 4 ];
    lButton gButtons[ TOTAL_BUTTONS ];
    // SDL_Joystick* gGameController;
    SDL_GameController* gGameController;
    SDL_Joystick* gJoystick;
    SDL_Haptic* gJoyHaptic;
    Mix_Music* gMusic;
    Mix_Chunk* gScratch;
    Mix_Chunk* gHigh;
    Mix_Chunk* gMedium;
    Mix_Chunk* gLow;
};

// Definition of lTexture_s
struct lTexture
{
    bool (*loadFromFile)(lTexture_s*, CL_Instance*, char *);
    // void (*render)(lTexture_s*, CL_Instance* , int, int ); // this one is for color
    // void (*render)(lTexture_s*, CL_Instance* , int, int, SDL_Rect* ); // this one is for sprite
    void (*render)(lTexture_s*, CL_Instance* , int, int, SDL_Rect* , double , SDL_Point* , SDL_RendererFlip );
    // void (*render)(lTexture_s*, CL_Instance* , int, int, SDL_RendererFlip ); // this one is for handling button events
    bool (*loadFromRenderedText)(lTexture_s* , CL_Instance*, char*, SDL_Color);
    int (*getWidth)(lTexture_s*);
    int (*getHeight)(lTexture_s*);
    void (*setColor)(SDL_Texture* ,Uint8, Uint8, Uint8);
    void (*setBlendMode)(SDL_Texture* , SDL_BlendMode);
    void (*setAlpha)(SDL_Texture*, Uint8);
    SDL_Texture* mTexture;
    int mWidth;
    int mHeight;
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

int init_instance(SDL_Instance *);
void draw_stuff(SDL_Instance* );
int poll_events();
// bool loadMedia(SDL_Instance *);
bool loadMedia(CL_Instance *);
bool loadMedia_Texture(SDL_Instance *);
bool loadMedia_Color(CL_Instance *);
bool loadMedia_Sprite(CL_Instance *);
bool loadMedia_Modulation(CL_Instance *);
bool loadMedia_Geometry();
void close(SDL_Instance *);
void close_color(CL_Instance *);
void close_sprite(CL_Instance *);
void close_instance(CL_Instance *);
SDL_Surface* loadSurface(char* , SDL_Instance* );
SDL_Texture* loadTexture(char *path, SDL_Instance* );

#endif
