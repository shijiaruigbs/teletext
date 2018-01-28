void render_codes(SDL_Simplewin *sw, fntrow fontdata[FNTCHARS][FNTHEIGHT], unsigned char a[LNS][WIDTH], int r, int c)
{
    int i, j;
    int ox = 0, oy = 0;

    for (i = 0; i < r; ++i)
    {
        for (j = 0; j < c; ++j)
        {
            if(a[i][j]<=0x9f){
                Neill_SDL_DrawChar(sw, fontdata, " ", ox, oy);
            }

            if(a[i][j]>=0xa0 && a[i][j] <= 0xff){
                Neill_SDL_DrawChar(sw, fontdata, (a[i][j] - 0x80), ox, oy);
            }
        }
        printf("\n");
    }
    printf("\n");
}

void render_codes(unsigned char a[LNS][WIDTH], int r, int c)
{
    int i, j;

    for (i = 0; i < r; ++i)
    {
        for (j = 0; j < c; ++j)
        {
            if(a[i][j]<=0x9f){
                printf(" ");
            }

            if(a[i][j]>=0xa0 && a[i][j] <= 0xff){
                printf("%c", (a[i][j] - 0x80));
            }
        }
        printf("\n");
    }
    printf("\n");
}