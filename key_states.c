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
	if( !instance->gPressTexture->loadFromFile(instance->gPressTexture, instance ,"img/press.png" ) )
	{
		printf( "Failed to load press texture!\n" );
		success = false;
	}
	
	//Load up texture
	if( !instance->gUpTexture->loadFromFile(instance->gUpTexture, instance, "img/up.png" ) )
	{
		printf( "Failed to load up texture!\n" );
		success = false;
	}

	//Load down texture
	if( !instance->gDownTexture->loadFromFile(instance->gDownTexture, instance, "img/down.png" ) )
	{
		printf( "Failed to load down texture!\n" );
		success = false;
	}

	//Load left texture
	if( !instance->gLeftTexture->loadFromFile(instance->gLeftTexture, instance, "img/left.png" ) )
	{
		printf( "Failed to load left texture!\n" );
		success = false;
	}

	//Load right texture
	if( !instance->gRightTexture->loadFromFile(instance->gRightTexture, instance, "img/right.png" ) )
	{
		printf( "Failed to load right texture!\n" );
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
    printf(" we are in render!\n");
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
    lTexture_s texture1, texture2, texture3, texture4, texture5;

    instance.gPressTexture = &texture1;
    instance.gPressTexture->loadFromFile = &loadFromFile;
    instance.gPressTexture->render = &render;

    instance.gUpTexture = &texture2;
    instance.gUpTexture->loadFromFile = &loadFromFile;
    instance.gUpTexture->render = &render;

    instance.gDownTexture = &texture3;
    instance.gDownTexture->loadFromFile = &loadFromFile;
    instance.gDownTexture->render = &render;

    instance.gLeftTexture = &texture4;
    instance.gLeftTexture->loadFromFile = &loadFromFile;
    instance.gLeftTexture->render = &render;

    instance.gRightTexture = &texture5;
    instance.gRightTexture->loadFromFile = &loadFromFile;
    instance.gRightTexture->render = &render;

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
            lTexture_s* currentTexture = NULL;
            while(!quit)
            {
                while(SDL_PollEvent( &e ) != 0)
                {
                    if (e.type == SDL_QUIT)
                    {
                        quit = true;
                    }
                }
				//Set texture based on current keystate
				const Uint8* currentKeyStates = SDL_GetKeyboardState( NULL );
                printf("code: %d\n", currentKeyStates[ SDL_SCANCODE_UP ]);
                printf(" we are here ! \n ");
				if( currentKeyStates[ SDL_SCANCODE_UP ] )
				{
					currentTexture = instance.gUpTexture;
				}
				else if( currentKeyStates[ SDL_SCANCODE_DOWN ] )
				{
					currentTexture = instance.gDownTexture;
				}
				else if( currentKeyStates[ SDL_SCANCODE_LEFT ] )
				{
					currentTexture = instance.gLeftTexture;
				}
				else if( currentKeyStates[ SDL_SCANCODE_RIGHT ] )
				{
					currentTexture = instance.gRightTexture;
				}
				else
				{
					currentTexture = instance.gPressTexture;
				}
                //Clear screen
                SDL_SetRenderDrawColor( instance.gRenderer, 0xFF, 0xFF, 0xFF, 0xFF );
                SDL_RenderClear( instance.gRenderer );

                printf("we are in else!\n");
				//Render current texture
				currentTexture->render(currentTexture, &instance, 0, 0, NULL, 0, NULL, flipType );
                //Update screen
                SDL_RenderPresent( instance.gRenderer );
            }
        }
    }
    close_sprite(&instance);
    return 0;
}
