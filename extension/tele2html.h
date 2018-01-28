#include <stdio.h>
#include <stdlib.h>

#define WIDTH 40
#define LNS 25
#define ASCIIDIFF 0x80

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
}Mode;   

void file_to_array(const char *filename, unsigned char a[LNS][WIDTH], int r, int c);
void render_codes(unsigned char a[LNS][WIDTH], int r, int c, FILE *fp);
void decoder(unsigned char a[LNS][WIDTH], unsigned char cur, int i, int j, Mode *mode, FILE *fp);
void decode_print(unsigned char a[LNS][WIDTH], unsigned char cur, int i, int j, FILE *fp, Mode *mode);
void decode_alphanum(unsigned char cur, Mode *mode, FILE *fp);
void decode_graphics(unsigned char cur, Mode *mode);
void decode_colours(FILE *fp, unsigned char code);
void decode_background(FILE *fp, unsigned char a[LNS][WIDTH], int i, int j);
void reset_line(FILE *fp, Mode *mode);
