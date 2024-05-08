#ifndef _DEMO_H_
#define _DEMO_H_

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <string.h>
#include <stdlib.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include <stdbool.h>
#include <math.h>

#define LEVEL_WIDTH 1280
#define LEVEL_HEIGHT 960
#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480
#define WALKING_ANIMATION_FRAMES 4
#define BUTTON_WIDTH 30 0
#define BUTTON_HEIGHT 200
#define TOTAL_BUTTONS 4
#define JOYSTICK_DEAD_ZONE 8000
#define SCREEN_FPS 60
#define SCREEN_TICKS_PER_FRAME (1000 / SCREEN_FPS)
#define TOTAL_DATA 10
#define MAX_RECORDING_DEVICES 10
#define MAX_RECORDING_SECONDS 5
#define RECORDING_BUFFER_SECONDS (MAX_RECORDING_SECONDS + 1)
#define TOTAL_WINDOWS 3
#define TOTAL_PARTICLES 20
//Tile constants
#define TILE_WIDTH 80
#define TILE_HEIGHT 80
#define TOTAL_TILES 192
#define TOTAL_TILE_SPRITES 12

//Different tile sprites
#define TILE_RED 0
#define TILE_GREEN 1
#define TILE_BLUE 2
#define TILE_CENTER 3
#define TILE_TOP 4
#define TILE_TOPRIGHT 5
#define TILE_RIGHT 6
#define TILE_BOTTOMRIGHT 7
#define TILE_BOTTOM 8
#define TILE_BOTTOMLEFT 9
#define TILE_LEFT 10
#define TILE_TOPLEFT 11



//The various recording actions we can take
enum RecordingState
{
    SELECTING_DEVICE,
    STOPPED,
    RECORDING,
    RECORDED,
    PLAYBACK,
    ERROR
};


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
typedef struct LTimer lTimer;
typedef struct Dot Dot_s;
typedef struct Circle Circle_s;
typedef struct Lwindow lWindow;
typedef struct Particle_s Particle;
typedef struct Tile_s Tile;
typedef struct LBitmapFont_s LBitmapFont;
typedef struct DataStream_s DataStream;

struct DataStream_s
{
    //Loads initial data
    bool (*loadMedia)(DataStream*);

    //Gets current frame data
    void* (*getBuffer)(DataStream*);

    //Internal data
    SDL_Surface* mImages[ 4 ];
    int mCurrentImage;
    int mDelayFrames;
};

struct Circle
{
    int x, y, r;
};

struct Dot
{
    //The dimensions of the dot
    int DOT_WIDTH;
    int DOT_HEIGHT;

    //Maximum axis velocity of the dot
    int DOT_VEL;

    //Takes key presses and adjusts the dot's velocity
    void (*handleEvent)(struct Dot*, SDL_Event* e );

    //Moves the dot (it will keep changing depending on the file you want to compile)
    void (*move)(struct Dot*, Tile* );

    //centers the camera over the dot
    void (*setCamera)(struct Dot*, SDL_Rect* camera);

    //Shows the dot on the screen
    void (*render)(struct Dot*, SDL_Rect* camera);

    //Collision box of the dot
    SDL_Rect mBox;

    //Shows the dot on the screen relative to the camera
    // void (*render)(int camX, int camY);

    //gets the collision boxes
    // SDL_Rect* (*getColliders)(struct Dot* );
    // gets collision circle
    Circle_s* (*getCollider)(struct Dot*);

    //Moves the collision boxes relative to the dot's offset
    void (*shiftColliders)(struct Dot*);
    //The X and Y offsets of the dot
    int mPosX; 
    int mPosY;
    //The velocity of the dot
    int mVelX;
    int mVelY;
    Circle_s* mCollider;
    //moves the collision circle relative to the dot's offset
    SDL_Rect* mColliders;
    //particles variables
    Particle** particles;
    //shows the particles
    void (*renderParticles) (Particle*);
};

