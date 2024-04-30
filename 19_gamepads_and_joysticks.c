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
	if( !instance->gArrowTexture->loadFromFile(instance->gArrowTexture, instance ,"img/arrow.png" ) )
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
    SDL_DestroyRenderer(instance->gRenderer);
    SDL_DestroyWindow(instance->gWindow);
    SDL_JoystickClose(instance->gGameController);
    instance->gGameController = NULL;
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
	if( SDL_Init( SDL_INIT_VIDEO | SDL_INIT_JOYSTICK ) < 0 )
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
            //Load joystick
            instance->gGameController = SDL_JoystickOpen( 0 );
            if (instance->gGameController == NULL)
            {
                printf("Warning: Unable to open game controller! SDL Error: %s\n", SDL_GetError() );
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

    instance.gArrowTexture = &texture1;
    instance.gArrowTexture->loadFromFile = &loadFromFile;
    instance.gArrowTexture->render = &render;

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
            int xDir = 0;
            int yDir = 0;
            SDL_Event e;
            lTexture_s* currentTexture = NULL;
            while(!quit)
            {
                while(SDL_PollEvent( &e ) != 0)
                {
                    if (e.type == SDL_QUIT)
                    {
                        quit = true;
                    } else if (e.type == SDL_JOYAXISMOTION)
                    {
                        //Motion on controller 0
                        if (e.jaxis.which == 0)
                        {
                            //X axis motion
                            if (e.jaxis.axis == 0)
                            {
                                //Left of dead zone
                                if (e.jaxis.value < -JOYSTICK_DEAD_ZONE )
                                {
                                    xDir = -1;
                                }
                                //Right of dead zone
                                else if (e.jaxis.value > JOYSTICK_DEAD_ZONE)
                                {
                                    xDir = 1;
                                }
                                else
                                {
                                    xDir = 0;
                                }
                            }
                        } else if (e.jaxis.axis == 1)
                        {
                            //Below of dead zone
                            if (e.jaxis.value < -JOYSTICK_DEAD_ZONE)
                            {
                                yDir = -1;
                            }
                            //Above of dead zone
                            else if (e.jaxis.value > JOYSTICK_DEAD_ZONE)
                            {
                                yDir = 1;
                            }
                            else
                            {
                                yDir = 0;
                            }
                        }
                    }
                }
                //Clear screen
                SDL_SetRenderDrawColor( instance.gRenderer, 0xFF, 0xFF, 0xFF, 0xFF );
                SDL_RenderClear( instance.gRenderer );

                //calculate angle
                double joystickAngle = atan2( (double)yDir, (double)xDir * (180.0 / M_PI ) );

                //Correct angle
                if (xDir == 0 && yDir == 0)
                {
                    joystickAngle = 0;
                }
				//Render joystick 8 way angle
                instance.gArrowTexture->render(instance.gArrowTexture, &instance, ( SCREEN_WIDTH - instance.gArrowTexture->mWidth ) / 2, ( SCREEN_HEIGHT - instance.gArrowTexture->mHeight ) / 2, NULL, joystickAngle, NULL, flipType );
                //Update screen
                SDL_RenderPresent( instance.gRenderer );
            }
        }
    }
    close_sprite(&instance);
    return 0;
}
