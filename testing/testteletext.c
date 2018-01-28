#include "teletext.h"
#include <assert.h>

int main(int argc, char const *argv[])
{
    SDL_Simplewin sw;   
    fntrow fntdata[FNTCHARS][FNTHEIGHT];
    unsigned char codes[LNS][WIDTH];

    printf("Beginning testing:\n");

    if(argc < MINARGS){
        fprintf(stderr, "Please provide the file to be rendered\n");
        exit(1);
    }

    Neill_SDL_ReadFont(fntdata, "m7fixed.fnt");

    file_to_array(argv[FIRSTARG], codes, LNS, WIDTH);

    Neill_SDL_Init(&sw);

    render_codes(sw, fntdata, codes, LNS, WIDTH);

    printf("Testing finished\n");

    return 0;
}

void file_to_array(const char *filename, unsigned char a[LNS][WIDTH], int r, int c)
{
    unsigned char curval;
    int rcnt, ccnt;
    FILE *source;

    if((source = fopen(filename, "r")) == NULL){
        fprintf(stderr, "Failed to open source file\n");
        exit(1);
    }

    for (rcnt = 0; rcnt < r; ++rcnt){
        for (ccnt = 0; ccnt < c; ++ccnt){
            fscanf(source, "%c", &curval);
            a[rcnt][ccnt] = curval;
        }
    }

    fclose(source);
}

void render_codes(SDL_Simplewin sw, fntrow fontdata[FNTCHARS][FNTHEIGHT], unsigned char a[LNS][WIDTH], int r, int c)
{
    int i, j, ox = 0, oy = 0, flag = 0;
    unsigned char cur;
    Colour colours;
    Mode mode;

    reset_line(&colours, &mode);
   /* mode.dbottom = OFF;*/ /* why is this line here?*/

    do{
        if(flag == 0){
            for (i = 0; i < r; ++i){
                for (j = 0; j < c; ++j){
                    cur = (a[i][j]);

                    decoder(sw, fontdata, a, cur, &colours, &mode, i, j, ox, oy);
                
                    ox = ox + FNTWIDTH; 
                }
                ox = 0;
                oy = oy + FNTHEIGHT;
                reset_line(&colours, &mode);
                /* test that new line starts with correct mode and colour configurations */
                assert(mode.dheight == OFF);
                assert(mode.dbottom == OFF);
                assert(mode.bgraphics == OFF);
                assert(mode.sgraphics == OFF);
                assert(mode.heldgraph == OFF);
                assert(colours.fred == MAX);
                assert(colours.fgreen == MAX);
                assert(colours.fblue == MAX);
                assert(colours.bred == MIN);
                assert(colours.bgreen == MIN);
                assert(colours.bblue == MIN);

            }
            flag = 1;
        }
        Neill_SDL_UpdateScreen(&sw);
        Neill_SDL_Events(&sw);
    }while(!sw.finished);
}

void decoder(SDL_Simplewin sw, fntrow fontdata[FNTCHARS][FNTHEIGHT], unsigned char a[LNS][WIDTH], unsigned char cur, Colour *colours, Mode *mode, int i, int j, int ox, int oy)
{
    decode_alphanum(&sw, a, cur, colours, mode, i, j, ox, oy);

    decode_background(&sw, a, cur, colours, i, j, ox, oy);

    decode_height(sw, a, cur, colours, mode, i, j, ox, oy);

    decode_block_graphics(a, sw, fontdata, cur, colours, mode, i, j, ox, oy);

    decode_sep_graphics(sw, cur, colours, mode, ox, oy);

    decode_held_graphics(sw, fontdata, cur, colours, mode, ox, oy);

    decode_print(sw, fontdata, a, cur, colours, mode, i, j, ox, oy);
}

