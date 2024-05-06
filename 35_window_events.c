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
    free_texture(instance->gSceneTexture);
    SDL_DestroyRenderer(instance->gRenderer);
    //free window
    free_window(instance->gWindow);
    instance->gRenderer = NULL;
    IMG_Quit();
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

bool init_window(lWindow* window)
{
	//Create window
	window->mWindow = SDL_CreateWindow( "SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE );
	if( window->mWindow != NULL )
	{
		window->mMouseFocus = true;
		window->mKeyboardFocus = true;
		window->mWidth = SCREEN_WIDTH;
		window->mHeight = SCREEN_HEIGHT;
	}

	return window->mWindow != NULL;
}

SDL_Renderer* createRenderer(lWindow* self)
{
	return SDL_CreateRenderer( self->mWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC );
}

void handleEvent(lWindow* self, CL_Instance* instance, SDL_Event* e)
{
	//Window event occured
	if( e->type == SDL_WINDOWEVENT )
	{
		//Caption update flag
		bool updateCaption = false;

		switch( e->window.event )
		{
			//Get new dimensions and repaint on window size change
			case SDL_WINDOWEVENT_SIZE_CHANGED:
			self->mWidth = e->window.data1;
			self->mHeight = e->window.data2;
			SDL_RenderPresent( instance->gRenderer );
			break;

			//Repaint on exposure
			case SDL_WINDOWEVENT_EXPOSED:
			SDL_RenderPresent( instance->gRenderer );
			break;

			//Mouse entered window
			case SDL_WINDOWEVENT_ENTER:
			self->mMouseFocus = true;
			updateCaption = true;
			break;
			
			//Mouse left window
			case SDL_WINDOWEVENT_LEAVE:
			self->mMouseFocus = false;
			updateCaption = true;
			break;

			//Window has keyboard focus
			case SDL_WINDOWEVENT_FOCUS_GAINED:
			self->mKeyboardFocus = true;
			updateCaption = true;
			break;

			//Window lost keyboard focus
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
		}

		//Update window caption with new data
		if( updateCaption )
		{
			char caption[50];
            sprintf(caption, "SDL Tutorial - MouseFocus: %s KeyboardFocus: %s", ( (self->mMouseFocus) ? "On" : "Off"), ( ( self->mKeyboardFocus ) ? "On" : "Off"  ) );
			SDL_SetWindowTitle( self->mWindow, caption );
		}
	}
	//Enter exit full screen on return key
	else if( e->type == SDL_KEYDOWN && e->key.keysym.sym == SDLK_RETURN )
	{
		if( self->mFullScreen )
		{
			SDL_SetWindowFullscreen( self->mWindow, 0 );
			self->mFullScreen = false;
		}
		else
		{
			SDL_SetWindowFullscreen( self->mWindow, SDL_WINDOW_FULLSCREEN_DESKTOP );
			self->mFullScreen = true;
			self->mMinimized = false;
		}
	}
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

void window_constructor(lWindow* self)
{
    self->mWindow = NULL;
    self->mMouseFocus = false;
    self->mKeyboardFocus = false;
    self->mFullScreen = false;
    self->mMinimized = false;
    self->mWidth = 0;
    self->mHeight = 0;
    self->handleEvent = &handleEvent;
    self->createRenderer = &createRenderer;
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
		if( !init_window(instance->gWindow) )
		{
			printf( "Window could not be created! SDL Error: %s\n", SDL_GetError() );
			success = false;
		}
		else
		{
			//Create renderer for window
			instance->gRenderer = SDL_CreateRenderer( instance->gWindow->mWindow, -1, SDL_RENDERER_ACCELERATED );
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
			}
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
    lWindow window;
    instance.gWindow = &window;
    window_constructor(instance.gWindow);
    texture_constructor(&instance.gSceneTexture);
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

            while(!quit)
            {
                while(SDL_PollEvent( &e ) != 0)
                {
                    if (e.type == SDL_QUIT)
                    {
                        quit = true;
                    } 
                    instance.gWindow->handleEvent(instance.gWindow, &instance, &e);
                }

				//Only draw when not minimized
				if( !instance.gWindow->mMinimized)
				{
					//Clear screen
					SDL_SetRenderDrawColor( instance.gRenderer, 0xFF, 0xFF, 0xFF, 0xFF );
					SDL_RenderClear( instance.gRenderer );

					//Render text textures
					instance.gSceneTexture->render(instance.gSceneTexture, &instance, ( instance.gWindow->mWidth - instance.gSceneTexture->mWidth ) / 2, ( instance.gWindow->mHeight - instance.gSceneTexture->mHeight ) / 2, NULL, 0, NULL, flipType );

					//Update screen
					SDL_RenderPresent( instance.gRenderer );
				}
            }
        }
    }

    close_sprite(&instance);
    return 0;
}
