#include "demo.h"

bool loadMedia(CL_Instance *instance)
{
    bool success = true;

    instance->gFont = TTF_OpenFont("img/lazy.ttf", 28);
    if (instance->gFont == NULL)
    {
        printf("Failed to load lazy font! SDL_ttf Error: %s\n", TTF_GetError());
        success = false;
    }
    else
    {
        instance->gPromptTexture->loadFromRenderedText(instance->gPromptTexture, instance, "Select your recording ", gTextColor);
        // Get capture device count
       	// int gRecordingDeviceCount = SDL_GetNumAudioDevices( SDL_TRUE );
        // printf("count: %d\n", gRecordingDeviceCount);
        instance->gRecordingDeviceCount = SDL_GetNumAudioDevices(SDL_TRUE);
        // No recording devices
        if (instance->gRecordingDeviceCount < 1)
        {
            printf("Unable to get audio capture device! SDL Error: %s\n", SDL_GetError());
            success = false;
        }
        // At least one device connected
        else
        {
            // Cap recording device count
            if (instance->gRecordingDeviceCount > MAX_RECORDING_DEVICES)
            {
                instance->gRecordingDeviceCount = MAX_RECORDING_DEVICES;
            }
            //Render device names
            char promptText[100];
            for (int i = 0; i < instance->gRecordingDeviceCount; ++i )
            {
                //Get capture device name
                sprintf(promptText, "%d: %s", i, SDL_GetAudioDeviceName(i, SDL_TRUE));

                //set texture from name
                instance->gDeviceTextures[ i ]->loadFromRenderedText( instance->gDeviceTextures[i], instance, promptText, gTextColor);
            }
        }
    }

    return success;
}

bool loadFromFile(lTexture_s *self, CL_Instance *instance, char *path)
{
    SDL_Texture *newTexture = NULL;
    SDL_Surface *loadedSurface = IMG_Load(path);

    if (loadedSurface == NULL)
    {
        printf("Unable to load image %s! SDL_image Error: %s\n", path, IMG_GetError());
    }
    else
    {
        SDL_SetColorKey(loadedSurface, SDL_TRUE, SDL_MapRGB(loadedSurface->format, 0, 0xFF, 0xFF));
        newTexture = SDL_CreateTextureFromSurface(instance->gRenderer, loadedSurface);
        if (newTexture == NULL)
        {
            printf("Unable to create texture from %s! SDL Error: %s\n", path, SDL_GetError());
        }
        else
        {
            self->mWidth = loadedSurface->w;
            self->mHeight = loadedSurface->h;
        }
        // Get rid of old loaded surface
        SDL_FreeSurface(loadedSurface);
    }
    self->mTexture = newTexture;

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

void free_texture(lTexture_s *self)
{
    if (self->mTexture != NULL)
    {
        SDL_DestroyTexture(self->mTexture);
        self->mTexture = NULL;
        self->mWidth = 0;
        self->mHeight = 0;
    }
}

void close_sprite(CL_Instance *instance)
{
    // Free playback audio
    if (gRecordingBuffer != NULL)
    {
        free(gRecordingBuffer);
        gRecordingBuffer = NULL;
    }
    // Free loaded images
    free_texture(instance->gPromptTexture);
    for (int i = 0; i < MAX_RECORDING_DEVICES; ++i)
    {
        free_texture(instance->gDeviceTextures[i]);
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

bool init(CL_Instance *instance)
{
    // Initialization flag
    bool success = true;

    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0)
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
                // Initialize SDL_ttf
                if (TTF_Init() == -1)
                {
                    printf("SDL_ttf could not initialize! SDL_ttf Error: %s\n", TTF_GetError());
                    success = false;
                }
            }
        }
    }

    return success;
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

void audioRecordingCallback( void* userdata, Uint8* stream, int len )
{
	//Copy audio from stream
	memcpy( &gRecordingBuffer[ gBufferBytePosition ], stream, len );

	//Move along buffer
	gBufferBytePosition += len;
}