void decode_print(SDL_Simplewin sw, fntrow fontdata[FNTCHARS][FNTHEIGHT], unsigned char a[LNS][WIDTH], unsigned char cur, Colour *colours, Mode *mode, int i, int j, int ox, int oy)
{
    if((cur >= SPACE && cur < BLOCK) || cur == BLOCK){
        cur = (a[i][j]-ASCIIDIFF); 
        SDL_DrawChar(&sw, fontdata, cur, ox, oy, *colours, *mode);        
        mode->prevcode = cur;


    }

    if(cur >= (SPACE - ASCIIDIFF) && cur <= (BLOCK - ASCIIDIFF)){
        SDL_DrawChar(&sw, fontdata, cur, ox, oy, *colours, *mode);        
        mode->prevcode = cur;
    } /*design decision: this implementation causes ascii codes to be printed as sixels when graphics mode is on */
}

void decode_alphanum(SDL_Simplewin *sw, unsigned char a[LNS][WIDTH], unsigned char cur, Colour *colours, Mode *mode, int i, int j, int ox, int oy)
{
    if(cur >= RED && cur <= WHITE){
        mode->bgraphics = OFF;
        mode->sgraphics = OFF;
        colour_pointer(cur, '\0', a[i][j+1], colours);
        /* design decision: held graphics is primarily for colour chages in graphics, so will not be implemented for regular text. */
        SDL_print_space(sw, *colours, ox, oy);
        
        /* test that graphics modes are correctly turned off by alphanumeric colour codes */
        assert(mode->bgraphics == OFF);
        assert(mode->bgraphics == OFF);
    }
}

void decode_background(SDL_Simplewin *sw, unsigned char a[LNS][WIDTH], unsigned char cur, Colour *colours, int i, int j, int ox, int oy)
{
    if(cur == NEWBACK){
        colour_pointer(cur, a[i][j-1], '\0', colours);
        SDL_print_space(sw, *colours, ox, oy);
    }

    if(cur == BLACKBACK){
        black_background(colours);
        SDL_print_space(sw, *colours, ox, oy);
    }
}

void decode_height(SDL_Simplewin sw, unsigned char a[LNS][WIDTH], unsigned char cur, Colour *colours, Mode *mode, int i, int j, int ox, int oy)
{
    int hflag;
    
    if(cur == SSIZE){
        hflag = 0;
        set_height(a, mode, hflag, i, j);
        SDL_print_space(&sw, *colours, ox, oy);
        
        /*test that double height mode is correctly turned off after single height code */
        assert(mode->dheight == OFF);
        assert(mode->dbottom == OFF);
    }

    if(cur == DSIZE){
        hflag = 1;
        set_height(a, mode, hflag, i, j);
        SDL_print_space(&sw, *colours, ox, oy);

        assert(mode->dheight == ON);
    }

    if(mode->dheight == ON && a[i-1][j] == DSIZE){
    mode->dheight = ON;
    mode->dbottom = ON;
    }
}

void decode_block_graphics(unsigned char a[LNS][WIDTH], SDL_Simplewin sw, fntrow fontdata[FNTCHARS][FNTHEIGHT], unsigned char cur, Colour *colours, Mode *mode, int i, int j, int ox, int oy)
{
    if(cur >= GRED && cur <= GWHITE){        
        if(mode->heldgraph == OFF){
            mode->bgraphics = ON;
            mode->sgraphics = OFF;
            mode->prevgraph = CONT;     
        } /* this implementation causes seperated graphics to return to contiguous graphics
            when a new graphics colour code is issued */

        colour_pointer(cur, '\0', a[i][j+1], colours);
        if(mode->heldgraph == ON){
            /*implements held graphics mode */
            if(mode->prevgraph == CONT){
                mode->sgraphics = OFF;
                mode->bgraphics = ON;
            }
            if(mode->prevgraph == SEP){
                mode->sgraphics = ON;
                mode->bgraphics = OFF;
            }
            SDL_DrawChar(&sw, fontdata, mode->prevcode, ox, oy, *colours, *mode); 
        }
        else{
            SDL_print_space(&sw, *colours, ox, oy);
        }
    }
}

