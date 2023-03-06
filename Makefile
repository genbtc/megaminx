#Simplest makefile possible. Works on Linux and Windows. And MSYS/MingW.

UNAME = $(shell uname -o)
ifeq ($(OS),Windows_NT)
	EXEEXT = .exe
	COMPILE_OPT = -lfreeglut -lglu32 -lopengl32
else
	COMPILE_OPT = -lGL -lGLU -lglut
endif

all: megaminx

megaminx: main.o main-menu.o center.o edge.o corner.o face.o megaminx.o camera.o opengl.o load.o shadow.o solve.o 
	g++ main.o main-menu.o center.o edge.o corner.o face.o megaminx.o camera.o opengl.o load.o shadow.o solve.o $(COMPILE_OPT) -o megaminx

main.o: src/main.cpp
	g++ -c src/main.cpp
main-menu.o: src/main-menu.cpp
	g++ -c src/main-menu.cpp
center.o: src/engine/center.cpp
	g++ -c src/engine/center.cpp
edge.o: src/engine/edge.cpp
	g++ -c src/engine/edge.cpp
corner.o: src/engine/corner.cpp
	g++ -c src/engine/corner.cpp
face.o: src/engine/face.cpp
	g++ -c src/engine/face.cpp
megaminx.o: src/engine/megaminx.cpp
	g++ -c src/engine/megaminx.cpp
load.o: src/engine/load.cpp
	g++ -c src/engine/load.cpp
shadow.o: src/engine/shadow.cpp
	g++ -c src/engine/shadow.cpp
solve.o: src/engine/solve.cpp
	g++ -c src/engine/solve.cpp	
camera.o: src/ui/camera.cpp
	g++ -c src/ui/camera.cpp
opengl.o: src/ui/opengl.cpp
	g++ -c src/ui/opengl.cpp
#Res.rc.o is a Resource file for an Icon, and windres.exe needs to be used with arguments, it needs this stuff
# (customized to use the 32-bit windres on a MingW64 setup)
# This is broken for linux, cant use properly
Res.rc.o:
	windres.exe --preprocessor=x86_64-w64-mingw32-g++.exe --preprocessor-arg=-E --preprocessor-arg=-xc-header --preprocessor-arg=-DRC_INVOKED -i src/Res.rc -o Res.rc.o

clean:
	rm -rf *.o megaminx$(EXEEXT)

run:
	./megaminx$(EXEEXT)

test:
	valgrind --leak-check=full ./megaminx$(EXEEXT)