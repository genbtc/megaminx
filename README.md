MegaMinx - v1.29 genBTC Mod
========
![Current129](https://puu.sh/y9LEC/fb26388442.png)
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