#include "demo.h"

// releasing memory
void free_texture(lTexture_s *self)
{
    // Free texture if it exists
    if (self->mTexture != NULL)
    {
        SDL_DestroyTexture(self->mTexture);
        self->mTexture = NULL;
        self->mWidth = 0;
        self->mHeight = 0;
    }

    // Free surface if it exists
    if (self->mSurfacePixels != NULL)
    {
        SDL_FreeSurface(self->mSurfacePixels);
        self->mSurfacePixels = NULL;
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

// file loading
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

// methods
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

bool buildFont(LBitmapFont *self, CL_Instance *instance, char *path)
{
    bool success = true;

    if (!self->mFontTexture->loadPixelsFromFile(self->mFontTexture, instance, path))
    {
        printf("Unable to load bitmap font surface!\n");
        success = false;
    }
    else
    {
        // Get the background color
        Uint32 bgColor = self->mFontTexture->getPixel32(self->mFontTexture, 0, 0);
        // Set the cell dimensions
        int cellW = self->mFontTexture->mWidth / 16;
        int cellH = self->mFontTexture->mHeight / 16;

        // New line variables
        int top = cellH;
        int baseA = cellH;

        // The current character we're setting
        int currentChar = 0;
        // Go through the cell rows

        // Go through the cell rows
        for (int rows = 0; rows < 16; ++rows)
        {
            // Go through the cell columns
            for (int cols = 0; cols < 16; ++cols)
            {
                // Set the character offset
                self->mChars[currentChar].x = cellW * cols;
                self->mChars[currentChar].y = cellH * rows;

                // Set the dimensions of the character
                self->mChars[currentChar].w = cellW;
                self->mChars[currentChar].h = cellH;

                // Find Left Side
                // Go through pixel columns
                for (int pCol = 0; pCol < cellW; ++pCol)
                {
                    // Go through pixel rows
                    for (int pRow = 0; pRow < cellH; ++pRow)
                    {
                        // Get the pixel offsets
                        int pX = (cellW * cols) + pCol;
                        int pY = (cellH * rows) + pRow;

                        // If a non colorkey pixel is found
                        if (self->mFontTexture->getPixel32(self->mFontTexture, pX, pY) != bgColor)
                        {
                            // Set the x offset
                            self->mChars[currentChar].x = pX;

                            // Break the loops
                            pCol = cellW;
                            pRow = cellH;
                        }
                    }
                }

                // Find Right Side
                // Go through pixel columns
                for (int pColW = cellW - 1; pColW >= 0; --pColW)
                {
                    // Go through pixel rows
                    for (int pRowW = 0; pRowW < cellH; ++pRowW)
                    {
                        // Get the pixel offsets
                        int pX = (cellW * cols) + pColW;
                        int pY = (cellH * rows) + pRowW;

                        // If a non colorkey pixel is found
                        if (self->mFontTexture->getPixel32(self->mFontTexture, pX, pY) != bgColor)
                        {
                            // Set the width
                            self->mChars[currentChar].w = (pX - self->mChars[currentChar].x) + 1;

                            // Break the loops
                            pColW = -1;
                            pRowW = cellH;
                        }
                    }
                }

                // Find Top
                // Go through pixel rows
                for (int pRow = 0; pRow < cellH; ++pRow)
                {
                    // Go through pixel columns
                    for (int pCol = 0; pCol < cellW; ++pCol)
                    {
                        // Get the pixel offsets
                        int pX = (cellW * cols) + pCol;
                        int pY = (cellH * rows) + pRow;

                        // If a non colorkey pixel is found
                        if (self->mFontTexture->getPixel32(self->mFontTexture, pX, pY) != bgColor)
                        {
                            // If new top is found
                            if (pRow < top)
                            {
                                top = pRow;
                            }

                            // Break the loops
                            pCol = cellW;
                            pRow = cellH;
                        }
                    }
                }

                // Find Bottom of A
                if (currentChar == 'A')
                {
                    // Go through pixel rows
                    for (int pRow = cellH - 1; pRow >= 0; --pRow)
                    {
                        // Go through pixel columns
                        for (int pCol = 0; pCol < cellW; ++pCol)
                        {
                            // Get the pixel offsets
                            int pX = (cellW * cols) + pCol;
                            int pY = (cellH * rows) + pRow;

                            // If a non colorkey pixel is found
                            if (self->mFontTexture->getPixel32(self->mFontTexture, pX, pY) != bgColor)
                            {
                                // Bottom of a is found
                                baseA = pRow;

                                // Break the loops
                                pCol = cellW;
                                pRow = -1;
                            }
                        }
                    }
                }

                // Go to the next character
                ++currentChar;
            }
        }

        // Calculate space
        self->mSpace = cellW / 2;

        // Calculate new line
        self->mNewLine = baseA - top;

        // Lop off excess top pixels
        for (int i = 0; i < 256; ++i)
        {
            self->mChars[i].y += top;
            self->mChars[i].h -= top;
        }

        // Create final texture
        if (!self->mFontTexture->loadFromPixels(self->mFontTexture, instance))
        {
            printf("Unable to create font texture!\n");
            success = false;
        }
    }

    return success;
}

void renderText(LBitmapFont *self, CL_Instance* instance, int x, int y, char* text )
{
    //If the font has been built
    if( self->mFontTexture->mWidth > 0 )
    {
		//Temp offsets
		int curX = x, curY = y;

        //Go through the text
        for( int i = 0; i < strlen(text); ++i )
        {
            //If the current character is a space
            if( text[ i ] == ' ' )
            {
                //Move over
                curX += self->mSpace;
            }
            //If the current character is a newline
            else if( text[ i ] == '\n' )
            {
                //Move down
                curY += self->mNewLine;

                //Move back
                curX = x;
            }
            else
            {
                //Get the ASCII value of the character
                int ascii = (unsigned char)text[ i ];

                //Show the character
				self->mFontTexture->render(self->mFontTexture, instance, curX, curY, &self->mChars[ ascii ], 0, NULL, SDL_FLIP_NONE );

                //Move over the width of the character with one pixel of padding
                curX += self->mChars[ ascii ].w + 1;
            }
        }
    }
}

// get data
Uint32 *getPixels32(lTexture_s *self)
{
    Uint32 *pixels = NULL;

    if (self->mSurfacePixels != NULL)
    {
        // convert pixels to 32 bit
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

Uint32 getPixel32(lTexture_s *self, Uint32 x, Uint32 y)
{
    Uint32 *pixels = NULL;

    if (self->mSurfacePixels != NULL)
    {
        // convert pixels to 32 bit
        pixels = (Uint32 *)(self->mSurfacePixels->pixels);
    }

    return pixels[y * getPitch32(self) + x];
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
    // free_texture(instance->gBitmapFont);
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
    (*self)->getPixel32 = &getPixel32;
    (*self)->getPitch32 = &getPitch32;
    (*self)->mapRGBA = &mapRGBA;
    (*self)->loadPixelsFromFile = &loadPixelsFromFile;
    (*self)->loadFromPixels = &loadFromPixels;
}

void bitmap_constructor(LBitmapFont *self)
{
    self->mNewLine = 0;
    self->mSpace = 0;
    self->buildFont = &buildFont;
    self->renderText = &renderText;
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

bool loadMedia(CL_Instance *instance, LBitmapFont* font)
{
	//Loading success flag
	bool success = true;

	//Load font texture
	if( !instance->gBitmapFont->buildFont( instance->gBitmapFont, instance, "img/lazyfont.png" ) )
	{
		printf( "Failed to load bitmap font!\n" );
		success = false;
	}
	return success;
}

int main()
{
    CL_Instance instance;
    LBitmapFont font;
    bitmap_constructor(&font);
    texture_constructor(&font.mFontTexture);
    instance.gBitmapFont = &font;

    if (!init(&instance))
    {
        printf("Failed to initialize!\n");
    }
    else
    {
        // Load media
        if (!loadMedia(&instance, &font))
        {
            printf("Failed to load media!\n");
        }
        else
        {

            // Main loop flag
            bool quit = false;

            // Event handler
            SDL_Event e;

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
                }

                // Clear screen
                SDL_SetRenderDrawColor(instance.gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
                SDL_RenderClear(instance.gRenderer);

				//Render test text
				instance.gBitmapFont->renderText(instance.gBitmapFont, &instance, 0, 0, "Bitmap Font:\nABDCEFGHIJKLMNOPQRSTUVWXYZ\nabcdefghijklmnopqrstuvwxyz\n0123456789" );

                // Update screen
                SDL_RenderPresent(instance.gRenderer);
            }
        }
    }

    close_instance(&instance);
    return 0;
}
