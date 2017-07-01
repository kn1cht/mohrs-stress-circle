CC = g++ --std=c++11 -Wall
FLAGS = -Wno-deprecated-declarations
LIBGLUI = -framework GLUI
LIBGLUT= -framework GLUT 
LIBGL= -framework OpenGL

all: main feedback_pick

main: main.cpp
	${CC} -o main main.cpp ${FLAGS} ${LIBGLUI} ${LIBGLUT} ${LIBGL} ${LIBS}

feedback_pick: feedback_pick.cpp
	${CC} -o feedback_pick feedback_pick.cpp${FLAGS} ${LIBGLUI} ${LIBGLUT} ${LIBGL} ${LIBS}

drag: drag.cpp
	${CC} -o drag drag.cpp${FLAGS} ${LIBGLUI} ${LIBGLUT} ${LIBGL} ${LIBS}

clean:
	rm main feedback_pick drag
