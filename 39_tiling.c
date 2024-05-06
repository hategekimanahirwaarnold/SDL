#include "demo.h"

bool checkCollision( SDL_Rect a, SDL_Rect b )
{
    //The sides of the rectangles
    int leftA, leftB;
    int rightA, rightB;
    int topA, topB;
    int bottomA, bottomB;

    //Calculate the sides of rect A
    leftA = a.x;
    rightA = a.x + a.w;
    topA = a.y;
    bottomA = a.y + a.h;

    //Calculate the sides of rect B
    leftB = b.x;
    rightB = b.x + b.w;
    topB = b.y;
    bottomB = b.y + b.h;

    //If any of the sides from A are outside of B
    if( bottomA <= topB )
    {
        return false;
    }

    if( topA >= bottomB )
    {
        return false;
    }

    if( rightA <= leftB )
    {
        return false;
    }

    if( leftA >= rightB )
    {
        return false;
    }

    //If none of the sides from A are outside B
    return true;
}

bool touchesWall( SDL_Rect box, Tile* tiles[] )
{
    //Go through the tiles
    for( int i = 0; i < TOTAL_TILES; ++i )
    {
        //If the tile is a wall type tile
        if( ( tiles[ i ]->mType >= TILE_CENTER ) && ( tiles[ i ]->mType <= TILE_TOPLEFT ) )
        {
            //If the collision box touches the wall tile
            if( checkCollision( box, tiles[ i ]->mBox ) )
            {
                return true;
            }
        }
    }

    //If no wall tiles were touched
    return false;
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

//closeing
void close_sprite(CL_Instance *instance)
{
    free_texture(instance->gDotTexture);
    free_texture(instance->gRedTexture);
    free_texture(instance->gGreenTexture);
    free_texture(instance->gShimmerTexture);

    // Destroy windows
    for (int i = 0; i < TOTAL_WINDOWS; ++i)
    {
        free_window(instance->gWindows[i]);
    }
    SDL_Quit();
}

void close_tile(Tile* tiles[])
{
    //Deallocate tiles
	for( int i = 0; i < TOTAL_TILES; ++i )
	{
		 if( tiles[ i ] != NULL )
		 {
			free(tiles[ i ]);
			tiles[ i ] = NULL;
		 }
	}
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

SDL_Renderer *createRenderer(lWindow *self)
{
    return SDL_CreateRenderer(self->mWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
}

void handleEvent(Dot_s *self, SDL_Event *e)
{
    // If a key was pressed
    if (e->type == SDL_KEYDOWN && e->key.repeat == 0)
    {
        // Adjust the velocity
        switch (e->key.keysym.sym)
        {
        case SDLK_UP:
            self->mVelY -= self->DOT_VEL;
            break;
        case SDLK_DOWN:
            self->mVelY += self->DOT_VEL;
            break;
        case SDLK_LEFT:
            self->mVelX -= self->DOT_VEL;
            break;
        case SDLK_RIGHT:
            self->mVelX += self->DOT_VEL;
            break;
        }
    }
    // If a key was released
    else if (e->type == SDL_KEYUP && e->key.repeat == 0)
    {
        // Adjust the velocity
        switch (e->key.keysym.sym)
        {
        case SDLK_UP:
            self->mVelY += self->DOT_VEL;
            break;
        case SDLK_DOWN:
            self->mVelY -= self->DOT_VEL;
            break;
        case SDLK_LEFT:
            self->mVelX += self->DOT_VEL;
            break;
        case SDLK_RIGHT:
            self->mVelX -= self->DOT_VEL;
            break;
        }
    }
}

void focus(lWindow *self)
{
    // Restore window if needed
    if (!self->mShown)
    {
        SDL_ShowWindow(self->mWindow);
    }
    // Move window forward
    SDL_RaiseWindow(self->mWindow);
}

bool isDead(Particle *self)
{
    return self->mFrame > 10;
}

void render_particle(Particle *self, CL_Instance *instance)
{
    // Show image
    self->mTexture->render(self->mTexture, instance, self->mPosX, self->mPosY, NULL, 0, NULL, SDL_FLIP_NONE);

    // Show shimmer
    if (self->mFrame % 2 == 0)
    {
        instance->gShimmerTexture->render(instance->gShimmerTexture, instance, self->mPosX, self->mPosY, NULL, 0, NULL, SDL_FLIP_NONE);
    }

    // Animate
    self->mFrame++;
}

void render_tile(Tile* self, SDL_Rect* camera, CL_Instance* instance)
{
    //If the tile is on screen
    if (checkCollision(*camera, self->mBox))
    {
        //show tile
        instance->gTileTexture->render( instance->gTileTexture, instance, self->mBox.x - camera->x, self->mBox.y - camera->y, &instance->gTileClips[self->mType], 0 , NULL, SDL_FLIP_NONE );
    }
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
}

void windows_constructor(lWindow *self)
{
    // lWindow window;
    // *self = &window;
    (self)->mWindow = NULL;
    (self)->mMouseFocus = true;
    (self)->mKeyboardFocus = true;
    (self)->mFullScreen = false;
    (self)->mMinimized = false;
    (self)->mShown = false;
    (self)->mWindowID = -1;
    (self)->mWidth = 0;
    (self)->mHeight = 0;
    // (self)->handleEvent = &handleEvent;
    (self)->createRenderer = &createRenderer;
    (self)->focus = &focus;
    (self)->render = &render_window;
}

void particle_constructor(Particle *self, CL_Instance *instance, int x, int y)
{
    static int i = 0;
    // Set offsets
    self->mPosX = x - 5 + (rand() % 25);
    self->mPosY = y - 5 + (rand() & 25);

    // Initialize animation
    self->mFrame = rand() & 5;

    // Set type
    switch (rand() & 3)
    {
    case 0:
        self->mTexture = instance->gRedTexture;
        break;
    case 1:
        self->mTexture = instance->gGreenTexture;
        break;
    case 2:
        self->mTexture = instance->gBlueTexture;
        break;
    default:
        self->mTexture = instance->gRedTexture;
        break;
    }
    self->isDead = &isDead;
    self->render = &render_particle;
    // printf("initialized particle: %d, isDead ? : %d\n", i++, self->isDead(self));
}

void tile_constructor(Tile* self, int x, int y, int tileType)
{
    //Get the offsets
    self->mBox.x = x;
    self->mBox.y = y;

    //set the collision box
    self->mBox.w = TILE_WIDTH;
    self->mBox.h = TILE_HEIGHT;

    self->mType = tileType;
    self->render = &render_tile;
}

void move(Dot_s *self, Tile *tiles[])
{
    //Move the dot left or right
    self->mBox.x += self->mVelX;
    //If the dot went too far to the left or right or touched a wall
    if( ( self->mBox.x < 0 ) || ( self->mBox.x + self->DOT_WIDTH > LEVEL_WIDTH ) || touchesWall( self->mBox, tiles ) )
    {
        //move back
        self->mBox.x -= self->mVelX;
    }

    //Move the dot up or down
    self->mBox.y += self->mVelY;

    //If the dot went too far up or down or touched a wall
    if( ( self->mBox.y < 0 ) || ( self->mBox.y + self->DOT_HEIGHT > LEVEL_HEIGHT ) || touchesWall( self->mBox, tiles ) )
    {
        //move back
        self->mBox.y -= self->mVelY;
    }
}

void setCamera(Dot_s* self, SDL_Rect* camera )
{
	//Center the camera over the dot
	camera->x = ( self->mBox.x + self->DOT_WIDTH / 2 ) - SCREEN_WIDTH / 2;
	camera->y = ( self->mBox.y + self->DOT_HEIGHT / 2 ) - SCREEN_HEIGHT / 2;

	//Keep the camera in bounds
	if( camera->x < 0 )
	{ 
		camera->x = 0;
	}
	if( camera->y < 0 )
	{
		camera->y = 0;
	}
	if( camera->x > LEVEL_WIDTH - camera->w )
	{
		camera->x = LEVEL_WIDTH - camera->w;
	}
	if( camera->y > LEVEL_HEIGHT - camera->h )
	{
		camera->y = LEVEL_HEIGHT - camera->h;
	}
}

void dot_constructor(Dot_s *self, CL_Instance *instance)
{
    self->DOT_WIDTH = 20;
    self->DOT_HEIGHT = 20;
    self->mPosX = 0;
    self->mPosY = 0;
    self->mVelX = 0;
    self->mVelY = 0;
    self->DOT_VEL = 10;
    self->mBox.x = 0;
    self->mBox.y = 0;
    self->mBox.w = self->DOT_WIDTH;
    self->mBox.h = self->DOT_HEIGHT;

    // self->move = &move;
}

void renderParticles(Dot_s *self, CL_Instance *instance)
{
    // Go through particles
    for (int i = 0; i < TOTAL_PARTICLES; ++i)
    {
        // Delete and replace dead particles
        if (self->particles[i]->isDead(self->particles[i]))
        {
            free(self->particles[i]);
            Particle *newParticle = malloc(sizeof(Particle));
            self->particles[i] = newParticle;
            particle_constructor(self->particles[i], instance, self->mPosX, self->mPosY);
        }
    }
    // Show particles
    for (int i = 0; i < TOTAL_PARTICLES; ++i)
    {
        self->particles[i]->render(self->particles[i], instance);
    }
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

bool setTiles(Tile *tiles[], CL_Instance* instance) {
    // Success flag
    int tilesLoaded = true;

    // The tile offsets
    int x = 0, y = 0;

    // Open the map file
    FILE *map = fopen("img/lazy.map", "r");

    // If the map couldn't be loaded
    if (!map) {
        printf("Unable to load map file!\n");
        tilesLoaded = false;
    } else {
        // Initialize the tiles
        for (int i = 0; i < TOTAL_TILES; ++i) {
            // Determines what kind of tile will be made
            int tileType = -1;

            // Read tile from map file
            if (fscanf(map, "%d", &tileType) != 1) {
                // If there was a problem in reading the map
                printf("Error loading map: Unexpected end of file!\n");
                tilesLoaded = false;
                break;
            }

            // If the number is a valid tile number
            if (tileType >= 0 && tileType < TOTAL_TILE_SPRITES) {
                tiles[i] = (Tile *)malloc(sizeof(Tile));
                tile_constructor(tiles[i], x, y, tileType);
            } else {
                // If we don't recognize the tile type
                printf("Error loading map: Invalid tile type at %d!\n", i);
                tilesLoaded = false;
                break;
            }

            // Move to next tile spot
            x += TILE_WIDTH;

            // If we've gone too far
            if (x >= LEVEL_WIDTH) {
                // Move back
                x = 0;

                // Move to the next row
                y += TILE_HEIGHT;
            }
        }

        //Clip the sprite sheet
		if( tilesLoaded )
		{
			instance->gTileClips[ TILE_RED ].x = 0;
			instance->gTileClips[ TILE_RED ].y = 0;
			instance->gTileClips[ TILE_RED ].w = TILE_WIDTH;
			instance->gTileClips[ TILE_RED ].h = TILE_HEIGHT;

			instance->gTileClips[ TILE_GREEN ].x = 0;
			instance->gTileClips[ TILE_GREEN ].y = 80;
			instance->gTileClips[ TILE_GREEN ].w = TILE_WIDTH;
			instance->gTileClips[ TILE_GREEN ].h = TILE_HEIGHT;

			instance->gTileClips[ TILE_BLUE ].x = 0;
			instance->gTileClips[ TILE_BLUE ].y = 160;
			instance->gTileClips[ TILE_BLUE ].w = TILE_WIDTH;
			instance->gTileClips[ TILE_BLUE ].h = TILE_HEIGHT;

			instance->gTileClips[ TILE_TOPLEFT ].x = 80;
			instance->gTileClips[ TILE_TOPLEFT ].y = 0;
			instance->gTileClips[ TILE_TOPLEFT ].w = TILE_WIDTH;
			instance->gTileClips[ TILE_TOPLEFT ].h = TILE_HEIGHT;

			instance->gTileClips[ TILE_LEFT ].x = 80;
			instance->gTileClips[ TILE_LEFT ].y = 80;
			instance->gTileClips[ TILE_LEFT ].w = TILE_WIDTH;
			instance->gTileClips[ TILE_LEFT ].h = TILE_HEIGHT;

			instance->gTileClips[ TILE_BOTTOMLEFT ].x = 80;
			instance->gTileClips[ TILE_BOTTOMLEFT ].y = 160;
			instance->gTileClips[ TILE_BOTTOMLEFT ].w = TILE_WIDTH;
			instance->gTileClips[ TILE_BOTTOMLEFT ].h = TILE_HEIGHT;

			instance->gTileClips[ TILE_TOP ].x = 160;
			instance->gTileClips[ TILE_TOP ].y = 0;
			instance->gTileClips[ TILE_TOP ].w = TILE_WIDTH;
			instance->gTileClips[ TILE_TOP ].h = TILE_HEIGHT;

			instance->gTileClips[ TILE_CENTER ].x = 160;
			instance->gTileClips[ TILE_CENTER ].y = 80;
			instance->gTileClips[ TILE_CENTER ].w = TILE_WIDTH;
			instance->gTileClips[ TILE_CENTER ].h = TILE_HEIGHT;

			instance->gTileClips[ TILE_BOTTOM ].x = 160;
			instance->gTileClips[ TILE_BOTTOM ].y = 160;
			instance->gTileClips[ TILE_BOTTOM ].w = TILE_WIDTH;
			instance->gTileClips[ TILE_BOTTOM ].h = TILE_HEIGHT;

			instance->gTileClips[ TILE_TOPRIGHT ].x = 240;
			instance->gTileClips[ TILE_TOPRIGHT ].y = 0;
			instance->gTileClips[ TILE_TOPRIGHT ].w = TILE_WIDTH;
			instance->gTileClips[ TILE_TOPRIGHT ].h = TILE_HEIGHT;

			instance->gTileClips[ TILE_RIGHT ].x = 240;
			instance->gTileClips[ TILE_RIGHT ].y = 80;
			instance->gTileClips[ TILE_RIGHT ].w = TILE_WIDTH;
			instance->gTileClips[ TILE_RIGHT ].h = TILE_HEIGHT;

			instance->gTileClips[ TILE_BOTTOMRIGHT ].x = 240;
			instance->gTileClips[ TILE_BOTTOMRIGHT ].y = 160;
			instance->gTileClips[ TILE_BOTTOMRIGHT ].w = TILE_WIDTH;
			instance->gTileClips[ TILE_BOTTOMRIGHT ].h = TILE_HEIGHT;
		}
	
        // Close the file
        fclose(map);
    }

    // If the map was loaded fine
    return tilesLoaded;
}

bool loadMedia_2(CL_Instance *instance, Tile* tiles[])
{

	//Loading success flag
	bool success = true;

	//Load dot texture
	if( !instance->gDotTexture->loadFromFile(instance->gDotTexture, instance, "img/dot_2.bmp" ) )
	{
		printf( "Failed to load dot texture!\n" );
		success = false;
	}

	//Load tile texture
	if( !instance->gTileTexture->loadFromFile(instance->gTileTexture, instance, "img/tiles.png" ) )
	{
		printf( "Failed to load tile set texture!\n" );
		success = false;
	}

	//Load tile map
	if( !setTiles( tiles, instance ) )
	{
		printf( "Failed to load tile set!\n" );
		success = false;
	}

	return success;
}

int main()
{
    CL_Instance instance;
    texture_constructor(&instance.gDotTexture);
    texture_constructor(&instance.gTileTexture);
    // Start up SDL and create window
    if (!init(&instance))
    {
        printf("Failed to initialize!\n");
    }
    else
    {
        //The level tiles
		Tile* tileSet[ TOTAL_TILES ];

        // Load media
        if (!loadMedia_2(&instance, tileSet))
        {
            printf("Failed to load media!\n");
        }
        else
        {
            // Main loop flag
            bool quit = false;

            // Event handler
            SDL_Event e;
        

            // The dot that will be moving around on the screen
            Dot_s dot;
            dot_constructor(&dot, &instance);
            
            //Level camera
			SDL_Rect camera = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };

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

                    // Handle input for the dot

                    handleEvent(&dot, &e);
                }

                // Move the dot
                move(&dot, tileSet);
                setCamera(&dot, &camera);

                // Clear screen
                SDL_SetRenderDrawColor(instance.gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
                SDL_RenderClear(instance.gRenderer);

				//Render level
				for( int i = 0; i < TOTAL_TILES; ++i )
				{
					tileSet[ i ]->render(tileSet[i], &camera, &instance);
				}

                //Show the dot relative to the camera
	            instance.gDotTexture->render(instance.gDotTexture, &instance, dot.mBox.x - camera.x, dot.mBox.y - camera.y, NULL, 0, NULL, SDL_FLIP_NONE);
                // Update screen
                SDL_RenderPresent(instance.gRenderer);
            }
        }

        close_tile(tileSet);
    }

    // Free resources and close SDL
    close_sprite(&instance);
    return 0;
}
