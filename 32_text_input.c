#include "demo.h"

bool loadMedia(CL_Instance* instance)
{
    bool success = true;

    instance->gFont = TTF_OpenFont("img/lazy.ttf", 28);
    if (instance->gFont == NULL)
    {
        printf("Failed to load lazy font! SDL_ttf Error: %s\n", TTF_GetError() );
        success = false;
    } else
    {
        //Render text
        SDL_Color textColor = {0, 0, 0, 0};
        if (!instance->gPromptTexture->loadFromRenderedText(instance->gPromptTexture, instance, "Enter text: ", textColor) )
        {
            printf("Failed to render text texture!\n");
            success = false;
        }
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
    texture_constructor(&instance.gPromptTexture);
    texture_constructor(&instance.gInputTextTexture);
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
            char* inputText = malloc(sizeof(char) * 100);
            strcpy(inputText, "Hirwa is cool!");
            instance.gInputTextTexture->loadFromRenderedText( instance.gInputTextTexture, &instance, inputText, textColor );


            // Enable text input
            SDL_StartTextInput();
            while(!quit)
            {
                //rerender text flag
                bool renderText = false;
                while(SDL_PollEvent( &e ) != 0)
                {
                    if (e.type == SDL_QUIT)
                    {
                        quit = true;
                    } else if( e.type == SDL_KEYDOWN )
                    {
                        //Handle backspace
                        if( e.key.keysym.sym == SDLK_BACKSPACE && strlen(inputText) > 0 )
                        {
                            // remove last character
                            inputText[strlen(inputText) - 1] = '\0';
                            renderText = true;
                        }
                        //Handle copy
                        else if( e.key.keysym.sym == SDLK_c && SDL_GetModState() & KMOD_CTRL )
                        {
                            SDL_SetClipboardText( inputText );
                        }
                        //Handle paste
                        else if( e.key.keysym.sym == SDLK_v && SDL_GetModState() & KMOD_CTRL )
                        {
                            //Copy text from temporary buffer
                            char* tempText = SDL_GetClipboardText();
                            inputText = tempText;
                            SDL_free( tempText );

                            renderText = true;
                        }
                    }                    
                    //Special text input event
                    else if( e.type == SDL_TEXTINPUT )
                    {
                        //Not copy or pasting
                        if( !( SDL_GetModState() & KMOD_CTRL && ( e.text.text[ 0 ] == 'c' || e.text.text[ 0 ] == 'C' || e.text.text[ 0 ] == 'v' || e.text.text[ 0 ] == 'V' ) ) )
                        {
                            //Append character        
                            strcat(inputText, e.text.text);
                            renderText = true;
                        }
                    }
                } 

				//Rerender text if needed
				if( renderText )
				{
					//Text is not empty
					if( strcmp(inputText, "") != 0 )
					{
						//Render new text
                        instance.gInputTextTexture->loadFromRenderedText( instance.gInputTextTexture, &instance, inputText, textColor );
					}
					//Text is empty
					else
					{
						//Render space texture
                        instance.gInputTextTexture->loadFromRenderedText( instance.gInputTextTexture, &instance, " ", textColor );
					}
				}
                // clear screen
                SDL_SetRenderDrawColor(instance.gRenderer, 0xFF, 0xFF, 0xFF, 0xFF );
                SDL_RenderClear(instance.gRenderer);

                // render arrow texture
                instance.gPromptTexture->render(instance.gPromptTexture, &instance, (SCREEN_WIDTH - instance.gPromptTexture->mWidth) / 2, 0, NULL, 0, NULL, flipType);
                instance.gInputTextTexture->render(instance.gInputTextTexture, &instance, (SCREEN_WIDTH - instance.gInputTextTexture->mWidth) / 2, instance.gPromptTexture->mHeight, NULL, 0, NULL, flipType);

                //Update screen
                SDL_RenderPresent(instance.gRenderer);
            }

            //DISABLE TEXT INPUT
            SDL_StopTextInput();
        }
    }

    close_sprite(&instance);
    return 0;
}
