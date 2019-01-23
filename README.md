MegaMinx - v1.34 genBTC Mod (January 23, 2019)
========
![Current129](https://puu.sh/yyfd7/525320ef95.png)
<p>It's a working simulator of a Rubik's ~~cube~~ Dodecahedron puzzle called the <b>Megaminx</b>!<br />
<h2>Usage:</h2>
<br>press H for on-screen Help (shown above): </br>
<br>Right click for Menu</br>
<br>Double click rotates. Hold Shift for reverse.</br>
<br>Default Front Face is Blue</br>
<br>Left drag changes Camera (arrow keys too). MouseWheel Zooms</br>
<br>Full List of Keyboard commands shown below:</br>
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
<h2>Keyboard Commands:</h2>
"[Right Click]  Actions Menu"<br>
"[Dbl Click]  Rotate Current >>"<br>
"[F1-F12]     Rotate Face #  >>"<br>
"  +Shift  CounterClockwise <<"<br>
"[W/w]  Rotate Upper Face </>"<br>
"[S/s]  Rotate Front Face </>"<br>
"[A/a]  Rotate Side/Left  </>"<br>
"[D/d]  Rotate Side/Right </>"<br>
"[Z/z]  Rotate Diag/Left  </>"<br>
"[C/c]  Rotate Diag/Right </>"<br>
"[X/x]  Rotate Bottom Face </>"<br>
"[Space]  Toggle Auto-Spin"<br>
"[BackSpace]  Reset Camera"<br>
"[Delete]  Scramble Puzzle"<br>
"[Enter] Solve Current Face"<br>
<p>
<h2>Possible cube manipulations</h2>
"Rotate Current Face ClockWise"<br>
"Solve/Reset Current Face (All)"<br>
"Solve Current Face's Edges"<br>
"Solve Current Face's Corners"<br>
"Swap Face Edge 1-5<br>
"Rotate Face Corner 1-5<br>
<h2>AutoSolve using Human Rotate Algorithms</h2>
"Solve White Star"<br>
"Solve White Corners"<br>
"2nd Layer Edges"<br>
"3rd Layer Corners"<br>
"4th Layer Edges"<br>
"5th Layer Corners"<br>
"6th Layer Edges"<br>
<p>
<h2>Last Layer solve helpers</h2><br>
"Solve Grey Star"<br>
"Solve Grey Corners"<br>
<h2>Notation Algorithms</h2><br>
"r u R' U'"<br>
"l u L' U'"<br>
"U' L' u l u r U' R'"<br>
"r u R' u r 2U' R'"<br>
"u l U' R' u L' U' r"<br>
"u r 2U' L' 2u R' 2U' l u"<br>
"R' D' R D"<br>
"Edge Permutation 1":  r2 U2' R2' U' r2 U2' R2' (5 to 2, 2 to 4, 4 to 5)<br>
"Edge Permutation 2":  r2 u2  R2' u  r2 u2  R2' (5 to 4, 4 to 2, 2 to 5)<br>
"Edge Permutation 3":  r u R' F', r  u  R' U', R' f r2 U' R' (5 to 1, 1 to 2, 2 to 5)<br>
"Edge Permutation 4":  r u R' u , R' U' r2 U', R' u R' u, r U2' (5 to 2, 2 to 1, 1 to 5)<br>
"Edge Permutation 5":  l r u2, L' u R', l U' r u2, L' u2 R' (5 and 3 swap, 1 and 2 swap)<br>
<h2> Programming Credits </h2>
genBTC - genBTC@gmx.com - December 2017, all of 2018, and January 2019 (C)<br>
Uses forked code from Taras Khalymon (tkhalymon) / @cybervisiontech / taras.khalymon@gmail.com<br>