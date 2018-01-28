#include "mysdl.h"

/* Set up a simple (WIDTH, HEIGHT) window.
   Attempt to hide the renderer etc. from user. */
void Neill_SDL_Init(SDL_Simplewin *sw)
{

   if (SDL_Init(SDL_INIT_VIDEO) != 0) {
      fprintf(stderr, "\nUnable to initialize SDL:  %s\n", SDL_GetError());
      SDL_Quit();
      exit(1);
   } 

   sw->finished = 0;
   
   sw->win= SDL_CreateWindow("SDL Window",
                          SDL_WINDOWPOS_UNDEFINED,
                          SDL_WINDOWPOS_UNDEFINED,
                          WWIDTH, WHEIGHT,
                          SDL_WINDOW_SHOWN);
   if(sw->win == NULL){
      fprintf(stderr, "\nUnable to initialize SDL Window:  %s\n", SDL_GetError());
      SDL_Quit();
      exit(1);
   }

   sw->renderer = SDL_CreateRenderer(sw->win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE);
   if(sw->renderer == NULL){
      fprintf(stderr, "\nUnable to initialize SDL Renderer:  %s\n", SDL_GetError());
      SDL_Quit();
      exit(1);
   }

   SDL_SetRenderDrawBlendMode(sw->renderer,SDL_BLENDMODE_BLEND);

   /* Create texture for display */
   sw->display = SDL_CreateTexture(sw->renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, WWIDTH, WHEIGHT);
   if(sw->display == NULL){
      fprintf(stderr, "\nUnable to initialize SDL texture:  %s\n", SDL_GetError());
      SDL_Quit();
      exit(1);
   }
   SDL_SetRenderTarget(sw->renderer, sw->display);

   /* Clear screen, & set draw colour to black */
   SDL_SetRenderDrawColor(sw->renderer, 0, 0, 0, 255);
   SDL_RenderClear(sw->renderer);

}

/* Housekeeping to do with the render buffer & updating the screen */
void Neill_SDL_UpdateScreen(SDL_Simplewin *sw)
{
   SDL_SetRenderTarget(sw->renderer, NULL);
   SDL_RenderCopy(sw->renderer, sw->display, NULL, NULL);
   SDL_RenderPresent(sw->renderer);
   SDL_SetRenderTarget(sw->renderer, sw->display);
}

/* Gobble all events & ignore most */
void Neill_SDL_Events(SDL_Simplewin *sw)
{
   SDL_Event event;
   while(SDL_PollEvent(&event)) 
   {      
       switch (event.type){
          case SDL_QUIT:
          case SDL_MOUSEBUTTONDOWN:
          case SDL_KEYDOWN:
             sw->finished = 1;
       }
    }
}

/* Trivial wrapper to avoid complexities of renderer & alpha channels */
void Neill_SDL_SetDrawColour(SDL_Simplewin *sw, Uint8 r, Uint8 g, Uint8 b)
{
   SDL_SetRenderDrawColor(sw->renderer, r, g, b, SDL_ALPHA_OPAQUE);
}

