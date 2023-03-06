GenBTC's Megaminx Solver v1.3.9 (March 06, 2023)
========
![LatestScreenshot](https://puu.sh/yyfd7/525320ef95.png)
<p>It's a working simulator of a <b>Megaminx</b>, a Rubik's ~~cube~~ Dodecahedron puzzle!
<p>You can test your moves by rotating a virtual cube, that works just like the real thing.
<p>Comes preloaded with over 100+ Megaminx algorithms, known sequences of moves that save you time.
<p>It also can auto-solve itself now, and print out the moves!
<p>Using this program will make you a Megaminx EXPERT!
<p>
<h2>Usage:</h2>
<br>press H for on-screen Help (shown above):
<br>spacebar Auto-spins the 3D render
<br>Right click for Menu
<br>Double click to rotate the current face clockwise. (+Shift to reverse).
<br>Left drag spins Camera / spin Viewport (arrow keys too). MouseWheel Zooms
<br>default Front Face is Light_Blue
<br>Supports SaveFiles, Save & Restore
<br>Supports instanciating a virtual Shadow Cube for experimental branching
<br>Full List of Keyboard commands shown below:
<br>Code made to take advantage of C++ features, to be std=c++17 compliant, minimal C used.
<br>Open source, and editable, customizable. Love FOSS. I am open to taking suggestions/PRs also.
<br>CMake build script (Makefile also included)
<h3>Compatible with Linux, Windows (maybe mac)</h3>
<h2>Build Instructions for CMake (CMakeLists.txt):
<br>Easily Builds using CMake. (make sure its installed) 
<br>cmake build .
<br>`cmake --build .` or regular `make` if no CMake
<h2>Compile, Build & run (Makefile)</h2>
<br>Linux: You'll need the pre-requisities: `apt-get install g++ make freeglut3-dev`
<br>Build with `make`, run with `./megaminx`
<br>Windows: Compatible out of the box with Visual Studio 2017 "Open Folder w/ CMake".
<h2>Builds on "Linux for Windows" (MSYS2/MingW) / WSL(untested)</h2>
<br>Requires Freeglut(mingw), OpenGL(windows), GLU(windows)
<br>Download and Install MSYS2/MingW64 environment, and from the shell run: 
<br>Install pre-reqs: `pacman -S base-devel freeglut`
<br>Re-Generate the project with cmake: `cmake -G "MSYS Makefiles" .` the last dot is the current dir.
<br>Alternatively, default Makefile provided
<p>
<h2>Main Menu:</h2>
<br>"Toggle Spinning"
<br>"Algorithms ---->" = Human Bulk Rotate Algos
<br>"AutoSolve Layer" = Computer Teleport InstaSolve by Layer (internally "pops-out"=aka cheating)
<br>"AutoSolve Face " = Computer Teleport InstaSolve by Faces (Reset to solved position)
<br>"AutoSolve Face" = Computer Teleport Current Face actions
<br>"AutoModify Piece" = Computer Teleport can AutoSwap Pieces
<p>
<h2>Usage / Navigation:</h2>
<h2>Keyboard Commands:</h2>
"[Right Click]  Actions Menu"<br>
"[Dbl Click]  Rotate Current >>"<br>
"[F1-F8=F9] AutoSolve Layer# >>"<br>
"  +Shift  CounterClockwise <<"<br>
"[W/w]  Rotate Upper Face </>"<br>
"[S/s]  Rotate Front Face </>"<br>
"[A/a]  Rotate Left Side </>"<br>
"[D/d]  Rotate Right Side</>"<br>
"[Z/z]  Rotate DownLeft  </>"<br>
"[C/c]  Rotate DownRight </>"<br>
"[X/x]  Rotate Bottom Face </>"<br>
"[Space]  Toggle Auto-Spin"<br>
"[BackSpace]  Reset Camera"<br>
"[Delete]  Scramble Puzzle"<br>
"[Enter] Solve Current Face"<br>
<p>
<h2>Possible cube-state actions</h2>
"Rotate Current Face ClockWise"<br>
"Solve/Reset Current Face (All)"<br>
"Solve Current Face's Edges"<br>
"Solve Current Face's Corners"<br>
"Swap Face's Edges 1-5<br>
"Swap Face's Corners 1-5<br>
<h2>AutoSolve using Human Rotate Algorithms</h2>
"Solve White Star" = F1/<br>
"Solve White Corners" = /F1<br>
"2nd Layer Edges" = F2<br>
"3rd Layer Corners" = F3<br>
"4th Layer Edges" = F4<br>
"5th Layer Corners" = F5<br>
"6th Layer Edges" = F6<br>
"7th Last Layer Edges" = F7<br>
"7th Last Layer Corners" = F8<br>
"All Layers at once" = F9<br>
<p>+Also Manual Last Layer solve commands for all of the above<br>
<h2>Notation Algorithms (outdated. see "face.h" at the bottom for complete list and description. TODO: TBD.)</h2><br>
"r u R' U'"<br>
"l u L' U'"<br>
"U' L' u l u r U' R'"<br>
"r u R' u r 2U' R'"<br>
"u l U' R' u L' U' r"<br>
"u r 2U' L' 2u R' 2U' l u"<br>
"R' D' R D" ORIENT CORNER<br>
"Edge Permutation 1":  r2 U2' R2' U' r2 U2' R2' (5 to 2, 2 to 4, 4 to 5) HORSE<br>
"Edge Permutation 2":  r2 u2  R2' u  r2 u2  R2' (5 to 4, 4 to 2, 2 to 5) HORSE<br>
"Edge Permutation 3":  r u R' F', r  u  R' U', R' f r2 U' R' (5 to 1, 1 to 2, 2 to 5) MUSHROOM<br>
"Edge Permutation 4":  r u R' u , R' U' r2 U', R' u R' u, r U2' (5 to 2, 2 to 1, 1 to 5) MUSHROOM<br>
"Edge Permutation 5":  l r u2, L' u R', l U' r u2, L' u2 R' (5 and 3 swap, 1 and 2 swap) BUNNY<br>

<h2>For Developers:</h2>
Simplified Project Structure: <br />
main.cpp	-	Main() entry point, GLUT callbacks, render function, keyboard/mouse handlers. the scaffolding structure hooks up to the megaminx engine, and frontend of the glut UI <br />
main-menu.cpp		-	on screen display text. menu GUI. this file bridges the front end and the back end. such as what the algorithms are named or categories shown, what order, etc <br />
engine/megaminx.cpp	-	Main puzzle. render() function draws geometry of each sub-piece. Everything in the cube is managed from interacting with this Megaminx* object <br />
engine/shadow.cpp	-	Shadow Cube is a virtual version of the cube that only exists in memory, for theoretical play. It can be created and destroyed at whim without affecting the main visible one, and State can be "cloned" back and forth. Useful for iterating moves into the future, then throwing them away. a snapshot for you to revert. hot/cold version control. <br />
engine/solve.cpp	-	longest function, all the actual cube transforms, that describe all the algorithms <br />
engine/face.cpp	    -	Conceptual heart of how the puzzle part is turned into code logic <br />
engine/piece.h		-	base class common definitions for basing the following on: <br />
engine/center.cpp	-	center primitive. Very simple openGL geometry. simple case statement abstracts important draw algorithm logic <br />
engine/corner.cpp	-	corner primitive. a little bit more openGL geometry, same thing. <br />
engine/edge.cpp		-	edge primitive. a little bit less openGL geometry, same thing. <br />
engine/load.cpp		-	Saves State. save/restore from File support, and clone/destroy virtual shadow cube <br />
ui/opengl.cpp		-	main orthographic viewport projection setup and various GL helpers <br />
ui/camera.cpp		-	the viewport. everything moves by shifting the camera's projected view (code feels like a mirror) <br />
<code>Headers:
	bytes	-	headers filename
	19326	-	./src/engine/face.h
	11120	-	./src/engine/piece.h
	7999 	-	./src/engine/megaminx.h
	5643 	-	./src/engine/color.h
	2096 	-	./src/ui/camera.h
	1683 	-	./src/main.h
	1513 	-	./src/ui/opengl.h
	311	 	-	./src/engine/corner.h
	299	 	-	./src/engine/edge.h
	269	 	-	./src/engine/center.h
==============
</code>
ClassDiagram.png not fullly updated.<br>
functions-face.txt is updated.<br>
<br>
<h2> Programming Credits </h2>
genBTC - genBTC@gmx.com - December 2017, all of 2018, and January 2019 and October 2019 - February 2020(C)<br>
Updated README 2023 and tried to make better.
Uses forked code originally from Taras Khalymon (tkhalymon) / @cybervisiontech / taras.khalymon@gmail.com<br>
