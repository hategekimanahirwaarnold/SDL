#include "demo.h"

// file loading
void free_texture(lTexture_s* self)
{
	//Free texture if it exists
	if( self->mTexture != NULL )
	{
		SDL_DestroyTexture( self->mTexture );
		self->mTexture = NULL;
		self->mWidth = 0;
		self->mHeight = 0;
	}

	//Free surface if it exists
	if( self->mSurfacePixels != NULL )
	{
		SDL_FreeSurface( self->mSurfacePixels );
		self->mSurfacePixels = NULL;
	}
}

bool loadPixelsFromFile(lTexture_s *self, CL_Instance *instance, char *path)
{
    // Load image at specified path
    free_texture(self);
    SDL_Surface *loadedSurface = IMG_Load(path);
    if (loadedSurface == NULL)
    {
        printf("Unable to load image %s! SDL_image Error: %s\n", path, IMG_GetError());
    }
    else
    {
        // Convert surface to display format
        self->mSurfacePixels = SDL_ConvertSurfaceFormat(loadedSurface, SDL_GetWindowPixelFormat(instance->gWindow), 0);
        if (self->mSurfacePixels == NULL)
        {
            printf("Unable to convert loaded surface to display format! SDL Error: %s\n", SDL_GetError());
        }
        else
        {
            // Get image dimensions
            self->mWidth = self->mSurfacePixels->w;
            self->mHeight = self->mSurfacePixels->h;
        }
        // Get rid of old loaded surface
        SDL_FreeSurface(loadedSurface);
    }

    return self->mSurfacePixels != NULL;
}

bool loadFromPixels(lTexture_s *self, CL_Instance *instance)
{
    // Only load if pixels exist
    if (self->mSurfacePixels == NULL)
    {
        printf("No pixels loaded!");
    }
    else
    {
        // Color key image
        SDL_SetColorKey(self->mSurfacePixels, SDL_TRUE, SDL_MapRGB(self->mSurfacePixels->format, 0, 0xFF, 0xFF));

        // Create texture from surface pixels
        self->mTexture = SDL_CreateTextureFromSurface(instance->gRenderer, self->mSurfacePixels);
        if (self->mTexture == NULL)
        {
            printf("Unable to create texture from loaded pixels! SDL Error: %s\n", SDL_GetError());
        }
        else
        {
            // Get image dimensions
            self->mWidth = self->mSurfacePixels->w;
            self->mHeight = self->mSurfacePixels->h;
        }

        // Get rid of old loaded surface
        SDL_FreeSurface(self->mSurfacePixels);
        self->mSurfacePixels = NULL;
    }

    // Return success
    return self->mTexture != NULL;
}

bool loadFromFile(lTexture_s *self, CL_Instance *instance, char *path)
{
    // Load pixels
    if (!loadPixelsFromFile(self, instance, path))
    {
        printf("Failed to load pixels for %s!\n", path);
    }
    else
    {
        // Load texture from pixels
        if (!loadFromPixels(self, instance))
        {
            printf("Failed to texture from pixels from %s!\n", path);
        }
    }
    printf("we reached load from file\n");
    // Return success
    return self->mTexture != NULL;
}

void render(lTexture_s *self, CL_Instance *instance, int x, int y, SDL_Rect *clip, double angle, SDL_Point *center, SDL_RendererFlip flip)
{
    SDL_Rect renderQuad = {x, y, self->mWidth, self->mHeight};

    // Set clip rendering dimensions
    if (clip != NULL)
    {
        renderQuad.w = clip->w;
        renderQuad.h = clip->h;
    }

    SDL_RenderCopyEx(instance->gRenderer, self->mTexture, clip, &renderQuad, angle, center, flip);
}

