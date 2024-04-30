#include "demo.h"

/** A condition for making sure we load SDL_ttf when we need it*/
#if defined(SDL_TTF_MAJOR_VERSION)
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

#endif /*if defined (SDL_TTF_MAJOR_VERSION)*/

bool loadMedia(CL_Instance* instance)
{
	//Loading success flag
	bool success = true;

	//Load press texture
	if( !instance->gSplashTexture->loadFromFile(instance->gSplashTexture, instance ,"img/arrow.png" ) )
	{
		printf( "Failed to load press texture!\n" );
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
    if (instance->gGameController != NULL)
        SDL_GameControllerClose(instance->gGameController);
    if (instance->gJoyHaptic != NULL)
        SDL_HapticClose(instance->gJoyHaptic);
    if (instance->gJoystick != NULL)
        SDL_JoystickClose( instance->gJoystick );
    instance->gGameController = NULL;
    instance->gJoystick = NULL;
    instance->gJoyHaptic = NULL;
    instance->gWindow = NULL;
    instance->gRenderer = NULL;
    SDL_DestroyRenderer(instance->gRenderer);
    SDL_DestroyWindow(instance->gWindow);
    IMG_Quit();
    SDL_Quit();
}

bool init(CL_Instance* instance)
{
	//Initialization flag
	bool success = true;

	//Initialize SDL
	if( SDL_Init( SDL_INIT_VIDEO | SDL_INIT_JOYSTICK | SDL_INIT_HAPTIC | SDL_INIT_GAMECONTROLLER ) < 0 )
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
        // check for joysticks
        if (SDL_NumJoysticks() < 1)
        {
            printf("Warning: No joysticks connected!\n");
        }
        else
        {
            //Check if first joystick is game controller interface compatible
            if (!SDL_IsGameController( 0 ))
            {
                printf("Warning: Joystick is not game controller interface compatible! SDL Error: %s\n", SDL_GetError() );
            } else
            {
                //open game controller and check if it supports rumble
                instance->gGameController = SDL_GameControllerOpen( 0 );
                if (!SDL_GameControllerHasRumble( instance->gGameController ))
                {
                    printf("Waring: Game controller does not have rumble! SDL Error: %s\n", SDL_GetError() );
                }
            }
            //Load joystick if game controller could not be loaded
            if (instance->gGameController == NULL)
            {
                //Open first joystick
                instance->gJoystick = SDL_JoystickOpen( 0 );
                if (instance->gJoystick == NULL)
                {
                    printf("Warning: Unable to open joystick! SDL Error: %s\n", SDL_GetError() );
                } else
                {
                    //Check if joystick supports haptic
                    if (!SDL_JoystickIsHaptic( instance->gJoystick ))
                    { 
                        printf("Warning: Controller does not support haptics! SDL Error: %s\n", SDL_GetError() );
                    }
                    else
                    {
                        //Get joystick haptic device
                        instance->gJoyHaptic = SDL_HapticOpenFromJoystick( instance->gJoystick );
                        if (instance->gJoyHaptic == NULL)
                        {
                            printf("Warning: Unable to get joystick haptics! SDL Error: %s\n", SDL_GetError() );
                        } else
                        {
                            //Initialize rumble
                            if (SDL_HapticRumbleInit( instance->gJoyHaptic ) < 0)
                            {
                                printf("Warning: Unable to initialize haptic rumble! SDL Error: %s\n", SDL_GetError() );
                            }
                        }
                    }
                }
            }
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
			instance->gRenderer = SDL_CreateRenderer( instance->gWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC );
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

void setPosition(lButton *self , int x, int y)
{
    self->mPosition.x = x;
    self->mPosition.y = y;
}

void handleEvent(SDL_Event* e, lButton* self)
{
	if( e->type == SDL_MOUSEMOTION || e->type == SDL_MOUSEBUTTONDOWN || e->type == SDL_MOUSEBUTTONUP )
    {
        int x, y;
        SDL_GetMouseState(&x,&y);
        bool inside = true;
        //mouse is left of the button
        if (x < self->mPosition.x)
            inside = false;
        //mouse is right of the button
        else if (x > self->mPosition.x + BUTTON_WIDTH)
            inside = false;
        //Mouse above the button
        else if (y < self->mPosition.y)
            inside = false;
        //Mouse below the button
        else if (y > self->mPosition.y + BUTTON_HEIGHT )
            inside = false;

        //Mouse is outside button
        if (!inside)
        {
            self->mCurrentSprite = BUTTON_SPRITE_MOUSE_OUT;
        }
        //Mouse is inside button
        else
        {
            //set mouse over sprite
            switch(e->type)
            {
                case SDL_MOUSEMOTION:
                self->mCurrentSprite =  BUTTON_SPRITE_MOUSE_OVER_MOTION;
                break;
                case SDL_MOUSEBUTTONDOWN:
                self->mCurrentSprite = BUTTON_SPRITE_MOUSE_DOWN;
                break;
                case SDL_MOUSEBUTTONUP:
                self->mCurrentSprite = BUTTON_SPRITE_MOUSE_UP;
                break;
            }
        }
        
    }

}

int main()
{
    CL_Instance instance;
    lTexture_s texture1;

    instance.gSplashTexture = &texture1;
    instance.gSplashTexture->loadFromFile = &loadFromFile;
    instance.gSplashTexture->render = &render;

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
        } 
        else
        {    
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
                    //Joystick button press
                    else if( e.type == SDL_JOYBUTTONDOWN )
                    {
                        //Use game controller
                        if( instance.gGameController != NULL )
                        {
                            //Play rumble at 75% strength for 500 milliseconds
                            if( SDL_GameControllerRumble( instance.gGameController, 0xFFFF * 3 / 4, 0xFFFF * 3 / 4, 500 ) != 0 )
                            {
                                printf( "Warning: Unable to play game contoller rumble! %s\n", SDL_GetError() );
                            }
                        }
                        //Use haptics
                        else if( instance.gJoyHaptic != NULL )
                        {
                            //Play rumble at 75% strength for 500 milliseconds
                            if( SDL_HapticRumblePlay( instance.gJoyHaptic, 0.75, 500 ) != 0 )
                            {
                                printf( "Warning: Unable to play haptic rumble! %s\n", SDL_GetError() );
                            }
                        }
                    }
                }

                //Clear screen
                SDL_SetRenderDrawColor( instance.gRenderer, 0xFF, 0xFF, 0xFF, 0xFF );
                SDL_RenderClear( instance.gRenderer );

                //Render splash image
                instance.gSplashTexture->render(instance.gArrowTexture, &instance, 0, 0 , NULL, 0, NULL, flipType );

                //Update screen
                SDL_RenderPresent( instance.gRenderer );
            }
        }
    }
    close_sprite(&instance);
    return 0;
}