void decode_sep_graphics(SDL_Simplewin sw, unsigned char cur, Colour *colours, Mode *mode, int ox, int oy)
{
    if(cur == SEPGRAPH){
        mode->sgraphics = ON;
        mode->bgraphics = OFF;
        mode->prevgraph = SEP;
        SDL_print_space(&sw, *colours, ox, oy);
    }

    if(cur == CONTGRAPH){
        mode->sgraphics = OFF;
        mode->bgraphics = ON;
        mode->prevgraph = CONT;
        SDL_print_space(&sw, *colours, ox, oy);
    }
}

void decode_held_graphics(SDL_Simplewin sw, fntrow fontdata[FNTCHARS][FNTHEIGHT], unsigned char cur, Colour *colours, Mode *mode, int ox, int oy)
{
    if(cur == HELDGRAPH){
        mode->heldgraph = ON;
        SDL_DrawChar(&sw, fontdata, mode->prevcode, ox, oy, *colours, *mode);

        assert(mode->heldgraph == ON);
    }

    if(cur == RELGRAPH){
        mode->heldgraph = OFF;
        mode->sgraphics = OFF;
        mode->bgraphics = OFF;
        SDL_print_space(&sw, *colours, ox, oy);

        assert(mode->heldgraph == OFF);
        assert(mode->sgraphics == OFF);
        assert(mode->bgraphics == OFF);
    }
}

void colour_pointer(unsigned char cur, unsigned char prev, unsigned char next, Colour *c)
{
    int *red, *green, *blue;

    if(((cur >= RED && cur <= WHITE) || (cur >= GRED && cur <= GWHITE)) && next != NEWBACK){
        red = &c->fred;
        green = &c->fgreen;
        blue = &c->fblue;

        assert(red != NULL);
        assert(green != NULL);
        assert(blue != NULL);

        set_colours(cur, red, green, blue);
    }

    if ((prev < RED || prev > WHITE) && (prev < GRED || prev > GWHITE))
    {
        prev = WHITE;

        assert(prev == WHITE);
    } /* New Background Colour with no colour code before it defaults to white background */

    if(cur == NEWBACK){
        red = &c->bred;
        green = &c->bgreen;
        blue = &c->bblue;

        assert(red != NULL);
        assert(green != NULL);
        assert(blue != NULL);

        set_colours(prev, red, green, blue);
    }    
}

void set_colours(unsigned char code, int *red, int *green, int *blue)
{
    if(code == RED || code == GRED){
        *red = MAX;
        *green = MIN;
        *blue = MIN;
    }

    if(code == GREEN || code == GGREEN){
        *red = MIN;
        *green = MAX;
        *blue = MIN;
    }

    if(code == YELLOW || code == GYELLOW){
        *red = MAX;
        *green = MAX;
        *blue = MIN;
    }

    if(code == BLUE || code == GBLUE){
        *red = MIN;
        *green = MIN;
        *blue = MAX;
    }

    if(code == MAGENTA || code == GMAGENTA){
        *red = MAX;
        *green = MIN;
        *blue = MAX;
    }

    if(code == CYAN || code == GCYAN){
        *red = MIN;
        *green = MAX;
        *blue = MAX;
    }

    if(code == WHITE || code == GWHITE){
        *red = MAX;
        *green = MAX;
        *blue = MAX;
    }
}

void black_background(Colour *c)
{
    c->bred = MIN;
    c->bgreen = MIN;
    c->bblue = MIN;

    assert(c->bred == MIN);
    assert(c->bgreen == MIN);
    assert(c->bblue == MIN);
}

void reset_line(Colour *c, Mode *mode)
{
    c->fred = MAX;
    c->fgreen = MAX;
    c->fblue = MAX;

    c->bred = MIN;
    c->bgreen = MIN;
    c->bblue = MIN;

    mode->bgraphics = OFF;
    mode->sgraphics = OFF;
    mode->heldgraph = OFF;


    mode->dheight = OFF;
    mode->dbottom = OFF;
}

