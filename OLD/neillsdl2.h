#include <SDL2/SDL.h>
#include <math.h>

#define WWIDTH 800
#define WHEIGHT 600

/* Font stuff */
typedef unsigned short fntrow;
#define FNTWIDTH (sizeof(fntrow)*8)
#define FNTHEIGHT 18
#define FNTCHARS 96
#define FNT1STCHAR 32

#define SDL_8BITCOLOUR 256

/* All info required for windows / renderer & event loop */
struct SDL_Simplewin {
   SDL_bool finished;
   SDL_Window *win;
   SDL_Renderer *renderer;
   SDL_Texture *display;
};
typedef struct SDL_Simplewin SDL_Simplewin;

typedef struct colour{
    int fred;
    int fgreen;
    int fblue;

    int bred;
    int bgreen;
    int bblue;
} Colour;

typedef struct mode
{
  int dheight;
  int dbottom;
  int bgraphics;
  int sgraphics;
  int heldgraph;
  int prevgraph;
  unsigned char prevcode;  

} Mode;

void Neill_SDL_Init(SDL_Simplewin *sw);
void Neill_SDL_Events(SDL_Simplewin *sw);
void Neill_SDL_SetDrawColour(SDL_Simplewin *sw, Uint8 r, Uint8 g, Uint8 b);
void Neill_SDL_DrawChar(SDL_Simplewin *sw, fntrow fontdata[FNTCHARS][FNTHEIGHT], unsigned char chr, int ox, int oy, Colour c, Mode m);
void Neill_SDL_ReadFont(fntrow fontdata[FNTCHARS][FNTHEIGHT], char *fname);
void Neill_SDL_UpdateScreen(SDL_Simplewin *sw);
void Double_SDL_DrawChar(SDL_Simplewin *sw, fntrow fontdata[FNTCHARS][FNTHEIGHT], unsigned char chr, int ox, int oy, Colour c, Mode m);
void draw_sixel(SDL_Simplewin *sw, unsigned char chr, int ox, int oy, Colour c, Mode m);
void set_start_pos(int block, int ox, int oy, int *startx, int *starty, Mode m);
void fill_block(SDL_Simplewin *sw, int startx, int starty, Colour c, Mode m);