#include "demo.h"

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
            printf("velocity up!\n");
            break;
        case SDLK_DOWN:
            self->mVelY += self->DOT_VEL;
            printf("velocity down!\n");
            break;
        case SDLK_LEFT:
            self->mVelX -= self->DOT_VEL;
            printf("velocity left!\n");
            break;
        case SDLK_RIGHT:
            self->mVelX += self->DOT_VEL;
            printf("velocity right!\n");
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

void move(Dot_s *self)
{
    // Move the dot left or right
    self->mPosX += self->mVelX;

    // If the dot went too far to the left or right
    if ((self->mPosX < 0) || (self->mPosX + self->DOT_WIDTH > SCREEN_WIDTH))
    {
        // Move back
        self->mPosX -= self->mVelX;
    }

    // Move the dot up or down
    self->mPosY += self->mVelY;

    // If the dot went too far up or down
    if ((self->mPosY < 0) || (self->mPosY + self->DOT_HEIGHT > SCREEN_HEIGHT))
    {
        // Move back
        self->mPosY -= self->mVelY;
    }
}

void dot_constructor(Dot_s *self, CL_Instance *instance)
{
    self->mPosX = 0;
    self->mPosY = 0;
    self->mVelX = 0;
    self->mVelY = 0;
    self->DOT_VEL = 10;
    self->particles = malloc(sizeof(Particle *) * TOTAL_PARTICLES);
    for (int i = 0; i < TOTAL_PARTICLES; i++)
    {
        self->particles[i] = malloc(sizeof(Particle));
        if (self->particles[i] == NULL)
        {
            // Handle allocation failure
            // For example, you can free previously allocated memory and return or exit the function
            for (int j = 0; j < i; j++)
            {
                free(self->particles[j]);
            }
            free(self->particles);
            self->particles = NULL;
            return;
        }
        particle_constructor(self->particles[i], instance, self->mPosX, self->mPosY);
    }

    self->move = &move;
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

bool loadMedia(CL_Instance *instance)
{

    // Loading success flag
    bool success = true;

    // Load dot texture
    if (!instance->gDotTexture->loadFromFile(instance->gDotTexture, instance, "img/dot_2.bmp"))
    {
        printf("Failed to load dot texture!\n");
        success = false;
    }

    // Load red texture
    if (!instance->gRedTexture->loadFromFile(instance->gRedTexture, instance, "img/red.bmp"))
    {
        printf("Failed to load red texture!\n");
        success = false;
    }

    // Load green texture
    if (!instance->gGreenTexture->loadFromFile(instance->gGreenTexture, instance, "img/green.bmp"))
    {
        printf("Failed to load green texture!\n");
        success = false;
    }

    // Load blue texture
    if (!instance->gBlueTexture->loadFromFile(instance->gBlueTexture, instance, "img/blue.bmp"))
    {
        printf("Failed to load blue texture!\n");
        success = false;
    }

    // Load shimmer texture
    if (!instance->gShimmerTexture->loadFromFile(instance->gShimmerTexture, instance, "img/shimmer.bmp"))
    {
        printf("Failed to load shimmer texture!\n");
        success = false;
    }

    // Set texture transparency
    instance->gRedTexture->setAlpha(instance->gRedTexture->mTexture, 192);
    instance->gGreenTexture->setAlpha(instance->gGreenTexture->mTexture, 192);
    instance->gBlueTexture->setAlpha(instance->gBlueTexture->mTexture, 192);
    instance->gShimmerTexture->setAlpha(instance->gShimmerTexture->mTexture, 192);

    return success;
}

int main()
{
    CL_Instance instance;
    texture_constructor(&instance.gDotTexture);
    texture_constructor(&instance.gRedTexture);
    texture_constructor(&instance.gGreenTexture);
    texture_constructor(&instance.gBlueTexture);
    texture_constructor(&instance.gShimmerTexture);
    // Start up SDL and create window
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
                move(&dot);

                // Clear screen
                SDL_SetRenderDrawColor(instance.gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
                SDL_RenderClear(instance.gRenderer);

                // Render objects
                instance.gDotTexture->render(instance.gDotTexture, &instance, dot.mPosX, dot.mPosY, NULL, 0, NULL, SDL_FLIP_NONE);
                renderParticles(&dot, &instance);
                // Update screen
                SDL_RenderPresent(instance.gRenderer);
            }
        }
    }

    // Free resources and close SDL
    close_sprite(&instance);

    return 0;
}