void set_height(unsigned char a[LNS][WIDTH], Mode *mode, int flag, int i, int j)
{
    if((flag == 1) && (a[i-1][j] != DSIZE)){
        mode->dheight = ON;
        mode->dbottom = OFF;

        assert(mode->dheight == ON);
        assert(mode->dbottom == OFF);
        }

    if((flag == 1) && (a[i-1][j] == DSIZE)){
        mode->dheight = ON;
        mode->dbottom = ON;

        assert(mode->dheight == ON);
        assert(mode->dbottom == ON);
    }

    if(flag == 0){
        mode->dheight = OFF;
        mode->dbottom = OFF;

        assert(mode->dheight == OFF);
        assert(mode->dbottom == OFF);
    } 
}

void SDL_DrawChar(SDL_Simplewin *sw, fntrow fontdata[FNTCHARS][FNTHEIGHT], unsigned char chr, int ox, int oy, Colour c, Mode m)
{
   unsigned x, y;

   if((m.bgraphics == ON || m.sgraphics == ON) && (chr < (ATCHAR - ASCIIDIFF)  || chr > (HASHCHAR - ASCIIDIFF))){
    
    assert((m.bgraphics == ON || m.sgraphics == ON) && (chr < (ATCHAR - ASCIIDIFF)  || chr > (HASHCHAR - ASCIIDIFF)));
    
    draw_sixel(sw, chr, ox, oy, c, m);
    return;
   }

   if(m.dheight == ON){
    
    assert(m.dheight == ON);

    Double_SDL_DrawChar(sw, fontdata, chr, ox, oy, c, m);  
    return;
   }

   for(y = 0; y < FNTHEIGHT; y++){
      for(x = 0; x < FNTWIDTH; x++){
         if(fontdata[chr-FNT1STCHAR][y] >> (FNTWIDTH - 1 - x) & 1){
            Neill_SDL_SetDrawColour(sw, c.fred, c.fgreen, c.fblue);
            SDL_RenderDrawPoint(sw->renderer, x + ox, y+oy);
         }
         else{
            Neill_SDL_SetDrawColour(sw, c.bred, c.bgreen, c.bblue);
            SDL_RenderDrawPoint(sw->renderer, x + ox, y+oy);
         }
      }
   }
}

void SDL_print_space(SDL_Simplewin *sw, Colour c, int ox, int oy){
  unsigned x, y;
  for(y = 0; y < FNTHEIGHT; y++){
      for(x = 0; x < FNTWIDTH; x++){
            Neill_SDL_SetDrawColour(sw, c.bred, c.bgreen, c.bblue);
            SDL_RenderDrawPoint(sw->renderer, x + ox, y+oy);
         }
      }

}

void Double_SDL_DrawChar(SDL_Simplewin *sw, fntrow fontdata[FNTCHARS][FNTHEIGHT], unsigned char chr, int ox, int oy, Colour c, Mode m)
{
   unsigned x, y, ystart, yfinish;
  
   /* test that this function is only called when double height mode is on */
   assert(m.dheight == ON);

   if(m.dheight == ON && m.dbottom == OFF){
      ystart = 0;
      yfinish = FNTHEIGHT;
      for(y = ystart; y < (yfinish); y++){
        for(x = 0; x < (FNTWIDTH); x++){
           if(fontdata[chr-FNT1STCHAR][y/HALF] >> (FNTWIDTH - 1 - x) & 1){
              Neill_SDL_SetDrawColour(sw, c.fred, c.fgreen, c.fblue);
              SDL_RenderDrawPoint(sw->renderer, x + ox, y+oy);
           }
           else{
              Neill_SDL_SetDrawColour(sw, c.bred, c.bgreen, c.bblue);
              SDL_RenderDrawPoint(sw->renderer, x + ox, y+oy);
           }
        }
     }
   }

   else if(m.dheight == ON && m.dbottom == ON){
      ystart = FNTHEIGHT;
      yfinish = FNTHEIGHT*DOUBLE;
      for(y = ystart; y < (yfinish); y++){
        for(x = 0; x < (FNTWIDTH); x++){
           if(fontdata[chr-FNT1STCHAR][y/HALF] >> (FNTWIDTH - 1 - x) & 1){
              Neill_SDL_SetDrawColour(sw, c.fred, c.fgreen, c.fblue);
              SDL_RenderDrawPoint(sw->renderer, x + ox, y+oy-FNTHEIGHT);
           }
           else{
              Neill_SDL_SetDrawColour(sw, c.bred, c.bgreen, c.bblue);
              SDL_RenderDrawPoint(sw->renderer, x + ox, y+oy-FNTHEIGHT);
           }
        }
      }
   } 

}

