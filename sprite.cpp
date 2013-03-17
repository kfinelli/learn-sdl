//stl includes
#include <string>
#include <iostream>

//sdl includes
#include "SDL/SDL.h"
#include "SDL/SDL_image.h"

//attributes of the screen
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;
const int SCREEN_BPP = 32;

//surfaces to be used
SDL_Surface *image = 0;
//SDL_Surface *background = 0;
SDL_Surface *screen = 0;

//the event structure
SDL_Event event;

//portions of the sprite map to be blitted
SDL_Rect clip[4];

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

  //set window caption
  SDL_WM_SetCaption("Hello, World", 0);

  return true;
}

bool load_files() {
//load image
  image = load_image("dots.png");
  if (!image)
    return false;

  // background = load_image("background.bmp");
  // if (!image)
  //   return false;


  return true;
}

void clean_up() {
  //Free loaded image
  SDL_FreeSurface(image);
  //  SDL_FreeSurface(background);

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

  //top left
  clip[0].x = 0;
  clip[0].y = 0;
  clip[0].w = 100;
  clip[0].h = 100;

  //top right
  clip[1].x = 100;
  clip[1].y = 0;
  clip[1].w = 100;
  clip[1].h = 100;

  //bottom left
  clip[2].x = 0;
  clip[2].y = 100;
  clip[2].w = 100;
  clip[2].h = 100;

  //bottom right
  clip[3].x = 100;
  clip[3].y = 100;
  clip[3].w = 100;
  clip[3].h = 100;

  SDL_FillRect(screen, &screen->clip_rect, SDL_MapRGB(screen->format, 0xFF, 0xFF, 0xFF) );

  //apply image to screen
  //  apply_surface(0, 0, background, screen);
  apply_surface(0, 0, image, screen, &clip[0]);
  apply_surface(540, 0, image, screen, &clip[1]);
  apply_surface(0, 380, image, screen, &clip[2]);
  apply_surface(540, 380, image, screen, &clip[3]);

  //update screen
  if ( SDL_Flip(screen) == -1) {
    std::cerr << "error flipping screen" << std::endl;
    return 1;
  }

  //Pause
  while(!quit) {
    while( SDL_PollEvent(&event) ) {
      //if user quits
      if(event.type==SDL_QUIT) {
	//quit program
	quit = true;
      }
    }
  }
  clean_up();

  return 0;    
}
