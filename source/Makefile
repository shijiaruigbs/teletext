CFLAGS = -Wall -Wextra -Wfloat-equal -pedantic -ansi -O2

teletext: teletext.c teletextmain.c teletext.h neillsdl.c neillsdl.h
	gcc teletext.c teletextmain.c neillsdl.c -o teletext -I include -L lib -l SDL2-2.0.0 $(CFLAGS)


clean:
	rm -fr teletext
