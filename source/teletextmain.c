#include "teletext.h"

int main(int argc, char const *argv[])
{
    SDL_Simplewin sw;   
    fntrow fntdata[FNTCHARS][FNTHEIGHT];
    unsigned char codes[LNS][WIDTH];

    if(argc < MINARGS){
        fprintf(stderr, "Please provide the file to be rendered\n");
        exit(1);
    }

    Neill_SDL_ReadFont(fntdata, "m7fixed.fnt");

    file_to_array(argv[FIRSTARG], codes, LNS, WIDTH);

    Neill_SDL_Init(&sw);

    render_codes(sw, fntdata, codes, LNS, WIDTH);

    return 0;
}
