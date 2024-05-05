#include "demo.h"

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

void render_window(lWindow* self)
{
	if (!self->mMinimized)
	{
		//Clear screen
		SDL_SetRenderDrawColor( self->mRenderer, 0xFF, 0xFF, 0xFF, 0xFF );
		SDL_RenderClear(self->mRenderer);

		//Update screen
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

void close_sprite(CL_Instance* instance)
{
	//Destroy windows
	for( int i = 0; i < TOTAL_WINDOWS; ++i )
	{
		free_window( instance->gWindows[ i ] );
	}
    SDL_Quit();
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

SDL_Renderer* createRenderer(lWindow* self)
{
	return SDL_CreateRenderer( self->mWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC );
}

void handleEvent(lWindow* self, CL_Instance* instance, SDL_Event* e)
{
	//Window event occured
	if( e->type == SDL_WINDOWEVENT && e->window.windowID == self->mWindowID )
	{
		//Caption update flag
		bool updateCaption = false;

		switch( e->window.event )
		{
			//Window appeared
			case SDL_WINDOWEVENT_SHOWN:
			self->mShown = true;
			break;

			//Window disappeared
			case SDL_WINDOWEVENT_HIDDEN:
			self->mShown = false;
			break;

			//Get new dimensions and repaint
			case SDL_WINDOWEVENT_SIZE_CHANGED:
			self->mWidth = e->window.data1;
			self->mHeight = e->window.data2;
			SDL_RenderPresent( self->mRenderer );
			break;

			//Repaint on expose
			case SDL_WINDOWEVENT_EXPOSED:
			SDL_RenderPresent( self->mRenderer );
			break;

			//Mouse enter
			case SDL_WINDOWEVENT_ENTER:
			self->mMouseFocus = true;
			updateCaption = true;
			break;
			
			//Mouse exit
			case SDL_WINDOWEVENT_LEAVE:
			self->mMouseFocus = false;
			updateCaption = true;
			break;

			//Keyboard focus gained
			case SDL_WINDOWEVENT_FOCUS_GAINED:
			self->mKeyboardFocus = true;
			updateCaption = true;
			break;
			
			//Keyboard focus lost
			case SDL_WINDOWEVENT_FOCUS_LOST:
			self->mKeyboardFocus = false;
			updateCaption = true;
			break;

			//Window minimized
			case SDL_WINDOWEVENT_MINIMIZED:
            self->mMinimized = true;
            break;

			//Window maximized
			case SDL_WINDOWEVENT_MAXIMIZED:
			self->mMinimized = false;
            break;
			
			//Window restored
			case SDL_WINDOWEVENT_RESTORED:
			self->mMinimized = false;
            break;

			//Hide on close
			case SDL_WINDOWEVENT_CLOSE:
			SDL_HideWindow( self->mWindow );
			break;
		}

		//Update window caption with new data
		if( updateCaption )
		{
			char caption[80];
        sprintf(caption, "SDL Tutorial - ID: %d - MouseFocus: %s KeyboardFocus: %s", self->mWindowID, ( (self->mMouseFocus) ? "On" : "Off"), ( ( self->mKeyboardFocus ) ? "On" : "Off"  ) );
			SDL_SetWindowTitle( self->mWindow, caption );
		}
	}
}

void focus(lWindow* self)
{
	//Restore window if needed
	if (!self->mShown)
	{
		SDL_ShowWindow( self->mWindow );
	}
	//Move window forward
	SDL_RaiseWindow( self->mWindow );
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

void windows_constructor(lWindow* self)
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

bool init_window(lWindow* self)
{
	//Create window
	self->mWindow = SDL_CreateWindow( "SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE );
	if( self->mWindow != NULL )
	{
		self->mMouseFocus = true;
		self->mKeyboardFocus = true;
		self->mWidth = SCREEN_WIDTH;
		self->mHeight = SCREEN_HEIGHT;

        //Create renderer for window
        self->mRenderer = SDL_CreateRenderer( self->mWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC );
		printf("we are here kk1\n");

		if( self->mRenderer == NULL )
		{
			printf( "Renderer could not be created! SDL Error: %s\n", SDL_GetError() );
			SDL_DestroyWindow( self->mWindow );
			self->mWindow = NULL;
		}
		else
		{
			//Initialize renderer color
			SDL_SetRenderDrawColor( self->mRenderer, 0xFF, 0xFF, 0xFF, 0xFF );

			//Grab window identifier
			self->mWindowID = SDL_GetWindowID( self->mWindow );

			//Flag as opened
			self->mShown = true;
		}
	}
	else
	{
		printf( "Window could not be created! SDL Error: %s\n", SDL_GetError() );
	}

	return self->mWindow != NULL && self->mRenderer != NULL;
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
		if (!init_window(instance->gWindows[0]))
		{
			printf("Window 0 could not be created!\n");
			success = false;
		}
	}

	return success;
}


bool loadMedia(CL_Instance* instance)
{
	//Loading success flag
	bool success = true;

	//Open the font
	if( !instance->gSceneTexture->loadFromFile(instance->gSceneTexture, instance, "img/window.png") )
	{
		printf( "Failed to load lazy font! SDL_ttf Error: %s\n", TTF_GetError() );
		success = false;
	}

	return success;
}

int main()
{
    CL_Instance instance;

	instance.gWindows = malloc(sizeof(lWindow) * TOTAL_WINDOWS);
	for (int i = 0 ; i < TOTAL_WINDOWS ; i++ )
	{
		lWindow window;
		instance.gWindows[i] = &window;
		windows_constructor(instance.gWindows[i]);
	}

    // windows_constructor(&instance.gWindows[0]);


    if ( !init(&instance) )
    {
        printf("Failed to initialize!\n");
    } else
    {
		//Initialize the rest of the windows
		for( int i = 1; i < TOTAL_WINDOWS; ++i )
		{
			if ( !init_window ( instance.gWindows[ i ] ) )
				printf("unable to create window %d\n", i);
		}


		//Main loop flag
		bool quit = false;

		//Event handler
		SDL_Event e;

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

				//Handle window events
				for( int i = 0; i < TOTAL_WINDOWS; ++i )
				{
					instance.gWindows[ i ]->handleEvent( instance.gWindows[ i ], &instance, &e );
				}

				//Pull up window
				if( e.type == SDL_KEYDOWN )
				{
					switch( e.key.keysym.sym )
					{
						case SDLK_1:
						instance.gWindows[ 0 ]->focus(instance.gWindows[0]);
						break;

						case SDLK_2:
						instance.gWindows[ 1 ]->focus(instance.gWindows[1]);
						break;
							
						case SDLK_3:
						instance.gWindows[ 2 ]->focus(instance.gWindows[2]);
						break;
					}
				}
			}

			//Update all windows
			for( int i = 0; i < TOTAL_WINDOWS; ++i )
			{
				instance.gWindows[ i ]->render(instance.gWindows[ i ]);
			}
				
			//Check all windows
			bool allWindowsClosed = true;
			for( int i = 0; i < TOTAL_WINDOWS; ++i )
			{
				if( instance.gWindows[ i ]->mShown )
				{
					allWindowsClosed = false;
					break;
				}
			}

			//Application closed all windows
			if( allWindowsClosed )
			{
				quit = true;
			}
		}
	
	}

    close_sprite(&instance);
    return 0;
}
