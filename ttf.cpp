//stl includes
#include <string>
#include <iostream>

//sdl includes
#include "SDL/SDL.h"
#include "SDL/SDL_image.h"
#include "SDL/SDL_ttf.h"

//attributes of the screen
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;
const int SCREEN_BPP = 32;

//surfaces to be used
SDL_Surface *background = 0;
SDL_Surface *screen = 0;
SDL_Surface *message = 0;
SDL_Surface *upMessage = 0;
SDL_Surface *downMessage = 0;
SDL_Surface *leftMessage = 0;
SDL_Surface *rightMessage = 0;

//the event structure
SDL_Event event;

//portions of the sprite map to be blitted
TTF_Font *font = 0;

SDL_Color textColor = {100, 100, 100};

SDL_Surface *load_image( std::string filename ) {
  //temporary storage for the image loaded
  SDL_Surface* loadedImage = 0;

  //optimized image that will be used
  SDL_Surface* optimizedImage = 0;

  //load image
  loadedImage = IMG_Load( filename.c_str() );

  //if nothing went wrong in loading the image
  if (loadedImage) {
    //create an optimized image
    optimizedImage = SDL_DisplayFormat(loadedImage);

    //free the old image
    SDL_FreeSurface(loadedImage);

    if (optimizedImage) {
      //map the color key
      Uint32 colorkey = SDL_MapRGB( optimizedImage->format, 0, 0xFF, 0xFF);
      //set transparency
      SDL_SetColorKey( optimizedImage, SDL_SRCCOLORKEY, colorkey);
    }
  }
  return optimizedImage;
}

void apply_surface(int x, int y, SDL_Surface* source, SDL_Surface* destination, SDL_Rect* clip=0) {
//make a temp rectangle to hold offsets
  SDL_Rect offset;
  offset.x = x;
  offset.y = y;

  SDL_BlitSurface( source, clip, destination, &offset);
  return;
}

bool init() {
  //Start SDL
  if (SDL_Init(SDL_INIT_EVERYTHING) == -1)
    return false;

  //set the screen
  screen = SDL_SetVideoMode( SCREEN_WIDTH, SCREEN_HEIGHT,
			     SCREEN_BPP, SDL_SWSURFACE );
  if (!screen)
    return false;

  //initialize SDL_ttf
  if(TTF_Init() == -1)
    return false;

  //set window caption
  SDL_WM_SetCaption("TTF Test", 0);

  return true;
}

bool load_files() {
//load image
  background = load_image("background.bmp");
  if (!background)
    return false;

  //open the font
  font = TTF_OpenFont("lazy.ttf", 28);

  if (!font)
    return false;

  return true;
}

void clean_up() {
  //Free loaded image
  SDL_FreeSurface(background);
  SDL_FreeSurface(upMessage);
  SDL_FreeSurface(downMessage);
  SDL_FreeSurface(leftMessage);
  SDL_FreeSurface(rightMessage);

  //close the font
  TTF_CloseFont(font);

  //quit ttf
  TTF_Quit();

  //Quit SDL
  SDL_Quit();
  return;
}

int main(int argc, char* args[]) {

  //make sure the program waits for a quit
  bool quit = false;

  if (!init()) {
    std::cerr << "failed to initialize SDL" << std::endl;
    return 1;
  }

  //load files
  if(!load_files()) {
    std::cerr << "failed to load images" << std::endl;
    return 1;
  }

  upMessage = TTF_RenderText_Solid(font, "Up was pressed.", textColor);
  downMessage = TTF_RenderText_Solid(font, "Down was pressed.", textColor);
  leftMessage = TTF_RenderText_Solid(font, "Left was pressed.", textColor);
  rightMessage = TTF_RenderText_Solid(font, "Right was pressed.", textColor);

  //being lazy about error checking
  if(!upMessage) {
    std::cerr << "failed to create ttf message" << std::endl;
    return 1;
  }

      apply_surface(0, 0, background, screen);
      apply_surface(320, 0, background, screen);
      apply_surface(0, 240, background, screen);
      apply_surface(320, 240, background, screen);
    //update screen
    if ( SDL_Flip(screen) == -1) {
      std::cerr << "error flipping screen" << std::endl;
      return 1;
    }


  //Pause
  while(!quit) {
    while( SDL_PollEvent(&event) ) {
      //get keystates
      Uint8 *keystates = SDL_GetKeyState(0);

      if ( keystates[SDLK_UP] ) message = upMessage;
      else if ( keystates[SDLK_DOWN] ) message = downMessage;
      else if ( keystates[SDLK_LEFT] ) message = leftMessage;
      else if ( keystates[SDLK_RIGHT] ) message = rightMessage;

      //if user quits
      if(event.type==SDL_QUIT) {
	//quit program
	quit = true;
      }
    }
    if(message) {
      apply_surface(0, 0, background, screen);
      apply_surface(320, 0, background, screen);
      apply_surface(0, 240, background, screen);
      apply_surface(320, 240, background, screen);
      apply_surface((SCREEN_WIDTH - message->w)/2, (SCREEN_HEIGHT - message->h)/2, message, screen);
      message = 0;
    }
    //update screen
    if ( SDL_Flip(screen) == -1) {
      std::cerr << "error flipping screen" << std::endl;
      return 1;
    }


  }
  clean_up();

  return 0;    
}
