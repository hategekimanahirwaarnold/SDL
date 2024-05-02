#include "demo.h"

bool loadMedia(CL_Instance* instance)
{
	//Loading success flag
	bool success = true;

	//Open the font
	instance->gFont = TTF_OpenFont( "img/lazy.ttf", 28 );
	if( instance->gFont == NULL )
	{
		printf( "Failed to load lazy font! SDL_ttf Error: %s\n", TTF_GetError() );
		success = false;
	}

	return success;
}

bool loadFromFile(lTexture_s *self, CL_Instance* instance, char* path)
{
    SDL_Texture* newTexture = NULL;
    SDL_Surface* loadedSurface = IMG_Load(path);

    if (loadedSurface == NULL)
    {
        printf("Unable to load image %s! SDL_image Error: %s\n", path, IMG_GetError());
    } else
    {
        SDL_SetColorKey(loadedSurface, SDL_TRUE, SDL_MapRGB (loadedSurface->format, 0, 0xFF, 0xFF));
        newTexture = SDL_CreateTextureFromSurface(instance->gRenderer, loadedSurface);
        if (newTexture == NULL)
        {
            printf("Unable to create texture from %s! SDL Error: %s\n", path, SDL_GetError() );
        }
        else
        {
            self->mWidth = loadedSurface->w;
            self->mHeight = loadedSurface->h;
        }
        //Get rid of old loaded surface
        SDL_FreeSurface(loadedSurface);
    }
    self->mTexture = newTexture;

    return self->mTexture != NULL;
}

void render (lTexture_s *self, CL_Instance* instance, int x, int y, SDL_Rect* clip, double angle, SDL_Point* center, SDL_RendererFlip flip)
{
    SDL_Rect renderQuad = { x, y, self->mWidth, self->mHeight };

    //Set clip rendering dimensions
    if ( clip != NULL )
    {
        renderQuad.w = clip->w;
        renderQuad.h = clip->h;
    }

    SDL_RenderCopyEx( instance->gRenderer, self->mTexture, clip, &renderQuad, angle, center, flip);
}

void close_sprite(CL_Instance* instance)
{
    SDL_DestroyRenderer(instance->gRenderer);
    SDL_DestroyWindow(instance->gWindow);
    instance->gWindow = NULL;
    instance->gRenderer = NULL;
    TTF_CloseFont(instance->gFont);
    instance->gFont = NULL;
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
}

