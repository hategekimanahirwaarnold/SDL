#include "demo.h"

bool loadFromFile(lTexture_s *self, CL_Instance *instance, char *path)
{
    SDL_Texture *newTexture = NULL;
    SDL_Surface *loadedSurface = IMG_Load(path);

    if (loadedSurface == NULL)
    {
        printf("Unable to load image %s! SDL_image Error: %s\n", path, IMG_GetError());
    }
    else
    {
        SDL_SetColorKey(loadedSurface, SDL_TRUE, SDL_MapRGB(loadedSurface->format, 0, 0xFF, 0xFF));
        newTexture = SDL_CreateTextureFromSurface(instance->gRenderer, loadedSurface);
        if (newTexture == NULL)
        {
            printf("Unable to create texture from %s! SDL Error: %s\n", path, SDL_GetError());
        }
        else
        {
            self->mWidth = loadedSurface->w;
            self->mHeight = loadedSurface->h;
        }
        // Get rid of old loaded surface
        SDL_FreeSurface(loadedSurface);
    }
    self->mTexture = newTexture;

    return self->mTexture != NULL;
}

void render(lTexture_s *self, CL_Instance *instance, int x, int y, SDL_Rect *clip, double angle, SDL_Point *center, SDL_RendererFlip flip)
{
    SDL_Rect renderQuad = {x, y, self->mWidth, self->mHeight};

    // Set clip rendering dimensions
    if (clip != NULL)
    {
        renderQuad.w = clip->w;
        renderQuad.h = clip->h;
    }

    SDL_RenderCopyEx(instance->gRenderer, self->mTexture, clip, &renderQuad, angle, center, flip);
}