void render_window(lWindow *self)
{
    if (!self->mMinimized)
    {
        // Clear screen
        SDL_SetRenderDrawColor(self->mRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
        SDL_RenderClear(self->mRenderer);

        // Update screen
        SDL_RenderPresent(self->mRenderer);
    }
}

void free_dot(Dot_s *self)
{
    for (int i = 0; i < TOTAL_PARTICLES; i++)
    {
        free(self->particles[i]);
    }
};

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

// get data
Uint32 *getPixels32(lTexture_s *self)
{
    Uint32 *pixels = NULL;

    if (self->mSurfacePixels != NULL)
    {
        pixels = (Uint32 *)(self->mSurfacePixels->pixels);
    }

    return pixels;
}

Uint32 getPitch32(lTexture_s *self)
{
    Uint32 pitch = 0;

    if (self->mSurfacePixels != NULL)
    {
        pitch = self->mSurfacePixels->pitch / 4;
    }

    return pitch;
}

Uint32 mapRGBA(lTexture_s *self, Uint8 r, Uint8 g, Uint8 b, Uint8 a)
{
    Uint32 pixel = 0;

    if (self->mSurfacePixels != NULL)
    {
        // printf("surface not null\n");
        pixel = SDL_MapRGBA(self->mSurfacePixels->format, r, g, b, a);
    }

    return pixel;
}

// closing
void close_instance(CL_Instance *instance)
{
    free_texture(instance->gFooTexture);
    // Destroy window
    SDL_DestroyRenderer(instance->gRenderer);
    SDL_DestroyWindow(instance->gWindow);
    instance->gWindow = NULL;
    instance->gRenderer = NULL;

    // Quit SDL subsystems
    IMG_Quit();
    SDL_Quit();
}

void setBlendMode(SDL_Texture *mTexture, SDL_BlendMode blending)
{
    // Set blending function
    SDL_SetTextureBlendMode(mTexture, blending);
}

void setAlpha(SDL_Texture *mTexture, Uint8 alpha)
{
    // Modulated texture alpha
    SDL_SetTextureAlphaMod(mTexture, alpha);
}

bool loadFromRenderedText(lTexture_s *self, CL_Instance *instance, char *textureText, SDL_Color textColor)
{
    SDL_Surface *textSurface = TTF_RenderText_Solid(instance->gFont, textureText, textColor);
    if (textSurface == NULL)
    {
        printf("Unable to render text sufrace! SDL_ttf Error: %s", TTF_GetError());
    }
    else
    {
        self->mTexture = SDL_CreateTextureFromSurface(instance->gRenderer, textSurface);
        if (self->mTexture == NULL)
        {
            printf("Unable to create texture from rendered text! SDL Error: %s\n", SDL_GetError());
        }
        else
        {
            self->mWidth = textSurface->w;
            self->mHeight = textSurface->h;
        }
        // remove old surface
        SDL_FreeSurface(textSurface);
    }
    return self->mTexture != NULL;
}

// constructors
void texture_constructor(lTexture_s **self)
{
    *self = (lTexture_s *)malloc(sizeof(lTexture_s));
    if (*self == NULL)
    {
        // Handle memory allocation failure
        printf("Failed to allocate memory for new texture!\n");
        return;
    }
    (*self)->loadFromFile = &loadFromFile;
    (*self)->render = &render;
    (*self)->setAlpha = &setAlpha;
    (*self)->setBlendMode = &setBlendMode;
    (*self)->loadFromRenderedText = &loadFromRenderedText;
    (*self)->getPixels32 = &getPixels32;
    (*self)->getPitch32 = &getPitch32;
    (*self)->mapRGBA = &mapRGBA;
    (*self)->loadPixelsFromFile = &loadPixelsFromFile;
    (*self)->loadFromPixels = &loadFromPixels;
}

bool init(CL_Instance *instance)
{
    // Initialization flag
    bool success = true;

    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        printf("SDL could not initialize! SDL Error: %s\n", SDL_GetError());
        success = false;
    }
    else
    {
        // Set texture filtering to linear
        if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1"))
        {
            printf("Warning: Linear texture filtering not enabled!");
        }

        // Create window
        instance->gWindow = SDL_CreateWindow("SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
        if (instance->gWindow == NULL)
        {
            printf("Window could not be created! SDL Error: %s\n", SDL_GetError());
            success = false;
        }
        else
        {
            // Create renderer for window
            instance->gRenderer = SDL_CreateRenderer(instance->gWindow, -1, SDL_RENDERER_ACCELERATED);
            if (instance->gRenderer == NULL)
            {
                printf("Renderer could not be created! SDL Error: %s\n", SDL_GetError());
                success = false;
            }
            else
            {
                // Initialize renderer color
                SDL_SetRenderDrawColor(instance->gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);

                // Initialize PNG loading
                int imgFlags = IMG_INIT_PNG;
                if (!(IMG_Init(imgFlags) & imgFlags))
                {
                    printf("SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError());
                    success = false;
                }
            }
        }
    }

    return success;
}

bool loadMedia(CL_Instance *instance)
{
    // Loading success flag
    bool success = true;

    // Load Foo' texture pixel
    if (!instance->gFooTexture->loadPixelsFromFile(instance->gFooTexture, instance, "img/foo_3.png"))
    {
        printf("Unable to load Foo' texture!\n");
        success = false;
    }
    else
    {
        // printf("after load pixels!\n");
        // Get pixel data
        Uint32 *pixels = instance->gFooTexture->getPixels32(instance->gFooTexture);
        int pixelCount = instance->gFooTexture->getPitch32(instance->gFooTexture) * instance->gFooTexture->mHeight;

        // Map colors
        Uint32 colorKey = instance->gFooTexture->mapRGBA(instance->gFooTexture, 0xFF, 0x00, 0xFF, 0xFF);
        Uint32 transparent = instance->gFooTexture->mapRGBA(instance->gFooTexture, 0xFF, 0xFF, 0xFF, 0x00);

        // Color key pixels
        for (int i = 0; i < pixelCount; ++i)
        {
            if (pixels[i] == colorKey)
            {
                pixels[i] = transparent;
            }
        }

        // Create texture from manually color keyed pixels
        if (!instance->gFooTexture->loadFromPixels(instance->gFooTexture, instance))
        {
            printf("Unable to load Foo' texture from surface!\n");
        }
    }

    return success;
}

int main()
{
    CL_Instance instance;
    texture_constructor(&instance.gFooTexture);
    if (!init(&instance))
    {
        printf("Failed to initialize!\n");
    }
    else
    {
        // Load media
        if (!loadMedia(&instance))
        {
            printf("Failed to load media!\n");
        } else
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
				}

				//Clear screen
				SDL_SetRenderDrawColor( instance.gRenderer, 0xFF, 0xFF, 0xFF, 0xFF );
				SDL_RenderClear( instance.gRenderer );

				//Render stick figure
				instance.gFooTexture->render(instance.gFooTexture, &instance, ( SCREEN_WIDTH - instance.gFooTexture->mWidth ) / 2, ( SCREEN_HEIGHT - instance.gFooTexture->mHeight ) / 2, NULL, 0, NULL, SDL_FLIP_NONE );

				//Update screen
				SDL_RenderPresent( instance.gRenderer );
			}
		}
	
    }

    close_instance(&instance);
    return 0;
}
