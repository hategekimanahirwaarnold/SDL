#include "demo.h"

bool loadMedia_Modulation(CL_Instance* instance)
{
    //load success flag
    bool success = true;

    if (!instance->gModulatedTexture->loadFromFile(instance->gModulatedTexture, instance, "img/colors.png"))
    {
        printf("Failed to load sprite texture image!");
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
        self->mTexture = newTexture;
    }
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

void setColor(SDL_Texture *mTexture, Uint8 red, Uint8 green, Uint8 blue)
{
    SDL_SetTextureColorMod(mTexture, red, green, blue);
}

int main()
{
    CL_Instance instance;
    lTexture_s texture;
    instance.gModulatedTexture = &texture;
    instance.gModulatedTexture->setColor = &setColor;
    instance.gModulatedTexture->loadFromFile = &loadFromFile;
    instance.gModulatedTexture->render = &render;
    bool quit = false;
    Uint8 r = 255;
    Uint8 g = 255;
    Uint8 b = 255;

    if ( !init(&instance) )
    {
        printf("Failed to initialize!\n");
    } else
    {
        // load media
        if (!loadMedia_Modulation(&instance))
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
                    } else if (e.type == SDL_KEYDOWN)
                    {
                        switch(e.key.keysym.sym)
                        {
                            //increase red
                            case SDLK_q:
                            r += 32;
                            break;
                            //increase green
                            case SDLK_w:
                            g += 32;
                            break;
                            //increase blue
                            case SDLK_e:
                            b += 32;
                            break;
                            //Decrease red
                            case SDLK_a:
                            r -= 32;
                            break;
                            //Decrease green
                            case SDLK_s:
                            g -= 32;
                            break;
                            //Decrease blue
                            case SDLK_d:
                            b -= 32;
                            break;
                        }
                    }
                }
                // clear screen
                SDL_SetRenderDrawColor(instance.gRenderer, 0xFF, 0xFF, 0xFF, 0xFF );
                SDL_RenderClear(instance.gRenderer);
                // modulate render texture
                instance.gModulatedTexture->setColor(instance.gModulatedTexture->mTexture, r, g, b);
                instance.gModulatedTexture->render(instance.gModulatedTexture, &instance, 0 , 0, NULL);

                // update screen
                SDL_RenderPresent(instance.gRenderer);
            }
        }
    }
    close_sprite(&instance);
}