void Neill_SDL_DrawChar(SDL_Simplewin *sw, fntrow fontdata[FNTCHARS][FNTHEIGHT], unsigned char chr, int ox, int oy, Colour c, Mode m)
{
   unsigned x, y;

   if((m.bgraphics == 1 || m.sgraphics == 1) && (chr < (0xC0 - 0x80/*ASCIIDIFF*/)  || chr > (0xDF - 0x80/*ASCIDIFF*/) )){
    draw_sixel(sw, chr, ox, oy, c, m);
    return;
   }

   if(m.dheight == 1){
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

void Double_SDL_DrawChar(SDL_Simplewin *sw, fntrow fontdata[FNTCHARS][FNTHEIGHT], unsigned char chr, int ox, int oy, Colour c, Mode m)
{
   unsigned x, y, ystart, yfinish;
  
   //printf("dheight: %d\n", m.dheight);
   //printf("dbottom: %d\n", m.dbottom);
   int y1=0;
   if(m.dheight == 1 && m.dbottom == 0){
      ystart = 0;
      yfinish = FNTHEIGHT / 2;
   }

   if(m.dheight == 1 && m.dbottom == 1){
      ystart = FNTHEIGHT / 2;
      yfinish = FNTHEIGHT;
   }  

   for(y = ystart; y < (yfinish); y++){
      for(x = 0; x < (FNTWIDTH); x++){
         if(fontdata[chr-FNT1STCHAR][y] >> (FNTWIDTH - 1 - x) & 1){
            Neill_SDL_SetDrawColour(sw, c.fred, c.fgreen, c.fblue);
            SDL_RenderDrawPoint(sw->renderer, x + ox, y+oy);
            //SDL_RenderDrawPoint(sw->renderer, x + ox, y1+oy+1);
            //y1++;
         }
         else{
            Neill_SDL_SetDrawColour(sw, c.bred, c.bgreen, c.bblue);
            SDL_RenderDrawPoint(sw->renderer, x + ox, y+oy);
            //SDL_RenderDrawPoint(sw->renderer, x + ox, y1+1+oy);
            //y1++;
         }
      }
   }

}

void draw_sixel(SDL_Simplewin *sw, unsigned char chr, int ox, int oy, Colour c, Mode m)
{
  int i, startx, starty;

  for (i = 1; i <= 7 /*magic number*/ ; ++i){
    if(((chr >> (i-1)) % 2) == 1 && i != 6){
      set_start_pos(i, ox, oy, &startx, &starty, m);
      fill_block(sw, startx, starty, c, m);      
    }
  }
  /* my three sixel functions will print foreground colours, but will not print background colours */
}

void set_start_pos(int block, int ox, int oy, int *startx, int *starty, Mode m)
{
  int p = 0, q = 0;

  if(m. sgraphics == 1){
    p = 1;
    q = 2;
  }

  if(block == 1){
    *startx = ox + p;
    *starty = oy + p;
  }

  if(block == 2){
    *startx = ox + (FNTWIDTH/2) + p;
    *starty = oy + p;
  }

  if(block == 3){
    *startx = ox + p;
    *starty = oy + (FNTHEIGHT/3) + p;
  }

  if(block == 4){
    *startx = ox + (FNTWIDTH/2) + p;
    *starty = oy + (FNTHEIGHT/3) + p;
  }

  if(block == 5){
    *startx = ox + p;
    *starty = oy + (2 * (FNTHEIGHT / 3)) + p;
  }

  if(block == 7){
    *startx = ox + (FNTWIDTH/2) + p;
    *starty = oy + (2 * (FNTHEIGHT / 3)) + p;
  }
}

void fill_block(SDL_Simplewin *sw, int startx, int starty, Colour c, Mode m)
{
  int x, y, p = 0;

  if(m.sgraphics == 1){
    p = 2;
  }

  for (x = startx; x <  (startx + (FNTWIDTH/2) - p/* magic number*/); ++x){
    for (y = starty; y < (starty + (FNTHEIGHT/3) - p); ++y){
      Neill_SDL_SetDrawColour(sw, c.fred, c.fgreen, c.fblue);
      SDL_RenderDrawPoint(sw->renderer, x, y);
    }
  }
}

void Neill_SDL_ReadFont(fntrow fontdata[FNTCHARS][FNTHEIGHT], char *fname)
{
    FILE *fp = fopen(fname, "rb");
    size_t itms;
    if(!fp){
       fprintf(stderr, "Can't open Font file %s\n", fname);
       exit(1);
   }
   itms = fread(fontdata, sizeof(fntrow), FNTCHARS*FNTHEIGHT, fp);
   if(itms != FNTCHARS*FNTHEIGHT){
       fprintf(stderr, "Can't read all Font file %s (%d) \n", fname, (int)itms);
       exit(1);
   }
   fclose(fp);
}
