MegaMinx - v1.29 genBTC Mod (December 2, 2017)
========
![Current129](https://puu.sh/yyfd7/525320ef95.png)
<p>It's a working simulator of a Rubik's ~~cube~~ Dodecahedron puzzle called the <b>Megaminx</b>!<br />
<h2>Usage:</h2>
<br>press H for on-screen Help (shown above): </br>
<br>Right click for Menu</br>
<br>Double click rotates. Hold Shift for reverse.</br>
<br>Left drag changes Camera (arrow keys too). MouseWheel Zooms</br>
<br>Full List of Keyboard commands at bottom of page</br>
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
"Default Front Face is Blue"<br>
"[Right Click]  Action Menu"<br>
"[Dbl Click]  Rotate Current Face Clockwise >>"<br>
"  /+Shift  Rotate Current Face CounterClockwise <<"<br>
"[D/d]  Rotate Right Face <>"<br>
"[A/a]  Rotate Left Face <>"<br>
"[S/s]  Rotate Front Face <>"<br>
"[W/w]  Rotate Upper Face <>"<br>
"[Z/z,X/x,C/c]  Rotate Diag <>"<br>
"[B/b]  Rotate Bottom Face <>"<br>
"[F1]-[F12]/+Shift  Rotate Face # <>"<br>
"[Space]  Toggle Auto-Spin"<br>
"[BackSpace]  Reset Camera"<br>
"[Delete]  Scramble Puzzle"<br>
"[Enter] Try Solve Current Face" (cheating)<br>
<h2> Programming Credits </h2>
genBTC - genBTC@gmx.com - November to December 2017<br>
Uses forked code from Taras Khalymon (tkhalymon) / @cybervisiontech / taras.khalymon@gmail.com<br>