#include "demo.h"

bool loadMedia(CL_Instance* instance)
{
    bool success = true;

    //load front alpha texture
    if (!instance->gSpriteSheetTexture->loadFromFile(instance->gSpriteSheetTexture, instance, "img/foo2.png"))
    {
        printf("Failed to load texture image!\n");
        success = false;
    } else
    {
        //sprite clips
        instance->gSpriteClips[ 0 ].x = 0;
        instance->gSpriteClips[ 0 ].y = 0;
        instance->gSpriteClips[ 0 ].w = 64;
        instance->gSpriteClips[ 0 ].h = 205;

        instance->gSpriteClips[ 1 ].x = 64;
        instance->gSpriteClips[ 1 ].y = 0;
        instance->gSpriteClips[ 1 ].w = 64;
        instance->gSpriteClips[ 1 ].h = 205;

        instance->gSpriteClips[ 2 ].x = 128;
        instance->gSpriteClips[ 2 ].y = 0;
        instance->gSpriteClips[ 2 ].w = 64;
        instance->gSpriteClips[ 2 ].h = 205;
    
        instance->gSpriteClips[ 3 ].x = 192;
        instance->gSpriteClips[ 3 ].y = 0;
        instance->gSpriteClips[ 3 ].w = 64;
        instance->gSpriteClips[ 3 ].h = 205;
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

void render (lTexture_s *self, CL_Instance* instance, int x, int y, SDL_Rect* clip)
{
    SDL_Rect renderQuad = { x, y, self->mWidth, self->mHeight };

    //Set clip rendering dimensions
    if ( clip != NULL )
    {
        renderQuad.w = clip->w;
        renderQuad.h = clip->h;
    }

    SDL_RenderCopy( instance->gRenderer, self->mTexture, clip, &renderQuad);
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

int main()
{
    CL_Instance instance;
    lTexture_s texture;
    instance.gSpriteSheetTexture = &texture;
    instance.gSpriteSheetTexture->loadFromFile = &loadFromFile;
    instance.gSpriteSheetTexture->render = &render;
    instance.gSpriteSheetTexture->setAlpha = &setAlpha;
    instance.gSpriteSheetTexture->setBlendMode = &setBlendMode;

    bool quit = false;
    Uint8 a = 255;
    int frame = 0;
    int timer = 40;

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
            SDL_Event e;

            while(!quit)
            {
                while(SDL_PollEvent( &e ) != 0)
                {
                    if (e.type == SDL_QUIT)
                    {
                        quit = true;
                    }
                }

                // clear screen
                SDL_SetRenderDrawColor(instance.gRenderer, 0xFF, 0xFF, 0xFF, 0xFF );
                SDL_RenderClear(instance.gRenderer);

                SDL_Rect* currentClip = &instance.gSpriteClips[ frame / timer ];
                instance.gSpriteSheetTexture->render(instance.gSpriteSheetTexture, &instance, ( SCREEN_WIDTH - currentClip->w ) / 2, ( SCREEN_HEIGHT - currentClip->h ) / 2, currentClip );
                //Update screen
                SDL_RenderPresent(instance.gRenderer);
                //next frame
                ++frame;
                // cycle animation
                if ( frame / timer >= WALKING_ANIMATION_FRAMES )
                    frame = 0;
            }
        }
    }
    close_sprite(&instance);
    return 0;
}
