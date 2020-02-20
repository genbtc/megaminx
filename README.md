GenBTC's Megaminx Solver v1.35 (February 20, 2020)
========
![Current129](https://puu.sh/yyfd7/525320ef95.png)
<p>It's a working simulator of a Rubik's ~~cube~~ Dodecahedron puzzle called the <b>Megaminx</b>!<br />
<p>You can test your moves by rotating a virtual cube, that works just like the real thing. <br/>
<p>Comes preloaded with over 35 Megaminx algorithms, known sequences of moves that save you time.<br/>
<p>It also solves itself from scratch!<br />
<p>Using this program will make you a Megaminx EXPERT!<br/>
<h2>Usage:</h2>
<br>press H for on-screen Help (shown above): </br>
<br>Right click for Menu</br>
<br>Double click rotates. Hold Shift for reverse.</br>
<br>Default Front Face is Blue</br>
<br>Left drag changes Camera (arrow keys too). MouseWheel Zooms</br>
<br>Supports SaveFiles, Save & Restore</br>
<br>Full List of Keyboard commands shown below:</br>
<br>Code made in C++ and fully std=c++17 compliant<br>
<br>Open source, and editable, customizable. I am taking suggestions also.<br>
<h2>Compatible out of the box with Visual Studio 2017 Open Folder w/ CMake.</h2>
<h2>Build & run (Linux)</h2>
You'll need the pre-requisities: `g++ make freeglut3-dev`<br />
Build with `make`, run with `./megaminx`
<h2>Builds on Windows too</h2>
Requires Freeglut(mingw), OpenGL(windows), GLU(windows)
Download and Install MSYS2/MingW64 environment, and from the shell run: 
Install pre-reqs: `pacman -S base-devel freeglut`<br />
Re-Generate the project with cmake: `cmake -G "MSYS Makefiles" .` <br />
`cmake --build .` or regular `make` if no CMake<br />
Easily Builds using CMake. (make sure its installed) <br />
Alternatively, default MSYS Makefile provided. <br />
When using MingW freeglut, only requires libfreeglut.dll, libgcc_s_dw2-1.dll, libstdc++-6.dll, libwinpthread-1.dll (from MSYS) if you want to redistribute the binaries afterward.<br />
<h2>For Developers:</h2>
========
![ClassDiagram](https://puu.sh/yycv0/5032380248.png)
<p>
<h2>Main Menu:</h2>
"Toggle Spinning"<br>
"Algorithms ---->" = Human Bulk Rotate Algos<br>
"AutoSolve Layer" = Computer InstaSolve by Layer (internally "pops-out"=aka cheating)<br>
"AutoSolve Face " = Computer InstaSolve by Faces (Reset to solved position)<br>
"ADV.Modify Face" = Current Face Modifications<br>
"ADV.Modify Piece" = AutoSwap Piece<br>
<p>
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
<h2>Possible cube-state modifications</h2>
"Rotate Current Face ClockWise"<br>
"Solve/Reset Current Face (All)"<br>
"Solve Current Face's Edges"<br>
"Solve Current Face's Corners"<br>
"Swap 2 Face's Edges 1-5<br>
"Swap 2 Face's Corners 1-5<br>
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
<p>
<h2>Also Manual Last Layer solve commands for all of the above</h2><br>
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
<h2> Programming Credits </h2>
genBTC - genBTC@gmx.com - December 2017, all of 2018, and January 2019 and October 2019 - February 2020(C)<br>
Uses forked code originally from Taras Khalymon (tkhalymon) / @cybervisiontech / taras.khalymon@gmail.com<br>