#include "tele2html.h"

int main(int argc, char const *argv[])
{
    unsigned char codes[LNS][WIDTH];
    FILE *fp;

    if(argc < 2){
        printf("Please provide the file to be rendered\n");
        exit(1);
    }

    if((fp = fopen("webversion.html", "w"))==NULL){
        printf("Failed to create html file\n");
        exit(1);
    }

    fprintf(fp, "<!DOCTYPE html> \n <html> \n <head> \n <title> \n %s html version \n </title> \n </head> \n", argv[1]);

    fprintf(fp,"<body>\n<p>\n<pre>\n");

    file_to_array(argv[1], codes, LNS, WIDTH);

    render_codes(codes, LNS, WIDTH, fp);

    fprintf(fp, "</pre>\n</p>\n</body>\n</html>\n");

    fclose(fp);


    return 0;
}

void file_to_array(const char *filename, unsigned char a[LNS][WIDTH], int r, int c)
{
    unsigned char curval;
    int rcnt, ccnt;
    FILE *source;

    if((source = fopen(filename, "r")) == NULL){
        printf("Failed to open source file\n");
        exit(1);
    }

    for (rcnt = 0; rcnt < r; ++rcnt){
        for (ccnt = 0; ccnt < c; ++ccnt){
            fscanf(source, "%c", &curval); /*what about EOF, preventing infinite loops etc?*/
            a[rcnt][ccnt] = curval;
        }
    }

    fclose(source);

}

void render_codes(unsigned char a[LNS][WIDTH], int r, int c, FILE *fp)
{
    int i, j;
    unsigned char cur;
    Mode mode;
    reset_line(fp, &mode);

        for (i = 0; i < r; ++i){
            for (j = 0; j < c; ++j){
                cur = (a[i][j]); /* Do I really need this line? */
                decoder(a, cur, i, j, &mode, fp);        
            }
            reset_line(fp, &mode);
            /*fprintf(fp, "\n");*/
        }
}

void decoder(unsigned char a[LNS][WIDTH], unsigned char cur, int i, int j, Mode *mode, FILE *fp)
{

    decode_print(a, cur, i, j, fp, mode);
    decode_alphanum(cur, mode, fp);
    decode_graphics(cur, mode);
    decode_background(fp, a, i, j);
}

void decode_print(unsigned char a[LNS][WIDTH], unsigned char cur, int i, int j, FILE *fp, Mode *mode)
{
    if((cur >= 0xa0 && cur <= 0xff) && (mode->bgraphics == OFF) && (mode->sgraphics == OFF)){
        cur = (a[i][j] - ASCIIDIFF);
        fprintf(fp, "%c", cur);
    }
}

void decode_alphanum(unsigned char cur, Mode *mode, FILE *fp)
{
    if(cur >= RED && cur <= WHITE){
        mode->bgraphics = OFF; 
        mode->sgraphics = OFF;
        fprintf(fp, " ");
        decode_colours(fp, cur);
    }
}

void decode_graphics(unsigned char cur, Mode *mode)
{
    if(cur >= GRED && cur <= GWHITE){        
        mode->bgraphics = ON;
        mode->sgraphics = OFF;
        mode->prevgraph = CONT;
        if(mode->heldgraph == ON){
            /*implements held graphics mode */
            if(mode->prevgraph == CONT){
                mode->sgraphics = OFF;
                mode->bgraphics = ON;
            }
            else{
                mode->sgraphics = ON;
                mode->bgraphics = OFF;
            }
        }
    }

    if(cur == SEPGRAPH){
        mode->sgraphics = ON;
        mode->bgraphics = OFF;
        mode->prevgraph = SEP;
    }

    if(cur == CONTGRAPH){
        mode->sgraphics = OFF;
        mode->bgraphics = ON;
        mode->prevgraph = CONT;
    }

    if(cur == HELDGRAPH){
        mode->heldgraph = ON;
    }

    if(cur == RELGRAPH){
        mode->heldgraph = OFF;
        mode->sgraphics = OFF;
        mode->bgraphics = OFF;
    }
}

void reset_line(FILE *fp, Mode *mode)
{
    mode->bgraphics = OFF;
    mode->sgraphics = OFF;
    fprintf(fp, "</pre><span style=\"color:black\">\n<pre>");
}

void decode_colours(FILE *fp, unsigned char code)
{
    if(code == RED){
        fprintf(fp, "</span><span style=\"color:red\">");
    }

    if(code == GREEN){
        fprintf(fp, "</span><span style=\"color:green\">");
    }

    if(code == YELLOW || code == GYELLOW){
        fprintf(fp, "</span><span style=\"color:Yellow\">");
    }

    if(code == BLUE || code == GBLUE){
        fprintf(fp, "</span><span style=\"color:blue\">");
    }

    if(code == MAGENTA || code == GMAGENTA){
        fprintf(fp, "</span><span style=\"color:magenta\">");
    }

    if(code == CYAN || code == GCYAN){
        fprintf(fp, "</span><span style=\"color:cyan\">");
    }

    if(code == WHITE || code == GWHITE){
        fprintf(fp, "</span><span style=\"color:black\">");
    }
}

void decode_background(FILE *fp, unsigned char a[LNS][WIDTH], int i, int j)
{
    unsigned char code;
    code = a[i][j-1];

    if(code == RED){
        fprintf(fp, "</span><span style=\"background-color:red\">");
    }

    if(code == GREEN){
        fprintf(fp, "</span><span style=\"background-color:green\">");
    }

    if(code == YELLOW || code == GYELLOW){
        fprintf(fp, "</span><span style=\"background-color:Yellow\">");
    }

    if(code == BLUE || code == GBLUE){
        fprintf(fp, "</span><span style=\"background-color:blue\">");
    }

    if(code == MAGENTA || code == GMAGENTA){
        fprintf(fp, "</span><span style=\"background-color:magenta\">");
    }

    if(code == CYAN || code == GCYAN){
        fprintf(fp, "</span><span style=\"background-color:cyan\">");
    }

    if(code == WHITE || code == GWHITE){
        fprintf(fp, "</span><span style=\"background-color:black\">");
    }

}






