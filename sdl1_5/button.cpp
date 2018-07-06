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

//clip regions
enum {CLIP_MOUSEOVER, CLIP_MOUSEOUT, CLIP_MOUSEDOWN, CLIP_MOUSEUP, NCLIPSTATES};
SDL_Rect clips[NCLIPSTATES];

//surfaces to be used
SDL_Surface *background = 0;
SDL_Surface *screen = 0;
SDL_Surface *buttonSheet = 0;

//the event structure
SDL_Event event;


void apply_surface(int x, int y, SDL_Surface* source, SDL_Surface* destination, SDL_Rect* clip=0) {
//make a temp rectangle to hold offsets
  SDL_Rect offset;
  offset.x = x;
  offset.y = y;

  SDL_BlitSurface( source, clip, destination, &offset);
  return;
}

class Button {
private:
  SDL_Rect box;
  SDL_Rect* clip;
public:
  Button( int x, int y, int w, int h);
  //handles events and set the button's sprite region
  void handle_events();
  void show();
};

Button::Button(int x, int y, int w, int h) {
  box.x = x;
  box.y = y;
  box.w = w;
  box.h = h;

  clip = &clips[CLIP_MOUSEOUT];
}

void Button::handle_events() {
  //mouse offsets
  int x = 0, y = 0;
  //if mouse moved
  if(event.type==SDL_MOUSEMOTION) {
    //get mouse offsets
    x = event.motion.x;
    y = event.motion.y;

    //if the mouse is over the button
    if( (x > box.x) && (x < box.x + box.w) && (y > box.y) && (y < box.y + box.h) ) {
      //set button sprite
      clip = &clips[CLIP_MOUSEOVER];
    } else {
      clip = &clips[CLIP_MOUSEOUT];
    }
  }
  if (event.type==SDL_MOUSEBUTTONDOWN) {
    //if left mouse button
    if(event.button.button==SDL_BUTTON_LEFT) {
      //get mouse offsets
      x = event.button.x;
      y = event.button.y;
      //if the mouse is over the button
      if( (x > box.x) && (x < box.x + box.w) && (y > box.y) && (y < box.y + box.h) ) {
	//set button sprite
	clip = &clips[CLIP_MOUSEDOWN];
      }
    }
  }
  if (event.type==SDL_MOUSEBUTTONUP) {
    //if left mouse button was released
    if(event.button.button == SDL_BUTTON_LEFT) {
      //get mouse offsets
      x = event.button.x;
      y = event.button.y;
      //if the mouse is over the button
      if( (x > box.x) && (x < box.x + box.w) && (y > box.y) && (y < box.y + box.h) ) {
	//set button sprite
	clip = &clips[CLIP_MOUSEUP];
      }
    }
  }
}

void Button::show() {
  //show the button
  apply_surface(box.x, box.y, buttonSheet, screen, clip);
}

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


void set_clips() {
  clips[ CLIP_MOUSEOVER ].x = 0;
  clips[ CLIP_MOUSEOVER ].y = 0;
  clips[ CLIP_MOUSEOVER ].w = 320;
  clips[ CLIP_MOUSEOVER ].h = 240;

  clips[ CLIP_MOUSEOUT ].x = 320;
  clips[ CLIP_MOUSEOUT ].y = 0;
  clips[ CLIP_MOUSEOUT ].w = 320;
  clips[ CLIP_MOUSEOUT ].h = 240;

  clips[ CLIP_MOUSEDOWN ].x = 0;
  clips[ CLIP_MOUSEDOWN ].y = 240;
  clips[ CLIP_MOUSEDOWN ].w = 320;
  clips[ CLIP_MOUSEDOWN ].h = 240;
    
  clips[ CLIP_MOUSEUP ].x = 320;
  clips[ CLIP_MOUSEUP ].y = 240;
  clips[ CLIP_MOUSEUP ].w = 320;
  clips[ CLIP_MOUSEUP ].h = 240;
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
  SDL_WM_SetCaption("button Test", 0);

  return true;
}

bool load_files() {
//load image
  background = load_image("background.bmp");
  if (!background)
    return false;

  buttonSheet = load_image("button.png");
  if (!background)
    return false;

  return true;
}

void clean_up() {
  //Free loaded image
  SDL_FreeSurface(background);
  SDL_FreeSurface(buttonSheet);

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

  set_clips();

  Button myButton(170, 120, 320, 240);

  //display background
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
      //handle buton events
      myButton.handle_events();

      //if user quits
      if(event.type==SDL_QUIT) {
	//quit program
	quit = true;
      }
    }
    apply_surface(0, 0, background, screen);
    apply_surface(320, 0, background, screen);
    apply_surface(0, 240, background, screen);
    apply_surface(320, 240, background, screen);
    myButton.show();

    //update screen
    if ( SDL_Flip(screen) == -1) {
      std::cerr << "error flipping screen" << std::endl;
      return 1;
    }


  }
  clean_up();

  return 0;    
}