bool init(CL_Instance* instance)
{
	//Initialization flag
	bool success = true;

	//Initialize SDL
	if( SDL_Init( SDL_INIT_VIDEO ) < 0 )
	{
		printf( "SDL could not initialize! SDL Error: %s\n", SDL_GetError() );
		success = false;
	}
	else
	{
		//Set texture filtering to linear
		if( !SDL_SetHint( SDL_HINT_RENDER_SCALE_QUALITY, "1" ) )
		{
			printf( "Warning: Linear texture filtering not enabled!" );
		}

		//Create window
		instance->gWindow = SDL_CreateWindow( "SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN );
		if( instance->gWindow == NULL )
		{
			printf( "Window could not be created! SDL Error: %s\n", SDL_GetError() );
			success = false;
		}
		else
		{
			//Create renderer for window
			instance->gRenderer = SDL_CreateRenderer( instance->gWindow, -1, SDL_RENDERER_ACCELERATED );
			if( instance->gRenderer == NULL )
			{
				printf( "Renderer could not be created! SDL Error: %s\n", SDL_GetError() );
				success = false;
			}
			else
			{
				//Initialize renderer color
				SDL_SetRenderDrawColor( instance->gRenderer, 0xFF, 0xFF, 0xFF, 0xFF );

				//Initialize PNG loading
				int imgFlags = IMG_INIT_PNG;
				if( !( IMG_Init( imgFlags ) & imgFlags ) )
				{
					printf( "SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError() );
					success = false;
				}
                //Initialize SDL_ttf
                if (TTF_Init() == -1)
                {
                    printf("SDL_ttf could not initialize! SDL_ttf Error: %s\n", TTF_GetError() );
                    success = false;
                }
			}
		}
	}

	return success;
}

void setBlendMode(SDL_Texture* mTexture,SDL_BlendMode blending)
{
    //Set blending function
    SDL_SetTextureBlendMode(mTexture, blending);
}

void setAlpha(SDL_Texture* mTexture, Uint8 alpha)
{
    //Modulated texture alpha
    SDL_SetTextureAlphaMod(mTexture, alpha);
}

bool loadFromRenderedText(lTexture_s* self, CL_Instance* instance, char* textureText, SDL_Color textColor)
{
    SDL_Surface* textSurface = TTF_RenderText_Solid(instance->gFont, textureText, textColor);
    if (textSurface == NULL)
    {
        printf("Unable to render text sufrace! SDL_ttf Error: %s", TTF_GetError());
    } else
    {
        self->mTexture = SDL_CreateTextureFromSurface(instance->gRenderer, textSurface);
        if (self->mTexture == NULL)
        {
            printf("Unable to create texture from rendered text! SDL Error: %s\n", SDL_GetError() );
        } else
        {
            self->mWidth = textSurface->w;
            self->mHeight = textSurface->h;
        }
        //remove old surface
        SDL_FreeSurface(textSurface);
    }
    return self->mTexture != NULL;
}

void constructor(lTimer* self)
{
    self->mStartTicks = 0;
    self->mPausedTicks = 0;
    self->mPaused = false;
    self->mStarted = false;
}

void start(lTimer* self)
{
    self->mStarted = true;
    self->mPaused = false;
    //get current time
    self->mStartTicks = SDL_GetTicks();
    self->mPausedTicks = 0;
}

void stop(lTimer* self)
{
    self->mStarted = false;
    self->mPaused = false;
    //clear tick variables
    self->mStartTicks = 0;
    self->mPausedTicks = 0;
}

void pause(lTimer* self)
{
    if (self->mStarted && !self->mPaused)
    {
        //pause the timer
        self->mPaused = true;
        //calculate the paused ticks
        self->mPausedTicks = SDL_GetTicks() - self->mStartTicks;
        self->mStartTicks = 0;
    }
}

void unpause(lTimer* self)
{
    //if the timer is running and paused
    if (self->mStarted && self->mPaused)
    {
        //unpause the timer
        self->mPaused = false;
        //Reset the starting ticks
        self->mStartTicks = SDL_GetTicks() - self->mPausedTicks;
        //reset the paused ticks
        self->mPausedTicks = 0;
    }
}

Uint32 getTicks(lTimer* self)
{
    Uint32 time = 0;
    //if the timer is running
    if (self->mStarted)
    {
        //it the timer is paused
        if (self->mPaused)
        {
            //Return the number of ticks when the timer was paused
            time = self->mPausedTicks;
        }
        else
        {
            //return the current time minus the start time
            time = SDL_GetTicks() - self->mStartTicks;
        }
    }

    return time;
}

bool isStarted(lTimer* self)
{
    //Timer is running and puased or unpaused
    return self->mStarted;
}
bool isPaused(lTimer* self)
{
    //timer is running and paused
    return self->mPaused && self->mStarted;
}

void texture_constructor(lTexture_s** self)
{
    *self = (lTexture_s*)malloc(sizeof(lTexture_s));
    if (*self == NULL) {
        // Handle memory allocation failure
        printf("Failed to allocate memory for new texture!\n");
        return;
    }
    (*self)->loadFromFile = &loadFromFile;
    (*self)->render = &render;
    (*self)->setAlpha = &setAlpha;
    (*self)->setBlendMode = &setBlendMode;
    (*self)->loadFromRenderedText = &loadFromRenderedText;
}


void time_constructor(lTimer* self)
{
    (self)->start = &start;
    (self)->stop = &stop;
    (self)->pause = &pause;
    (self)->unpause = &unpause;
    (self)->getTicks = &getTicks;
    (self)->isStarted = &isStarted;
    (self)->isPaused = &isPaused;
    (self)->mStartTicks = 0;
    (self)->mPausedTicks = 0;
    (self)->mPaused = false;
    (self)->mStarted = false;
}

int main()
{
    CL_Instance instance;
    lTimer fpsTimer;
    texture_constructor(&instance.gFPSTextTexture);
    time_constructor(&fpsTimer);

    SDL_RendererFlip flipType = SDL_FLIP_NONE;

    if ( !init(&instance) )
    {
        printf("Failed to initialize!\n");
    } else
    {
        // load media
        if (!loadMedia(&instance))
        {
            printf("Failed to load media!\n");
        } else
        {

            bool quit = false;
            int countedFrames = 0;
            SDL_Event e;
            //Set text color as black
            SDL_Color textColor = { 0, 0, 0, 255 };

            //Current time start time
            Uint32 startTime = 0;
            char timeText[100] = "Milliseconds since start time: ";
            double num;
            char formatted[20];
            int position = strlen(timeText);
            printf("we are here!\n");
            fpsTimer.start(&fpsTimer);

            while(!quit)
            {
                while(SDL_PollEvent( &e ) != 0)
                {
                    if (e.type == SDL_QUIT)
                    {
                        quit = true;
                    } 

                }

                //Calculate and correct frame per second
                float avgFPS = countedFrames / (fpsTimer.getTicks(&fpsTimer) / 1000.f );
                if (avgFPS > 2000000)
                {
                    avgFPS = 0;
                }
                sprintf(formatted, "%f", avgFPS);
                strcpy(timeText + position, formatted);

                
				//Render text
				if( !instance.gFPSTextTexture->loadFromRenderedText(instance.gFPSTextTexture, &instance, timeText, textColor ) )
				{
					printf( "Unable to render FPS texture!\n" );
				}
                // clear screen
                SDL_SetRenderDrawColor(instance.gRenderer, 0xFF, 0xFF, 0xFF, 0xFF );
                SDL_RenderClear(instance.gRenderer);

                // render arrow texture
                instance.gFPSTextTexture->render(instance.gFPSTextTexture, &instance, (SCREEN_WIDTH - instance.gFPSTextTexture->mWidth) / 2, (SCREEN_HEIGHT - instance.gFPSTextTexture->mHeight) / 2, NULL, 0, NULL, flipType);

                //Update screen
                SDL_RenderPresent(instance.gRenderer);
                ++countedFrames;
            }
        }
    }
    close_sprite(&instance);
    return 0;
}
