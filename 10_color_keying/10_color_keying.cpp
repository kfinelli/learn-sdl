/*This source code copyrighted by Lazy Foo' Productions (2004-2015)
and may not be redistributed without written permission.*/
//KF- modified to use smart pointers

//Using SDL, SDL_image, standard IO, and strings
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <string>
#include <memory>

//Screen dimension constants
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

//Texture wrapper class
class LTexture {
  public:
    //Initializes variables
    LTexture();

    //Deallocates memory
    ~LTexture();

    //Loads image at specified path
    bool loadFromFile( std::string path );

    //Deallocates texture
    void free();

    //Renders texture at given point
    void render( int x, int y );

    //Gets image dimensions
    int getWidth();
    int getHeight();

  private:
    //The actual hardware texture
    std::unique_ptr<SDL_Texture, decltype(&SDL_DestroyTexture)> mTexture;


    //Image dimensions
    int mWidth;
    int mHeight;
};

//Starts up SDL and creates window
bool init();

//Loads media
bool loadMedia();

//Frees media and shuts down SDL
void close();

//The window we'll be rendering to
std::unique_ptr<SDL_Window, decltype(&SDL_DestroyWindow)> gWindow(nullptr, SDL_DestroyWindow);

//The window renderer
std::unique_ptr<SDL_Renderer, decltype(&SDL_DestroyRenderer)> gRenderer(nullptr, SDL_DestroyRenderer);

//Scene textures
LTexture gFooTexture;
LTexture gBackgroundTexture;


LTexture::LTexture() :
  mTexture(nullptr, SDL_DestroyTexture),
  mWidth(0), mHeight(0)
{
}

LTexture::~LTexture()
{
	//Deallocate
	//free();
}

bool LTexture::loadFromFile( std::string path )
{
  //Get rid of preexisting texture
  free();

  //Load image at specified path
  std::unique_ptr<SDL_Surface, decltype(&SDL_FreeSurface)>
    loadedSurface(IMG_Load( path.c_str() ), SDL_FreeSurface);

  if( loadedSurface == nullptr ) {
    printf( "Unable to load image %s! SDL_image Error: %s\n", path.c_str(), IMG_GetError() );
  }
  else {
    //Color key image
    SDL_SetColorKey( loadedSurface.get(), SDL_TRUE, SDL_MapRGB( loadedSurface->format, 0, 0xFF, 0xFF ) );

    //Create texture from surface pixels
    mTexture.reset(SDL_CreateTextureFromSurface( gRenderer.get(), loadedSurface.get() ));

    if( mTexture == nullptr ) {
      printf( "Unable to create texture from %s! SDL Error: %s\n", path.c_str(), SDL_GetError() );
    }
    else {
      //Get image dimensions
      mWidth = loadedSurface->w;
      mHeight = loadedSurface->h;
    }
  }

  //Return success
  return mTexture != nullptr;
}

void LTexture::free()
{
	//Free texture if it exists
	if( mTexture != nullptr )
	{
		mTexture.reset(nullptr);
		mWidth = 0;
		mHeight = 0;
	}
}

void LTexture::render( int x, int y )
{
	//Set rendering space and render to screen
	SDL_Rect renderQuad = { x, y, mWidth, mHeight };
	SDL_RenderCopy( gRenderer.get(), mTexture.get(), nullptr, &renderQuad );
}

int LTexture::getWidth()
{
	return mWidth;
}

int LTexture::getHeight()
{
	return mHeight;
}

bool init()
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
		gWindow.reset(SDL_CreateWindow( "SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN ));
		if( gWindow == nullptr )
		{
			printf( "Window could not be created! SDL Error: %s\n", SDL_GetError() );
			success = false;
		}
		else
		{
			//Create renderer for window
			gRenderer.reset(SDL_CreateRenderer( gWindow.get(), -1, SDL_RENDERER_ACCELERATED ));
			if( gRenderer == NULL )
			{
				printf( "Renderer could not be created! SDL Error: %s\n", SDL_GetError() );
				success = false;
			}
			else
			{
				//Initialize renderer color
				SDL_SetRenderDrawColor( gRenderer.get(), 0xFF, 0xFF, 0xFF, 0xFF );

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

bool loadMedia()
{
	//Loading success flag
	bool success = true;

	//Load Foo' texture
	if( !gFooTexture.loadFromFile( "foo.png" ) )
	{
		printf( "Failed to load Foo' texture image!\n" );
		success = false;
	}
	
	//Load background texture
	if( !gBackgroundTexture.loadFromFile( "background.png" ) )
	{
		printf( "Failed to load background texture image!\n" );
		success = false;
	}

	return success;
}

void close()
{
	//Free loaded images
	gFooTexture.free();
	gBackgroundTexture.free();

	//Quit SDL subsystems
	IMG_Quit();
	SDL_Quit();
}

int main( int argc, char* args[] )
{
	//Start up SDL and create window
	if( !init() )
	{
		printf( "Failed to initialize!\n" );
	}
	else
	{
		//Load media
		if( !loadMedia() )
		{
			printf( "Failed to load media!\n" );
		}
		else
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
				SDL_SetRenderDrawColor( gRenderer.get(), 0xFF, 0xFF, 0xFF, 0xFF );
				SDL_RenderClear( gRenderer.get() );

				//Render background texture to screen
				gBackgroundTexture.render( 0, 0 );

				//Render Foo' to the screen
				gFooTexture.render( 240, 190 );

				//Update screen
				SDL_RenderPresent( gRenderer.get() );
			}
		}
	}

	//Free resources and close SDL
	close();

	return 0;
}
