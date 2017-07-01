CC = g++ --std=c++11 -Wall
FLAGS = -framework GLUT -framework OpenGL -Wno-c++11-extensions -Wno-deprecated-declarations

all: main feedback_pick

main: main.cpp
	g++ -o main main.cpp ${FLAGS}

feedback_pick: feedback_pick.cpp
	g++ -o feedback_pick feedback_pick.cpp ${FLAGS}

clean:
	rm main feedback_pick
