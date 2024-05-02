#include "demo.h"

bool loadMedia(CL_Instance* instance)
{
	//Loading success flag
	bool success = true;

	if( !instance->gDotTexture->loadFromFile( instance->gDotTexture, instance, "img/dot.bmp" ))
	{
		printf( "Failed to load dot texture!\n" );
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

void handleEvent(Dot_s* self, SDL_Event* e )
{
    //If a key was pressed
    if( e->type == SDL_KEYDOWN && e->key.repeat == 0 )
    {
        //Adjust the velocity
        switch( e->key.keysym.sym )
        {
            case SDLK_UP: self->mVelY -= self->DOT_VEL; break;
            case SDLK_DOWN: self->mVelY += self->DOT_VEL; break;
            case SDLK_LEFT: self->mVelX -= self->DOT_VEL; break;
            case SDLK_RIGHT: self->mVelX += self->DOT_VEL; break;
        }
    }
        //If a key was released
    else if( e->type == SDL_KEYUP && e->key.repeat == 0 )
    {
        //Adjust the velocity
        switch( e->key.keysym.sym )
        {
            case SDLK_UP: self->mVelY += self->DOT_VEL; break;
            case SDLK_DOWN: self->mVelY -= self->DOT_VEL; break;
            case SDLK_LEFT: self->mVelX += self->DOT_VEL; break;
            case SDLK_RIGHT: self->mVelX -= self->DOT_VEL; break;
        }
    }
}

void move(Dot_s* self, SDL_Rect* otherColliders)
{
    self->mPosX += self->mVelX;
    self->shiftColliders(self);
    // if the dot went too far to the left or right
    if ( ( self->mPosX < 0 ) || ( self->mPosX + self->DOT_WIDTH > SCREEN_WIDTH ) || checkCollision( self->mColliders, otherColliders))
    {
        //Move back
        self->mPosX -= self->mVelX;
        self->shiftColliders(self);
    }
    // Move the dot up or down
    self->mPosY += self->mVelY;
    self->shiftColliders(self);
    // If the dot went too far up or down
    if ( ( self->mPosY < 0 ) || ( self->mPosY + self->DOT_HEIGHT > SCREEN_HEIGHT ) || checkCollision( self->mColliders, otherColliders ) )
    {
        //Move back
        self->mPosY -= self->mVelY;
        self->shiftColliders(self);
    }
}

bool checkCollision(SDL_Rect* a, SDL_Rect* b)
{
    //The sides of the rectangles
    int leftA, leftB;
    int rightA, rightB;
    int topA, topB;
    int bottomA, bottomB;
    int size_a = 11;
    int size_b = 11;
    // printf("size a: %d, size b: %d \n", size_a, size_b);
    //Go through the A boxes
    for( int Abox = 0; Abox < size_a ; Abox++ )
    {
        //Calculate the sides of rect A
        leftA = a[ Abox ].x;
        rightA = a[ Abox ].x + a[ Abox ].w;
        topA = a[ Abox ].y;
        bottomA = a[ Abox ].y + a[ Abox ].h;

        //Go through the B boxes
        for( int Bbox = 0; Bbox < size_b; Bbox++ )
        {
            //Calculate the sides of rect B
            leftB = b[ Bbox ].x;
            rightB = b[ Bbox ].x + b[ Bbox ].w;
            topB = b[ Bbox ].y;
            bottomB = b[ Bbox ].y + b[ Bbox ].h;

            //If no sides from A are outside of B
            if( ( ( bottomA <= topB ) || ( topA >= bottomB ) || ( rightA <= leftB ) || ( leftA >= rightB ) ) == false )
            {
                //A collision is detected
                return true;
            }
        }
    }

    //If neither set of collision boxes touched
    return false;
}

void shiftColliders(Dot_s* self)
{
    //the row offset
    int r = 0;
    int size = 11;
    //go through the dot's collision boxes
    for (int set = 0; set < size; ++set)
    {
        //center the collision box
        self->mColliders[ set ].x = self->mPosX + (self->DOT_WIDTH - self->mColliders[ set ].w) / 2;

        //set the collision at its row offset
        self->mColliders[ set ].y = self->mPosY + r;
        
        //move the row offset down the height of the collision box
        r += self->mColliders[ set ].h;
    }
}

SDL_Rect* getColliders(Dot_s* self)
{
    return self->mColliders;
}

void dot_constructor(Dot_s* self, int x, int y)
{
    self->mPosX = x;
    self->mPosY = y;
    self->mColliders = malloc(sizeof(SDL_Rect) * 11);
    self->mVelX = 0;
    self->mVelY = 0;
    //Initialize the collision boxes' width and height
    self->mColliders[ 0 ].w = 6;
    self->mColliders[ 0 ].h = 1;

    self->mColliders[ 1 ].w = 10;
    self->mColliders[ 1 ].h = 1;

    self->mColliders[ 2 ].w = 14;
    self->mColliders[ 2 ].h = 1;

    self->mColliders[ 3 ].w = 16;
    self->mColliders[ 3 ].h = 2;

    self->mColliders[ 4 ].w = 18;
    self->mColliders[ 4 ].h = 2;

    self->mColliders[ 5 ].w = 20;
    self->mColliders[ 5 ].h = 6;

    self->mColliders[ 6 ].w = 18;
    self->mColliders[ 6 ].h = 2;

    self->mColliders[ 7 ].w = 16;
    self->mColliders[ 7 ].h = 2;

    self->mColliders[ 8 ].w = 14;
    self->mColliders[ 8 ].h = 1;

    self->mColliders[ 9 ].w = 10;
    self->mColliders[ 9 ].h = 1;

    self->mColliders[ 10 ].w = 6;
    self->mColliders[ 10 ].h = 1;

    //Initialize colliders relative to position
    self->shiftColliders = &shiftColliders;
    self->getColliders = &getColliders;
    self->handleEvent = &handleEvent;
    self->move = &move;
    self->DOT_WIDTH = 20;
    self->DOT_HEIGHT = 20;
    self->DOT_VEL = 1;
    self->shiftColliders(self);
}

int main()
{
    CL_Instance instance;
    texture_constructor(&instance.gDotTexture);
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
            SDL_Event e;
            //Set the wall
            Dot_s* dot = malloc(sizeof(Dot_s));
            Dot_s* otherDot = malloc(sizeof(Dot_s));
            dot_constructor(dot, 0, 0);
            dot_constructor(otherDot, SCREEN_WIDTH / 4, SCREEN_HEIGHT / 4);
           while(!quit)
            {
                while(SDL_PollEvent( &e ) != 0)
                {
                    if (e.type == SDL_QUIT)
                    {
                        quit = true;
                    } 
                    //Handle input for the dot
                    dot->handleEvent(dot, &e);
                }
                //Move the dot anc check collision
                dot->move( dot, otherDot->getColliders(otherDot));

                // clear screen
                SDL_SetRenderDrawColor(instance.gRenderer, 0xFF, 0xFF, 0xFF, 0xFF );
                SDL_RenderClear(instance.gRenderer);

                //Render wall
                SDL_SetRenderDrawColor( instance.gRenderer, 0x00, 0x00, 0x00, 0xFF );        
 
                // render texture
                instance.gDotTexture->render(instance.gDotTexture, &instance, dot->mPosX, dot->mPosY, NULL, 0, NULL, flipType);
                instance.gDotTexture->render(instance.gDotTexture, &instance, otherDot->mPosX, otherDot->mPosY, NULL, 0, NULL, flipType);

                //Update screen
                SDL_RenderPresent(instance.gRenderer);
            }
        }
    }
    close_sprite(&instance);
    return 0;
}
