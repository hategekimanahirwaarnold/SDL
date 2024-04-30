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
	bool success = true;

	//Load sprites
	if( !instance->gButtonSpriteSheetTexture->loadFromFile(instance->gButtonSpriteSheetTexture, instance, "img/button.png" ) )
	{
		printf( "Failed to load button sprite texture!\n" );
		success = false;
	}
	else
	{
		//Set sprites
		for( int i = 0; i < BUTTON_SPRITE_TOTAL; ++i )
		{
			instance->gSpriteClips[ i ].x = 0;
			instance->gSpriteClips[ i ].y = i * 200;
			instance->gSpriteClips[ i ].w = BUTTON_WIDTH;
			instance->gSpriteClips[ i ].h = BUTTON_HEIGHT;
		}

		//Set buttons in corners
		instance->gButtons[ 0 ].setPosition(&instance->gButtons[0], 0, 0 );
		instance->gButtons[ 1 ].setPosition(&instance->gButtons[1], SCREEN_WIDTH - BUTTON_WIDTH, 0 );
		instance->gButtons[ 2 ].setPosition(&instance->gButtons[2], 0, SCREEN_HEIGHT - BUTTON_HEIGHT );
		instance->gButtons[ 3 ].setPosition(&instance->gButtons[3], SCREEN_WIDTH - BUTTON_WIDTH, SCREEN_HEIGHT - BUTTON_HEIGHT );
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

void renderButton(lButton* self, CL_Instance* instance)
{
    instance->gButtonSpriteSheetTexture->render(instance->gButtonSpriteSheetTexture, instance, self->mPosition.x, self->mPosition.y, &instance->gSpriteClips[ self->mCurrentSprite ], 0, NULL, SDL_FLIP_NONE);
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
    lTexture_s texture;
    instance.gButtonSpriteSheetTexture = &texture;
    instance.gButtonSpriteSheetTexture->loadFromFile = &loadFromFile;
    instance.gButtonSpriteSheetTexture->render = &render;
    instance.gButtonSpriteSheetTexture->setAlpha = &setAlpha;
    instance.gButtonSpriteSheetTexture->setBlendMode = &setBlendMode;

    for( int i = 0; i < TOTAL_BUTTONS; ++i )
    {
        instance.gButtons[ i ].setPosition = &setPosition;
        instance.gButtons[ i ].handleEvent = &handleEvent;
        instance.gButtons[ i ].render = &renderButton;
        instance.gButtons[ i ].mPosition.x = 0;
        instance.gButtons[ i ].mPosition.y = 0;
        instance.gButtons[ i ].mCurrentSprite = BUTTON_SPRITE_MOUSE_OUT;
    }

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
                    // Handle button events
                    for (int i = 0; i < TOTAL_BUTTONS; ++i)
                    {
                        instance.gButtons[ i ].handleEvent( &e, &instance.gButtons[ i ] );
                    } 
                }

                //Clear screen
                SDL_SetRenderDrawColor( instance.gRenderer, 0xFF, 0xFF, 0xFF, 0xFF );
                SDL_RenderClear( instance.gRenderer );

                //Render buttons
                for( int i = 0; i < TOTAL_BUTTONS; ++i )
                {
                    instance.gButtons[ i ].render(&instance.gButtons[i], &instance);
                }
                //Update screen
                SDL_RenderPresent( instance.gRenderer );
            }
        }
    }
    close_sprite(&instance);
    return 0;
}
