#Simplest makefile possible. Works on Linux and Windows. And MSYS/MingW.

UNAME = $(shell uname -o)
ifeq ($(OS),Windows_NT)
	EXEEXT = .exe
	COMPILE_OPT = -lfreeglut -lglu32 -lopengl32
else
	COMPILE_OPT = -lGL -lGLU -lglut
endif

all: megaminx

megaminx: main.o center.o edge.o corner.o face.o utils.o megaminx.o camera.o opengl.o raytri.o Res.rc.o
	g++ main.o center.o edge.o corner.o face.o utils.o megaminx.o camera.o opengl.o raytri.o Res.rc.o $(COMPILE_OPT) -o megaminx

main.o: main.cpp
	g++ -c main.cpp
center.o: engine/center.cpp
	g++ -c engine/center.cpp
edge.o: engine/edge.cpp
	g++ -c engine/edge.cpp
corner.o: engine/corner.cpp
	g++ -c engine/corner.cpp
face.o: engine/face.cpp
	g++ -c engine/face.cpp
utils.o: engine/utils.cpp
	g++ -c engine/utils.cpp
megaminx.o: engine/megaminx.cpp
	g++ -c engine/megaminx.cpp
camera.o: common_physics/camera.cpp
	g++ -c common_physics/camera.cpp
opengl.o: common_physics/opengl.cpp
	g++ -c common_physics/opengl.cpp
raytri.o: common_physics/raytri.cpp
	g++ -c common_physics/raytri.cpp
#Res.rc.o is a Resource file for an Icon, and windres.exe needs to be used with arguments, it needs this stuff
# (customized to use the 32-bit windres on a MingW64 setup)
Res.rc.o:
	windres.exe --preprocessor=x86_64-w64-mingw32-g++.exe --preprocessor-arg=-E --preprocessor-arg=-xc-header --preprocessor-arg=-DRC_INVOKED -i Res.rc -o Res.rc.o

clean:
	rm -rf *.o megaminx$(EXEEXT)

run:
	./megaminx$(EXEEXT)
	
test:
	valgrind --leak-check=full ./megaminx$(EXEEXT)
