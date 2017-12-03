MegaMinx - v1.29 genBTC Mod (December 2, 2017)
========
![Current129](https://puu.sh/yyfd7/525320ef95.png)
<p>It's a working simulator of a Rubik's cube like puzzle called the <b>Megaminx</b>!<br />
<h2>Usage:</h2>
<p>H - Help for: </p>
<p> - Keyboard commands to turn the pieces</p>
<p>Right click for Menu</p>
<p>Left drag changes Camera</p>
<h2>Build & run (Linux)</h2>
You'll need libraries: `g++ make freeglut3-dev`<br />
Build with `make`, run with `make run`
<h2>Builds on Windows</h2>
Download and Install MSYS2/MingW64 environment, and from the shell run: 
`pacman -S base-devel freeglut`<br />
`cmake -G "MSYS Makefiles" .` <br />
`cmake --build .` or `make` <br />
Easily Builds using CMake. <br />
alternatively, MSYS Makefile provided. <br />
Requires Freeglut(mingw), OpenGL(windows), GLU(windows)
Only requires libfreeglut.dll libgcc_s_dw2-1.dll libstdc++-6.dll libwinpthread-1.dll (from MSYS) 
if you want to redistribute the binaries afterward.<br />
<h2>Also Compatible out of the box with Visual Studio Open Folder w/ CMake.</h2>
<h2>For Developers:</h2>
<br>Class Diagram:</br>
![Class Diagram](https://puu.sh/yycv0/5032380248.png)
<h2>Keyboard Commands:</h2>
"Default Front Face is Blue"
"[Right Click]  Action Menu"
"[Dbl Click]  Rotate Current Face Clockwise >>"
"  /+Shift  Rotate Current Face CounterClockwise <<"
"[D/d]  Rotate Right Face <>"
"[A/a]  Rotate Left Face <>"
"[S/s]  Rotate Front Face <>"
"[W/w]  Rotate Upper Face <>"
"[Z/z,X/x,C/c]  Rotate Diag <>"
"[B/b]  Rotate Bottom Face <>"
"[F1]-[F12]/+Shift  Rotate Face # <>"
"[Space]  Toggle Auto-Spin"
"[BackSpace]  Reset Camera"
"[Delete]  Scramble Puzzle"
"[Enter] Try Solve Current Face" (cheating)