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

    //Load prompt texture
    if (!instance->gPromptTexture->loadFromFile(instance->gPromptTexture, instance, "img/prompt.png"))
    {
        printf("Failed to load prompt texture!\n");
        success = false;
    }
    //Load music
    instance->gMusic = Mix_LoadMUS("sound/beat.wav");
    if (instance->gMusic == NULL)
    {
        printf("Failed to load beat music! SDL_mixer Error: %s\n", Mix_GetError());
        success = false;
    }
    //Load sound effects
    instance->gScratch = Mix_LoadWAV("sound/scratch.wav");
    if (instance->gScratch == NULL)
    {
        printf("Failed to load scratch sound effect! SDL_mixer Error: %s\n", Mix_GetError());
        success = false;
    }

    instance->gHigh = Mix_LoadWAV("sound/high.wav");
    if (instance->gHigh == NULL)
    {
        printf("Failed to load high sound effect! SDL_mixer Error: %s\n", Mix_GetError());
        success = false;
    }

    instance->gMedium = Mix_LoadWAV("sound/medium.wav");
    if (instance->gMedium == NULL)
    {
        printf("Failed to load medium sound effect! SDL_mixer Error: %s\n", Mix_GetError());
        success = false;
    }

    instance->gLow = Mix_LoadWAV("sound/low.wav");
    if (instance->gLow == NULL)
    {
        printf("Failed to load low sound effect! SDL_mixer Error: %s\n", Mix_GetError());
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

void close_instance(CL_Instance* instance)
{
    Mix_FreeChunk(instance->gScratch);
    Mix_FreeChunk(instance->gHigh);
    Mix_FreeChunk(instance->gMedium);
    Mix_FreeChunk(instance->gLow);
    instance->gScratch = NULL;
    instance->gHigh = NULL;
    instance->gMedium = NULL;
    instance->gLow = NULL;

    Mix_FreeMusic(instance->gMusic);
    instance->gMusic = NULL;

    SDL_DestroyRenderer(instance->gRenderer);
    SDL_DestroyWindow(instance->gWindow);
    instance->gWindow = NULL;
    instance->gRenderer = NULL;
    //quit sdl subsystems
    Mix_Quit();
    IMG_Quit();
    SDL_Quit();
}

bool init(CL_Instance* instance)
{
	//Initialization flag
	bool success = true;

	//Initialize SDL
	if( SDL_Init( SDL_INIT_VIDEO | SDL_INIT_AUDIO ) < 0 )
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
			//Create vsynced renderer for window
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

				 //Initialize SDL_mixer
				if( Mix_OpenAudio( 44100, MIX_DEFAULT_FORMAT, 2, 2048 ) < 0 )
				{
					printf( "SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError() );
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

    instance.gPromptTexture = &texture1;
    instance.gPromptTexture->loadFromFile = &loadFromFile;
    instance.gPromptTexture->render = &render;

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
					//Handle key press
					else if( e.type == SDL_KEYDOWN )
					{
						switch( e.key.keysym.sym )
						{
							//Play high sound effect
							case SDLK_1:
							Mix_PlayChannel( -1, instance.gHigh, 0 );
							break;
							
							//Play medium sound effect
							case SDLK_2:
							Mix_PlayChannel( -1, instance.gMedium, 0 );
							break;
							
							//Play low sound effect
							case SDLK_3:
							Mix_PlayChannel( -1, instance.gLow, 0 );
							break;
							
							//Play scratch sound effect
							case SDLK_4:
							Mix_PlayChannel( -1, instance.gScratch, 0 );
							break;
							
							case SDLK_9:
							//If there is no music playing
							if( Mix_PlayingMusic() == 0 )
							{
								//Play the music
								Mix_PlayMusic( instance.gMusic, -1 );
							}
							//If music is being played
							else
							{
								//If the music is paused
								if( Mix_PausedMusic() == 1 )
								{
									//Resume the music
									Mix_ResumeMusic();
								}
								//If the music is playing
								else
								{
									//Pause the music
									Mix_PauseMusic();
								}
							}
							break;
							
							case SDLK_0:
							//Stop the music
							Mix_HaltMusic();
							break;
						}
					}
				}

                //Clear screen
                SDL_SetRenderDrawColor( instance.gRenderer, 0xFF, 0xFF, 0xFF, 0xFF );
                SDL_RenderClear( instance.gRenderer );

                //Render splash image
                instance.gPromptTexture->render(instance.gPromptTexture, &instance, 0, 0 , NULL, 0, NULL, flipType );

                //Update screen
                SDL_RenderPresent( instance.gRenderer );
            }
        }
    }
    close_instance(&instance);
    return 0;
}