struct LTimer
{
    void (*start)();
    void (*stop)();
    void (*pause)();
    void (*unpause)();
    //Gets the timer's time
    Uint32 (*getTicks)();

    //Checks the status of the timer
    bool (*isStarted)();
    bool (*isPaused)();
    //The clock time when the timer started
    Uint32 mStartTicks;

    //The ticks stored when the timer was paused
    Uint32 mPausedTicks;

    //The timer status
    bool mPaused;
    bool mStarted;
};

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
    Sint32 gData[ TOTAL_DATA ];
    lTexture_s* gDataTexture[TOTAL_DATA];
    SDL_Window* gWindow;
    SDL_Renderer* gRenderer;
    lTexture_s* gTextTexture;
    lTexture_s* gArrowTexture;
    lTexture_s* gFooTexture;
    lTexture_s* gBackgroundTexture; 
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
    lTexture_s* gTimeTextTexture;
    lTexture_s* gStartPromptTexture;
    lTexture_s* gPausePromptTexture;
    lTexture_s* gFPSTextTexture; 
    lTexture_s* gDotTexture; 
    lTexture_s* gBGTexture; 
    lTexture_s* gInputTextTexture; 
    lTexture_s* gStreamingTexture;
    lTexture_s* gTargetTexture;
    DataStream* gDataStream; 
    //tile texture
    lTexture_s* gTileTexture; 
    SDL_Rect gTileClips[ TOTAL_TILE_SPRITES ];
    //particle textures
    lTexture_s* gRedTexture;
    lTexture_s* gGreenTexture;
    lTexture_s* gBlueTexture;
    lTexture_s* gShimmerTexture;
    TTF_Font* gFont;
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
    //audio settings
    //Number of available devices
    lTexture_s* gDeviceTextures[ MAX_RECORDING_DEVICES ];
    int gRecordingDeviceCount;
    //Received audio spec
    SDL_AudioSpec gReceivedRecordingSpec;
    SDL_AudioSpec gReceivedPlaybackSpec;
    //Size of data buffer
    Uint32 gBufferByteSize;
    //Maximum position in data buffer for recording
    Uint32 gBufferByteMaxPosition;
    //window's variables
    // lWindow* gWindow;
    lWindow** gWindows;
    lTexture_s* gSceneTexture;
    int gTotalDisplays;
    SDL_Rect* gDisplayBounds;
    LBitmapFont* gBitmapFont;
};

struct Tile_s
{
    void (*render)(Tile* , SDL_Rect*, CL_Instance* );
    SDL_Rect mBox;
    int mType;
};

struct LBitmapFont_s
{
    bool (*buildFont)(LBitmapFont*, CL_Instance*, char *path);
    void (*renderText)(LBitmapFont*, CL_Instance*, int x, int y, char* text);
    lTexture_s* mFontTexture;
    SDL_Rect mChars[ 256 ];
    int mNewLine, mSpace;
};

struct lTexture
{
    bool (*loadFromFile)(lTexture_s*, CL_Instance*, char *);
    bool (*loadPixelsFromFile)( lTexture_s*, CL_Instance* , char*);
    bool (*loadFromPixels)(lTexture_s* , CL_Instance*);
    // void (*render)(lTexture_s*, CL_Instance* , int, int ); // this one is for color
    // void (*render)(lTexture_s*, CL_Instance* , int, int, SDL_Rect* ); // this one is for sprite
    void (*render)(lTexture_s*, CL_Instance* , int, int, SDL_Rect* , double , SDL_Point* , SDL_RendererFlip );
    // void (*render)(lTexture_s*, CL_Instance* , int, int, SDL_RendererFlip ); // this one is for handling button events

