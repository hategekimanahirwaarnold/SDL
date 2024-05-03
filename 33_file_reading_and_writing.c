#include "demo.h"

bool loadMedia(CL_Instance* instance)
{
    bool success = true;
    SDL_Color textColor = { 0, 0, 0, 0xFF };
	SDL_Color highlightColor = { 0xFF, 0, 0, 0xFF };

    instance->gFont = TTF_OpenFont("img/lazy.ttf", 28);
    if (instance->gFont == NULL)
    {
        printf("Failed to load lazy font! SDL_ttf Error: %s\n", TTF_GetError() );
        success = false;
    } else
    {
        //Render text
        if (!instance->gPromptTexture->loadFromRenderedText(instance->gPromptTexture, instance, "Enter data: ", textColor) )
        {
            printf("Failed to render prompt texture!\n");
            success = false;
        }
    }

	//Open file for reading in binary
	SDL_RWops* file = SDL_RWFromFile( "files/nums.bin", "r+b" );

	//File does not exist
	if( file == NULL )
	{
		printf( "Warning: Unable to open file! SDL Error: %s\n", SDL_GetError() );
		
		//Create file for writing
		file = SDL_RWFromFile( "files/nums.bin", "w+b" );
		if( file != NULL )
		{
			printf( "New file created!\n" );

			//Initialize data
			for( int i = 0; i < TOTAL_DATA; ++i )
			{
				instance->gData[ i ] = 0;	
				SDL_RWwrite( file, &instance->gData[ i ], sizeof(Sint32), 1 );
			}
			
			//Close file handler
			SDL_RWclose( file );
		}
		else
		{
			printf( "Error: Unable to create file! SDL Error: %s\n", SDL_GetError() );
			success = false;
		}
	}
	//File exists
	else
	{
		//Load data
		printf( "Reading file...!\n" );
		for( int i = 0; i < TOTAL_DATA; ++i )
		{
            SDL_RWread(file, &instance->gData[i], sizeof(Sint32), 1);
        }

        // colose file handler
        SDL_RWclose(file);
    }
    char data[20];
    sprintf(data, "%d", instance->gData[ 0 ]);
    // Initializse data textures
    instance->gDataTexture[ 0 ]->loadFromRenderedText(instance->gDataTexture[0], instance, &data[0], highlightColor );
    for ( int i = 1; i < TOTAL_DATA; ++i )
    {
        char data2[20];
        sprintf(data2, "%d", instance->gData[i]);
        instance->gDataTexture[ 0 ]->loadFromRenderedText(instance->gDataTexture[ 1 ], instance, data2, textColor );
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

void free_texture(lTexture_s* self)
{
    if (self->mTexture != NULL)
    {
        SDL_DestroyTexture(self->mTexture);
        self->mTexture = NULL;
        self->mWidth = 0;
        self->mHeight = 0;
    }
}

void close_sprite(CL_Instance* instance)
{
	//Open data for writing
	SDL_RWops* file = SDL_RWFromFile( "files/nums.bin", "w+b" );
	if( file != NULL )
	{
		//Save data
		for( int i = 0; i < TOTAL_DATA; ++i )
		{
			SDL_RWwrite( file, &instance->gData[ i ], sizeof(Sint32), 1 );
		}

		//Close file handler
		SDL_RWclose( file );
	}
	else
	{
		printf( "Error: Unable to save file! %s\n", SDL_GetError() );
	}

	//Free loaded images
	free_texture(instance->gPromptTexture);
	for( int i = 0; i < TOTAL_DATA; ++i )
	{
		free_texture(instance->gDataTexture[ i ]);
	}

    SDL_DestroyRenderer(instance->gRenderer);
    SDL_DestroyWindow(instance->gWindow);
    instance->gWindow = NULL;
    instance->gRenderer = NULL;
    TTF_CloseFont(instance->gFont);
    instance->gFont = NULL;
    TTF_Quit();
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
                //Initialize SDL_ttf
                if (TTF_Init() == -1)
                {
                    printf("SDL_ttf could not initialize! SDL_ttf Error: %s\n", TTF_GetError() );
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
    (*self)->mWidth = 0;
    (*self)->mHeight = 0;
    (*self)->mTexture = NULL;
}

int main()
{
    CL_Instance instance;
    for (int i = 0; i < TOTAL_DATA; i++)
        texture_constructor(&instance.gDataTexture[i]);

    texture_constructor(&instance.gPromptTexture);

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
            SDL_Color textColor = {0, 0, 0, 0XFF};
            SDL_Color highlightColor = { 0xFF, 0, 0, 0xFF };
            int currentData = 0;
            char data[20];
        
            // Enable text input
            SDL_StartTextInput();
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
					else if( e.type == SDL_KEYDOWN )
					{
                        sprintf(data, "%d", instance.gData[ currentData ]);
						switch( e.key.keysym.sym )
						{
							//Previous data entry
							case SDLK_UP:
							//Rerender previous entry input point
							instance.gDataTexture[ currentData ]->loadFromRenderedText( instance.gDataTexture[ currentData ], &instance, data, textColor );
							--currentData;
							if( currentData < 0 )
							{
								currentData = TOTAL_DATA - 1;
							}
							
							//Rerender current entry input point
							instance.gDataTexture[ currentData ]->loadFromRenderedText( instance.gDataTexture[ currentData ], &instance, data, highlightColor );
							break;
							
							//Next data entry
							case SDLK_DOWN:
							//Rerender previous entry input point
							instance.gDataTexture[ currentData ]->loadFromRenderedText( instance.gDataTexture[ currentData ], &instance, data, textColor );
							++currentData;
							if( currentData == TOTAL_DATA )
							{
								currentData = 0;
							}
							
							//Rerender current entry input point
							instance.gDataTexture[ currentData ]->loadFromRenderedText( instance.gDataTexture[ currentData ], &instance, data, highlightColor );
							break;

							//Decrement input point
							case SDLK_LEFT:
							--instance.gData[ currentData ];
							instance.gDataTexture[ currentData ]->loadFromRenderedText( instance.gDataTexture[ currentData ], &instance, data, highlightColor );
							break;
							
							//Increment input point
							case SDLK_RIGHT:
							++instance.gData[ currentData ];
							instance.gDataTexture[ currentData ]->loadFromRenderedText( instance.gDataTexture[ currentData ], &instance, data, highlightColor );
							break;
						}
					}
				}

				//Clear screen
				SDL_SetRenderDrawColor( instance.gRenderer, 0xFF, 0xFF, 0xFF, 0xFF );
				SDL_RenderClear( instance.gRenderer );

				//Render text textures
				instance.gPromptTexture->render( instance.gPromptTexture, &instance, ( SCREEN_WIDTH - instance.gPromptTexture->mWidth ) / 2, 0, NULL, 0, NULL, flipType );
				for( int i = 0; i < TOTAL_DATA; ++i )
				{
					instance.gDataTexture[ i ]->render( instance.gDataTexture[ i ], &instance, ( SCREEN_WIDTH - instance.gDataTexture[ i ]->mWidth ) / 2, instance.gPromptTexture->mHeight + instance.gDataTexture[ 0 ]->mHeight * i, NULL, 0, NULL, flipType );
				}

				//Update screen
				SDL_RenderPresent( instance.gRenderer );
			}
        }
    }

    close_sprite(&instance);
    return 0;
}
