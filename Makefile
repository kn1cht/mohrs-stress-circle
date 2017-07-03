CC = g++ --std=c++11 -Wall
CFLAGS = -Wno-deprecated-declarations
LIBGLUI = -framework GLUI
LIBGLUT= -framework GLUT 
LIBGL= -framework OpenGL

all: main feedback_pick drag

main: main.cpp
	${CC} -o $@  $^ ${CFLAGS} ${LIBGLUI} ${LIBGLUT} ${LIBGL}

feedback_pick: feedback_pick.cpp
	${CC} -o $@  $^ ${CFLAGS} ${LIBGLUI} ${LIBGLUT} ${LIBGL}

drag: drag.cpp
	${CC} -o $@  $^ ${CFLAGS} ${LIBGLUI} ${LIBGLUT} ${LIBGL}

clean:
	rm main feedback_pick drag