void render_window(lWindow *self)
{
    if (!self->mMinimized)
    {
        // Clear screen
        SDL_SetRenderDrawColor(self->mRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
        SDL_RenderClear(self->mRenderer);

        // Update screen
        SDL_RenderPresent(self->mRenderer);
    }
}

void free_texture(lTexture_s *self)
{
    if (self->mTexture != NULL)
    {
        SDL_DestroyTexture(self->mTexture);
        self->mTexture = NULL;
        self->mWidth = 0;
        self->mHeight = 0;
    }
}

void free_dot(Dot_s* self)
{
    for (int i = 0; i < TOTAL_PARTICLES; i++)
    {
        free(self->particles[i]);
    }
};

void free_window(lWindow *self)
{
    if (self->mWindow != NULL)
    {
        SDL_DestroyWindow(self->mWindow);
    }
    self->mMouseFocus = false;
    self->mKeyboardFocus = false;
    self->mWidth = 0;
    self->mHeight = 0;
}

void close_sprite(CL_Instance *instance)
{
    free_texture(instance->gDotTexture);
    free_texture(instance->gRedTexture);
    free_texture(instance->gGreenTexture);
    free_texture(instance->gShimmerTexture);

    // Destroy windows
    for (int i = 0; i < TOTAL_WINDOWS; ++i)
    {
        free_window(instance->gWindows[i]);
    }
    SDL_Quit();
}

void setBlendMode(SDL_Texture *mTexture, SDL_BlendMode blending)
{
    // Set blending function
    SDL_SetTextureBlendMode(mTexture, blending);
}

void setAlpha(SDL_Texture *mTexture, Uint8 alpha)
{
    // Modulated texture alpha
    SDL_SetTextureAlphaMod(mTexture, alpha);
}

bool loadFromRenderedText(lTexture_s *self, CL_Instance *instance, char *textureText, SDL_Color textColor)
{
    SDL_Surface *textSurface = TTF_RenderText_Solid(instance->gFont, textureText, textColor);
    if (textSurface == NULL)
    {
        printf("Unable to render text sufrace! SDL_ttf Error: %s", TTF_GetError());
    }
    else
    {
        self->mTexture = SDL_CreateTextureFromSurface(instance->gRenderer, textSurface);
        if (self->mTexture == NULL)
        {
            printf("Unable to create texture from rendered text! SDL Error: %s\n", SDL_GetError());
        }
        else
        {
            self->mWidth = textSurface->w;
            self->mHeight = textSurface->h;
        }
        // remove old surface
        SDL_FreeSurface(textSurface);
    }
    return self->mTexture != NULL;
}

SDL_Renderer *createRenderer(lWindow *self)
{
    return SDL_CreateRenderer(self->mWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
}

void handleEvent(lWindow *self, CL_Instance *instance, SDL_Event *e)
{
    bool updateCaption = false;

    // If an event was detected for this window
    if (e->type == SDL_WINDOWEVENT && e->window.windowID == self->mWindowID)
    {
        switch (e->window.event)
        {
        // Window moved
        case SDL_WINDOWEVENT_MOVED:
            self->mWindowDisplayID = SDL_GetWindowDisplayIndex(self->mWindow);
            updateCaption = true;
            break;

        // Window appeared
        case SDL_WINDOWEVENT_SHOWN:
            self->mShown = true;
            break;

        // Window disappeared
        case SDL_WINDOWEVENT_HIDDEN:
            self->mShown = false;
            break;

        // Get new dimensions and repaint
        case SDL_WINDOWEVENT_SIZE_CHANGED:
            self->mWidth = e->window.data1;
            self->mHeight = e->window.data2;
            SDL_RenderPresent(self->mRenderer);
            break;

        // Repaint on expose
        case SDL_WINDOWEVENT_EXPOSED:
            SDL_RenderPresent(self->mRenderer);
            break;

        // Mouse enter
        case SDL_WINDOWEVENT_ENTER:
            self->mMouseFocus = true;
            updateCaption = true;
            break;

        // Mouse exit
        case SDL_WINDOWEVENT_LEAVE:
            self->mMouseFocus = false;
            updateCaption = true;
            break;

        // Keyboard focus gained
        case SDL_WINDOWEVENT_FOCUS_GAINED:
            self->mKeyboardFocus = true;
            updateCaption = true;
            break;

        // Keyboard focus lost
        case SDL_WINDOWEVENT_FOCUS_LOST:
            self->mKeyboardFocus = false;
            updateCaption = true;
            break;

        // Window minimized
        case SDL_WINDOWEVENT_MINIMIZED:
            self->mMinimized = true;
            break;

        // Window maximized
        case SDL_WINDOWEVENT_MAXIMIZED:
            self->mMinimized = false;
            break;

        // Window restored
        case SDL_WINDOWEVENT_RESTORED:
            self->mMinimized = false;
            break;

        // Hide on close
        case SDL_WINDOWEVENT_CLOSE:
            SDL_HideWindow(self->mWindow);
            break;
        }
    }
    else if (e->type == SDL_KEYDOWN)
    {
        // Display change flag
        bool switchDisplay = false;

        // Cycle through displays on up/down
        switch (e->key.keysym.sym)
        {
        case SDLK_UP:
            ++self->mWindowDisplayID;
            switchDisplay = true;
            break;

        case SDLK_DOWN:
            --self->mWindowDisplayID;
            switchDisplay = true;
            break;
        }

        // Display needs to be updated
        if (switchDisplay)
        {
            // Bound display index
            if (self->mWindowDisplayID < 0)
            {
                self->mWindowDisplayID = instance->gTotalDisplays - 1;
            }
            else if (self->mWindowDisplayID >= instance->gTotalDisplays)
            {
                self->mWindowDisplayID = 0;
            }

            // Move window to center of next display
            SDL_SetWindowPosition(self->mWindow, instance->gDisplayBounds[self->mWindowDisplayID].x + (instance->gDisplayBounds[self->mWindowDisplayID].w - self->mWidth) / 2, instance->gDisplayBounds[self->mWindowDisplayID].y + (instance->gDisplayBounds[self->mWindowDisplayID].h - self->mHeight) / 2);
            updateCaption = true;
        }
    }

    // Update window caption with new data
    if (updateCaption)
    {
        char caption[80];
        sprintf(caption, "SDL Tutorial - ID: %d Display: %d - MouseFocus: %s KeyboardFocus: %s", self->mWindowID, self->mWindowDisplayID, ((self->mMouseFocus) ? "On" : "Off"), ((self->mKeyboardFocus) ? "On" : "Off"));
        SDL_SetWindowTitle(self->mWindow, caption);
    }
}

void focus(lWindow *self)
{
    // Restore window if needed
    if (!self->mShown)
    {
        SDL_ShowWindow(self->mWindow);
    }
    // Move window forward
    SDL_RaiseWindow(self->mWindow);
}

void texture_constructor(lTexture_s **self)
{
    *self = (lTexture_s *)malloc(sizeof(lTexture_s));
    if (*self == NULL)
    {
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

void windows_constructor(lWindow *self)
{
    // lWindow window;
    // *self = &window;
    (self)->mWindow = NULL;
    (self)->mMouseFocus = true;
    (self)->mKeyboardFocus = true;
    (self)->mFullScreen = false;
    (self)->mMinimized = false;
    (self)->mShown = false;
    (self)->mWindowID = -1;
    (self)->mWidth = 0;
    (self)->mHeight = 0;
    (self)->handleEvent = &handleEvent;
    (self)->createRenderer = &createRenderer;
    (self)->focus = &focus;
    (self)->render = &render_window;
}

void particle_constructor(Particle* self, CL_Instance* instance, int x, int y)
{
    //Set offsets
    self->mPosX = x - 5 + ( rand() % 25 );
    self->mPosY = y - 5 + ( rand() & 25 );

    //Initialize animation
    self->mFrame = rand() & 5;

    //Set type
    switch( rand() & 3 )
    {
        case 0: self->mTexture = instance->gRedTexture; break;
        case 1: self->mTexture = instance->gGreenTexture; break;
        case 2: self->mTexture = instance->gBlueTexture; break;
    }
}

void move(Dot_s* self)
{
    //Move the dot left or right
    self->mPosX += self->mVelX;

    //If the dot went too far to the left or right
    if( ( self->mPosX < 0 ) || ( self->mPosX + self->DOT_WIDTH > SCREEN_WIDTH ) )
    {
        //Move back
        self->mPosX -= self->mVelX;
    }

    //Move the dot up or down
    self->mPosY += self->mVelY;

    //If the dot went too far up or down
    if( ( self->mPosY < 0 ) || ( self->mPosY + self->DOT_HEIGHT > SCREEN_HEIGHT ) )
    {
        //Move back
        self->mPosY -= self->mVelY;
    }
}

void dot_constructor(Dot_s* self, CL_Instance* instance)
{
    self->mPosX = 0;
    self->mPosY = 0;
    self->mVelX = 0;
    self->mVelY = 0;
    Particle particles[TOTAL_PARTICLES];
    //Initialize particles
    for ( int i = 0; i < TOTAL_PARTICLES; i++)
    {
        self->particles[ i ] = &particles[i];
        particle_constructor(self->particles[i], instance, self->mPosX, self->mPosY);
    }
}

void render_particle(Particle* self, CL_Instance* instance)
{
    //Show image
	self->mTexture->render(self->mTexture, instance, self->mPosX, self->mPosY, NULL, 0, NULL, SDL_FLIP_NONE );

    //Show shimmer
    if( self->mFrame % 2 == 0 )
    {
		instance->gShimmerTexture->render (instance->gShimmerTexture, instance, self->mPosX, self->mPosY, NULL, 0, NULL, SDL_FLIP_NONE );
    }

    //Animate
    self->mFrame++;
}

void renderParticles(Dot_s* self, CL_Instance* instance)
{
	//Go through particles
    for( int i = 0; i < TOTAL_PARTICLES; ++i )
    {
        //Delete and replace dead particles
        if( self->particles[i]->isDead(self->particles) )
        {
            free(self->particles[ i ]);
            Particle* newParticle = malloc(sizeof(Particle));
            self->particles[ i ] = newParticle;
            particle_constructor(self->particles[ i ], instance, self->mPosX, self->mPosY);
        }
    }

    //Show particles
    for( int i = 0; i < TOTAL_PARTICLES; ++i )
    {
        self->particles[ i ]->render(self->particles[i]);
    }
}

bool isDead(Particle* self)
{
    return self->mFrame > 10;
}

bool init_window(lWindow *self)
{
    // Create window
    self->mWindow = SDL_CreateWindow("SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
    if (self->mWindow != NULL)
    {
        self->mMouseFocus = true;
        self->mKeyboardFocus = true;
        self->mWidth = SCREEN_WIDTH;
        self->mHeight = SCREEN_HEIGHT;

        // Create renderer for window
        self->mRenderer = SDL_CreateRenderer(self->mWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

        if (self->mRenderer == NULL)
        {
            printf("Renderer could not be created! SDL Error: %s\n", SDL_GetError());
            SDL_DestroyWindow(self->mWindow);
            self->mWindow = NULL;
        }
        else
        {
            // Initialize renderer color
            SDL_SetRenderDrawColor(self->mRenderer, 0xFF, 0xFF, 0xFF, 0xFF);

            // Grab window identifier
            self->mWindowID = SDL_GetWindowID(self->mWindow);
            self->mWindowDisplayID = SDL_GetWindowDisplayIndex(self->mWindow);

            // Flag as opened
            self->mShown = true;
        }
    }
    else
    {
        printf("Window could not be created! SDL Error: %s\n", SDL_GetError());
    }

    return self->mWindow != NULL && self->mRenderer != NULL;
}

bool init(CL_Instance *instance)
{
    // Initialization flag
    bool success = true;

    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        printf("SDL could not initialize! SDL Error: %s\n", SDL_GetError());
        success = false;
    }
    else
    {
        // Set texture filtering to linear
        if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1"))
        {
            printf("Warning: Linear texture filtering not enabled!");
        }

        // Get number of displays
        instance->gTotalDisplays = SDL_GetNumVideoDisplays();
        if (instance->gTotalDisplays < 2)
        {
            printf("Warning: Only one display connected!");
        }

        // Get bounds of each display
        instance->gDisplayBounds = malloc(sizeof(SDL_Rect) * instance->gTotalDisplays);
        for (int i = 0; i < instance->gTotalDisplays; ++i)
        {
            SDL_GetDisplayBounds(i, &instance->gDisplayBounds[i]);
        }

        // Create window
        if (!init_window(instance->gWindow))
        {
            printf("Window could not be created!\n");
            success = false;
        }
    }

    return success;
}

bool loadMedia(CL_Instance *instance)
{

	//Loading success flag
	bool success = true;

	//Load dot texture
	if( !instance->gDotTexture->loadFromFile(instance->gDotTexture, instance, "img/dot.bmp" ) )
	{
		printf( "Failed to load dot texture!\n" );
		success = false;
	}

	//Load red texture
	if( !instance->gRedTexture->loadFromFile(instance->gRedTexture, instance, "img/red.bmp" ) )
	{
		printf( "Failed to load red texture!\n" );
		success = false;
	}

	//Load green texture
	if( !instance->gGreenTexture->loadFromFile(instance->gGreenTexture, instance, "img/green.bmp" ) )
	{
		printf( "Failed to load green texture!\n" );
		success = false;
	}

	//Load blue texture
	if( !instance->gBlueTexture->loadFromFile(instance->gBlueTexture, instance, "img/blue.bmp" ) )
	{
		printf( "Failed to load blue texture!\n" );
		success = false;
	}

	//Load shimmer texture
	if( !instance->gShimmerTexture->loadFromFile(instance->gShimmerTexture, instance, "img/shimmer.bmp" ) )
	{
		printf( "Failed to load shimmer texture!\n" );
		success = false;
	}
	
	//Set texture transparency
	instance->gRedTexture->setAlpha( instance->gRedTexture, 192 );
	instance->gGreenTexture->setAlpha( instance->gGreenTexture, 192 );
	instance->gBlueTexture->setAlpha( instance->gBlueTexture, 192 );
	instance->gShimmerTexture->setAlpha( instance->gShimmerTexture, 192 );

	return success;
}

int main()
{
    CL_Instance instance;
	//Start up SDL and create window
	if( !init(&instance) )
	{
		printf( "Failed to initialize!\n" );
	}
	else
	{
		//Load media
		if( !loadMedia(&instance) )
		{
			printf( "Failed to load media!\n" );
		}
		else
		{	
			//Main loop flag
			bool quit = false;

			//Event handler
			SDL_Event e;

			//The dot that will be moving around on the screen
			Dot_s dot;

			//While application is running
			while( !quit )
			{
				//Handle events on queue
				while( SDL_PollEvent( &e ) != 0 )
				{
					//User requests quit
					if( e.type == SDL_QUIT )
					{
						quit = true;
					}

					//Handle input for the dot
					handleEvent( &dot, &instance, &e );
				}

				//Move the dot
				move(&dot);

				//Clear screen
				SDL_SetRenderDrawColor( instance.gRenderer, 0xFF, 0xFF, 0xFF, 0xFF );
				SDL_RenderClear( instance.gRenderer );

				//Render objects
				instance.gDotTexture->render(instance.gDotTexture, &instance, dot.mPosX, dot.mPosY, NULL, 0, NULL, SDL_FLIP_NONE);
                renderParticles(&dot, &instance);
				//Update screen
				SDL_RenderPresent( instance.gRenderer );
			}
		}
	}

	//Free resources and close SDL
	close_sprite(&instance);

	return 0;
}