    //creates blank texture (it will keep changing depending on the file you want to compile)
    bool (*createBlank)(lTexture_s*, CL_Instance*, int, int, SDL_TextureAccess access );
    bool (*loadFromRenderedText)(lTexture_s* , CL_Instance*, char*, SDL_Color);
    int (*getWidth)(lTexture_s*);
    int (*getHeight)(lTexture_s*);
    void (*setColor)(SDL_Texture* ,Uint8, Uint8, Uint8);
    void (*setBlendMode)(SDL_Texture* , SDL_BlendMode);
    void (*setAlpha)(SDL_Texture*, Uint8);
    SDL_Texture* mTexture;
    int mWidth;
    int mHeight;

    // set self as render target
    void (*setAsRenderTarget)(lTexture_s*, CL_Instance* );

    //Pixel accessors
    Uint32 (*getPixel32) (lTexture_s*, Uint32 , Uint32 );
    Uint32* (*getPixels32) (lTexture_s* );
    Uint32 (*getPitch32)(lTexture_s*);
    Uint32 (*mapRGBA)(lTexture_s*, Uint8 r, Uint8 g, Uint8 b, Uint8 a);
    void (*copyRawPixels32)(lTexture_s*, void* pixels);
    bool (*lockTexture)(lTexture_s*);
    bool (*unlockTexture)(lTexture_s*);

    //surface pixels
    SDL_Surface* mSurfacePixels;

    //Raw pixels
    void* mRawPixels;
    int mRawPitch;
};

struct Lwindow
{
    //Creates window
    bool (*init) (lWindow*);

    //Handles window events
    void (*handleEvent) (lWindow*,  CL_Instance* , SDL_Event* );

    //focueses on window
    void (*focus) (lWindow*);

    void (*render) (lWindow*);

    //Creates renderer from internal window
    SDL_Renderer* (*createRenderer) (lWindow*);
    //Deallocates internals
    void (*free_window)(lWindow*);

    bool (*hasMouseFocus)(lWindow*);
    bool (*hasKeyboardFocus)(lWindow*);
    bool (*isMinimized)(lWindow*);

    //Window data
    SDL_Window* mWindow;
    SDL_Renderer* mRenderer;
    int mWindowID;
    int mWindowDisplayID;

    //Window dimensions
    int mWidth;
    int mHeight;

    //Window focus
    bool mMouseFocus;
    bool mKeyboardFocus;
    bool mFullScreen;
    bool mMinimized;
    bool mShown;
};

struct Particle_s
{
    void (*render) (Particle* , CL_Instance* );
    bool (*isDead)(Particle* );
    //offsets
    int mPosX, mPosY;
    //current frame
    int mFrame;
    //Type of partcle
    lTexture_s *mTexture;
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

// global variables
SDL_Color gTextColor =  { 0, 0, 0, 0xFF };
//Recording data buffer
Uint8* gRecordingBuffer = NULL;
//Position in data buffer
Uint32 gBufferBytePosition = 0;
// Definition of lTexture_s

int init_instance(SDL_Instance *);
void draw_stuff(SDL_Instance* );
int poll_events();
// bool loadMedia(SDL_Instance *);
// bool loadMedia(CL_Instance *);
bool loadMedia_Texture(SDL_Instance *);
bool loadMedia_Color(CL_Instance *);
bool loadMedia_Sprite(CL_Instance *);
bool loadMedia_Modulation(CL_Instance *);
bool loadMedia_Geometry();
// bool checkCollision(SDL_Rect *a, SDL_Rect *b); // keep changing depending on the file
bool circle_checkCollision( Circle_s* a, Circle_s* b);
bool rectangle_checkCollision( Circle_s* a, SDL_Rect* b);
//Calculates distance squared between two points
double distanceSquared( int x1, int y1, int x2, int y2 );
// void close(SDL_Instance *);
void close_color(CL_Instance *);
// void close_sprite(CL_Instance *);
void close_instance(CL_Instance *);
SDL_Surface* loadSurface(char* , SDL_Instance* );
SDL_Texture* loadTexture(char *path, SDL_Instance* );
void audioRecordingCallback( void* userdata, Uint8* stream, int len );
void audioPlaybackCallback( void* userdata, Uint8* stream, int len );
bool init_window(lWindow* window);

#endif
