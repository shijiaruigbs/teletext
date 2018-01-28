#include <stdio.h>
#include <stdlib.h>
#include "neillsdl.h"

#define MINARGS 2
#define FIRSTARG 1

#define WIDTH 40
#define LNS 25
#define ASCIIDIFF 0x80

#define SPACE 0xA0
#define BLOCK 0xFF
#define ATCHAR 0xC0
#define HASHCHAR 0xDF

#define RED 0x81
#define GREEN 0x82
#define YELLOW 0x83
#define BLUE 0x84
#define MAGENTA 0x85
#define CYAN 0x86 
#define WHITE 0x87

#define GRED 0x91
#define GGREEN 0x92
#define GYELLOW 0x93
#define GBLUE 0x94
#define GMAGENTA 0x95
#define GCYAN 0x96 
#define GWHITE 0x97

#define MAX 255
#define MIN 0

#define HALF 2
#define DOUBLE 2
#define THIRD 3
#define ISODD 2
#define POSOFFSET 1
#define PRINTOFFSET 2
#define UNDEFINED 6

#define BLACKBACK 0x9C
#define NEWBACK 0x9D
#define SSIZE 0x8C
#define DSIZE 0x8D

#define OFF 0
#define ON 1
#define CONTGRAPH 0x99
#define SEPGRAPH  0x9A
#define CONT 1
#define SEP 2
#define HELDGRAPH 0x9E
#define RELGRAPH 0x9F

#define TOPLEFT 1
#define TOPRIGHT 2
#define MIDLEFT 3
#define MIDRIGHT 4
#define BOTLEFT 5
#define BOTRIGHT 7

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

void file_to_array(const char *filename, unsigned char a[LNS][WIDTH], int r, int c);
void render_codes(SDL_Simplewin sw, fntrow fontdata[FNTCHARS][FNTHEIGHT], unsigned char a[LNS][WIDTH], int r, int c);
void decoder(SDL_Simplewin sw, fntrow fontdata[FNTCHARS][FNTHEIGHT], unsigned char a[LNS][WIDTH], unsigned char cur, Colour *colours, Mode *mode, int i, int j, int ox, int oy);
void decode_print(SDL_Simplewin sw, fntrow fontdata[FNTCHARS][FNTHEIGHT], unsigned char a[LNS][WIDTH], unsigned char cur, Colour *colours, Mode *mode, int i, int j, int ox, int oy);
void decode_alphanum(SDL_Simplewin *sw, unsigned char a[LNS][WIDTH], unsigned char cur, Colour *colours, Mode *mode, int i, int j, int ox, int oy);
void decode_background(SDL_Simplewin *sw, unsigned char a[LNS][WIDTH], unsigned char cur, Colour *colours, int i, int j, int ox, int oy);
void decode_height(SDL_Simplewin sw, unsigned char a[LNS][WIDTH], unsigned char cur, Colour *colours, Mode *mode, int i, int j, int ox, int oy);
void decode_block_graphics(unsigned char a[LNS][WIDTH], SDL_Simplewin sw, fntrow fontdata[FNTCHARS][FNTHEIGHT], unsigned char cur, Colour *colours, Mode *mode, int i, int j, int ox, int oy);
void decode_sep_graphics(SDL_Simplewin sw, unsigned char cur, Colour *colours, Mode *mode, int ox, int oy);
void decode_held_graphics(SDL_Simplewin sw, fntrow fontdata[FNTCHARS][FNTHEIGHT], unsigned char cur, Colour *colours, Mode *mode, int ox, int oy);
void colour_pointer(unsigned char cur, unsigned char prev, unsigned char next, Colour *c);
void set_colours(unsigned char code, int *red, int *green, int *blue);
void reset_line(Colour *c, Mode *mode);
void black_background(Colour *c);
void set_height(unsigned char a[LNS][WIDTH], Mode *mode, int flag, int i, int j);

void SDL_DrawChar(SDL_Simplewin *sw, fntrow fontdata[FNTCHARS][FNTHEIGHT], unsigned char chr, int ox, int oy, Colour c, Mode m);
void Double_SDL_DrawChar(SDL_Simplewin *sw, fntrow fontdata[FNTCHARS][FNTHEIGHT], unsigned char chr, int ox, int oy, Colour c, Mode m);
void SDL_print_space(SDL_Simplewin *sw, Colour c, int ox, int oy);
void draw_sixel(SDL_Simplewin *sw, unsigned char chr, int ox, int oy, Colour c, Mode m);
void set_start_pos(int block, int ox, int oy, int *startx, int *starty, Mode m);
void fill_block(SDL_Simplewin *sw, int startx, int starty, Colour c, Mode m, int background);