void audioPlaybackCallback( void* userdata, Uint8* stream, int len )
{
	//Copy audio to stream
	memcpy( stream, &gRecordingBuffer[ gBufferBytePosition ], len );

	//Move along buffer
	gBufferBytePosition += len;
}

int main()
{
    CL_Instance instance;
    //audio data
    instance.gRecordingDeviceCount = 0;
    instance.gBufferByteMaxPosition = 0;

    texture_constructor(&instance.gPromptTexture);
    for (int i = 0; i < MAX_RECORDING_DEVICES; i++)
        texture_constructor(&instance.gDeviceTextures[i]);

    SDL_RendererFlip flipType = SDL_FLIP_NONE;

    if (!init(&instance))
    {
        printf("Failed to initialize!\n");
    }
    else
    {
        // load media
        if (!loadMedia(&instance))
        {
            printf("Failed to load media!\n");
        }
        else
        {
            bool quit = false;
            SDL_Event e;

            int currentState = SELECTING_DEVICE;
            SDL_AudioDeviceID recordingDeviceId = 0;
            SDL_AudioDeviceID playbackDeviceId = 0;

            int currentData = 0;
            char data[20];

            // Enable text input
            SDL_StartTextInput();
            // While application is running
            while (!quit)
            {
                // Handle events on queue
                while (SDL_PollEvent(&e) != 0)
                {
                    // User requests quit
                    if (e.type == SDL_QUIT)
                    {
                        quit = true;
                    }
					//Do current state event handling
					switch( currentState )
					{
						//User is selecting recording device
						case SELECTING_DEVICE:

							//On key press
							if( e.type == SDL_KEYDOWN )
							{
								//Handle key press from 0 to 9 
								if( e.key.keysym.sym >= SDLK_0 && e.key.keysym.sym <= SDLK_9 )
								{
									//Get selection index
									int index = e.key.keysym.sym - SDLK_0;

									//Index is valid
									if( index < instance.gRecordingDeviceCount )
									{
										//Default audio spec
										SDL_AudioSpec desiredRecordingSpec;
										SDL_zero(desiredRecordingSpec);
										desiredRecordingSpec.freq = 44100;
										desiredRecordingSpec.format = AUDIO_F32;
										desiredRecordingSpec.channels = 2;
										desiredRecordingSpec.samples = 4096;
										desiredRecordingSpec.callback = audioRecordingCallback;

										//Open recording device
										recordingDeviceId = SDL_OpenAudioDevice( SDL_GetAudioDeviceName( index, SDL_TRUE ), SDL_TRUE, &desiredRecordingSpec, &instance.gReceivedRecordingSpec, SDL_AUDIO_ALLOW_FORMAT_CHANGE );
										
										//Device failed to open
										if( recordingDeviceId == 0 )
										{
											//Report error
											printf( "Failed to open recording device! SDL Error: %s", SDL_GetError() );
											instance.gPromptTexture->loadFromRenderedText( instance.gPromptTexture, &instance, "Failed to open recording device!", gTextColor );
											currentState = ERROR;
										}
										//Device opened successfully
										else
										{
											//Default audio spec
											SDL_AudioSpec desiredPlaybackSpec;
											SDL_zero(desiredPlaybackSpec);
											desiredPlaybackSpec.freq = 44100;
											desiredPlaybackSpec.format = AUDIO_F32;
											desiredPlaybackSpec.channels = 2;
											desiredPlaybackSpec.samples = 4096;
											desiredPlaybackSpec.callback = audioPlaybackCallback;

											//Open playback device
											playbackDeviceId = SDL_OpenAudioDevice( NULL, SDL_FALSE, &desiredPlaybackSpec, &instance.gReceivedPlaybackSpec, SDL_AUDIO_ALLOW_FORMAT_CHANGE );

											//Device failed to open
											if( playbackDeviceId == 0 )
											{
												//Report error
												printf( "Failed to open playback device! SDL Error: %s", SDL_GetError() );
												instance.gPromptTexture->loadFromRenderedText(instance.gPromptTexture, &instance, "Failed to open playback device!", gTextColor );
												currentState = ERROR;
											}
											//Device opened successfully
											else
											{
												//Calculate per sample bytes
												int bytesPerSample = instance.gReceivedRecordingSpec.channels * ( SDL_AUDIO_BITSIZE( instance.gReceivedRecordingSpec.format ) / 8 );

												//Calculate bytes per second
												int bytesPerSecond = instance.gReceivedRecordingSpec.freq * bytesPerSample;

												//Calculate buffer size
												instance.gBufferByteSize = ( RECORDING_BUFFER_SECONDS ) * bytesPerSecond;

												//Calculate max buffer use
												instance.gBufferByteMaxPosition = MAX_RECORDING_SECONDS * bytesPerSecond;

												//Allocate and initialize byte buffer
												gRecordingBuffer = malloc( instance.gBufferByteSize );
												memset( gRecordingBuffer, 0, instance.gBufferByteSize );

												//Go on to next state
												instance.gPromptTexture->loadFromRenderedText(instance.gPromptTexture, &instance, "Press 1 to record for 5 seconds.", gTextColor);
												currentState = STOPPED;
											}
										}
									}
								}
							}
							break;	

						//User getting ready to record
						case STOPPED:

							//On key press
							if( e.type == SDL_KEYDOWN )
							{
								//Start recording
								if( e.key.keysym.sym == SDLK_1 )
								{
									//Go back to beginning of buffer
									gBufferBytePosition = 0;

									//Start recording
									SDL_PauseAudioDevice( recordingDeviceId, SDL_FALSE );

									//Go on to next state
									instance.gPromptTexture->loadFromRenderedText(instance.gPromptTexture, &instance, "Recording...", gTextColor );
									currentState = RECORDING;
								}
							}
							break;	

						//User has finished recording
						case RECORDED:

							//On key press
							if( e.type == SDL_KEYDOWN )
							{
								//Start playback
								if( e.key.keysym.sym == SDLK_1 )
								{
									//Go back to beginning of buffer
									gBufferBytePosition = 0;

									//Start playback
									SDL_PauseAudioDevice( playbackDeviceId, SDL_FALSE );

									//Go on to next state
									instance.gPromptTexture->loadFromRenderedText(instance.gPromptTexture, &instance, "Playing...", gTextColor );
									currentState = PLAYBACK;
								}
								//Record again
								if( e.key.keysym.sym == SDLK_2 )
								{
									//Reset the buffer
									gBufferBytePosition = 0;
									memset( gRecordingBuffer, 0, instance.gBufferByteSize );

									//Start recording
									SDL_PauseAudioDevice( recordingDeviceId, SDL_FALSE );

									//Go on to next state
									instance.gPromptTexture->loadFromRenderedText(instance.gPromptTexture, &instance, "Recording...", gTextColor );
									currentState = RECORDING;
								}
							}
							break;
					}
                
                }

                // Clear screen
                SDL_SetRenderDrawColor(instance.gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
                SDL_RenderClear(instance.gRenderer);

                // Render text textures
                instance.gPromptTexture->render(instance.gPromptTexture, &instance, (SCREEN_WIDTH - instance.gPromptTexture->mWidth) / 2, 0, NULL, 0, NULL, flipType);
                for (int i = 0; i < MAX_RECORDING_DEVICES; ++i)
                {
                    instance.gDeviceTextures[i]->render(instance.gDeviceTextures[i], &instance, (SCREEN_WIDTH - instance.gDeviceTextures[i]->mWidth) / 2, instance.gPromptTexture->mHeight + instance.gDeviceTextures[0]->mHeight * i, NULL, 0, NULL, flipType);
                }

                // Update screen
                SDL_RenderPresent(instance.gRenderer);
            }
        }
    }

    close_sprite(&instance);
    return 0;
}