void draw_sixel(SDL_Simplewin *sw, unsigned char chr, int ox, int oy, Colour c, Mode m)
{
  int i, startx, starty, background;

  for (i = TOPLEFT; i <= BOTRIGHT; ++i){
    if(((chr >> (i-1)) % ISODD) == 1 && i != UNDEFINED){
      set_start_pos(i, ox, oy, &startx, &starty, m);
      background = 0;
      fill_block(sw, startx, starty, c, m, background);      
    }
  }
  
  for (i = TOPLEFT; i <= BOTRIGHT; ++i){
    if(((chr >> (i-1)) % ISODD) == 0 && i != UNDEFINED){
      set_start_pos(i, ox, oy, &startx, &starty, m);
      background = 1;
      fill_block(sw, startx, starty, c, m, background);      
    }
  }
}

void set_start_pos(int block, int ox, int oy, int *startx, int *starty, Mode m)
{
  int p = 0;

  if(m.sgraphics == ON){
    p = POSOFFSET;
  }

  if(block == TOPLEFT){
    *startx = ox + p;
    *starty = oy + p;
  }

  if(block == TOPRIGHT){
    *startx = ox + (FNTWIDTH/HALF) + p;
    *starty = oy + p;
  }

  if(block == MIDLEFT){
    *startx = ox + p;
    *starty = oy + (FNTHEIGHT/THIRD) + p;
  }

  if(block == MIDRIGHT){
    *startx = ox + (FNTWIDTH/HALF) + p;
    *starty = oy + (FNTHEIGHT/THIRD) + p;
  }

  if(block == BOTLEFT){
    *startx = ox + p;
    *starty = oy + (DOUBLE * (FNTHEIGHT / THIRD)) + p;
  }

  if(block == BOTRIGHT){
    *startx = ox + (FNTWIDTH/DOUBLE) + p;
    *starty = oy + (DOUBLE * (FNTHEIGHT / THIRD)) + p;
  }
}

void fill_block(SDL_Simplewin *sw, int startx, int starty, Colour c, Mode m, int background)
{
  int y, p = 0;
  unsigned long x;

  if(m.sgraphics == ON){
    p = PRINTOFFSET;
  }

  if(background == 0){
    for (x = startx; x <  (startx + (FNTWIDTH/HALF) - p); ++x){
      for (y = starty; y < (starty + (FNTHEIGHT/THIRD) - p); ++y){
        Neill_SDL_SetDrawColour(sw, c.fred, c.fgreen, c.fblue);
        SDL_RenderDrawPoint(sw->renderer, x, y);
      }
    }
  }

  if(background == 1){
    for (x = startx; x <  (startx + (FNTWIDTH/HALF)); ++x){
      for (y = starty; y < (starty + (FNTHEIGHT/THIRD)); ++y){
        Neill_SDL_SetDrawColour(sw, c.bred, c.bgreen, c.bblue);
        SDL_RenderDrawPoint(sw->renderer, x, y);
      }
    }
  }
}






