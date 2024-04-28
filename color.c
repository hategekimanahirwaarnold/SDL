#include "demo.h"

bool loadMedia_Color(CL_Instance* instance)
{
    //load success flag
    bool success = true;

    if (!instance->gFooTexture->loadFromFile(instance->gFooTexture, instance, "img/foo.png"))
    {
        printf("Failed to load Foo' texture image!");
        success = false;
    }
    //Load from background texture
    if (!instance->gBackgroundTexture->loadFromFile(instance->gBackgroundTexture, instance, "img/background.png"))
    {
        printf("Failed to load background texture image!\n");
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

void render (lTexture_s *self, CL_Instance* instance, int x, int y)
{
    SDL_Rect renderQuad = { x, y, self->mWidth, self->mHeight };
    SDL_RenderCopy( instance->gRenderer, self->mTexture, NULL, &renderQuad);
}
void close_color(CL_Instance* instance)
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

int main()
{
    CL_Instance instance;
    lTexture_s texture, texture2;
    instance.gFooTexture = &texture;
    instance.gBackgroundTexture = &texture2;
    instance.gFooTexture->render = render;
    instance.gFooTexture->loadFromFile = loadFromFile;
    instance.gBackgroundTexture->render = render;
    instance.gBackgroundTexture->loadFromFile = loadFromFile;
    bool quit = false;
    if ( !init(&instance) )
    {
        printf("Failed to initialize!\n");
    } else
    {
        // load media
        if (!loadMedia_Color(&instance))
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
                //render background texture to screen
                instance.gBackgroundTexture->render(instance.gBackgroundTexture, &instance , 0, 0);
                //render foo' to the screen
                instance.gFooTexture->render(instance.gFooTexture, &instance, 240, 190);
                // update screen
                SDL_RenderPresent(instance.gRenderer);
            }
        }
    }
    close_color(&instance);
}
