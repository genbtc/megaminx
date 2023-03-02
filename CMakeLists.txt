# by genBTC 2017/2018/2019/2020 requires CMake>=3.8 for C++ 17
cmake_minimum_required(VERSION 3.8)
# project setup on CMake on Windows 64 using MSYS2/MingW64/G++ compilers. Change stuff as needed.
# Also compiles natively on Windows with MSVC as long as you MSVC build freeglut and set the GLUT_ROOT_PATH on Line 33.
# Works on VCPKG using CMake in Visual Studio 2017 natively, with: "vcpkg install freeglut:x64-windows" ; "vcpkg integrate install"
# Confirmed to work on Linux 2023
set(PACKAGE_NAME "Megaminx")
set(PACKAGE_VERSION "1.3.8")
project(${PACKAGE_NAME})
set(PACKAGE_STRING "${PACKAGE_NAME} ${PACKAGE_VERSION}")
set(PACKAGE_TARNAME ${PACKAGE_NAME})

#Windows/MingW Specific Settings:
if(MINGW)	#MINGW means Windows MSYS2-MingW64 cross, 64 bit
	set(CMAKE_PREFIX_PATH "C:/msys64/mingw64")
	set(CMAKE_MAKE_PROGRAM "ninja")
	set(CMAKE_C_COMPILER   "gcc")
	set(CMAKE_CXX_COMPILER "g++")
	message("MingW Platform detected, using G++/GCC/Ninja from: " ${CMAKE_PREFIX_PATH})
	#explicit path defines, change these as applicable.
	set(MING_INCLUDES "
     ${CMAKE_PREFIX_PATH}/include/c++/10.4.0;
     ${CMAKE_PREFIX_PATH}/include/c++/10.4.0/x86_64-w64-mingw32;
     ${CMAKE_PREFIX_PATH}/include/c++/10.4.0/backward;
     ${CMAKE_PREFIX_PATH}/lib/gcc/x86_64-w64-mingw32/10.4.0/include;
     ${CMAKE_PREFIX_PATH}/include;
     ${CMAKE_PREFIX_PATH}/lib/gcc/x86_64-w64-mingw32/10.4.0/include-fixed;
     ${CMAKE_PREFIX_PATH}/x86_64-w64-mingw32/include;
    ")
	message("Manually specified MingW GCC System Include Dirs: " ${MING_INCLUDES})
	include_directories(${MING_INCLUDES})
elseif(NOT MINGW AND MSVC)	#Windows Native MSVC
    ##If VCPKG is ON (Auto-loaded from CMakeSettings.json cmakeCommandArgs: "-DCMAKE_TOOLCHAIN_FILE=":
    if (VCPKG_TOOLCHAIN)
        message("VCPkg Enabled!")
        message("CMAKE_TOOLCHAIN_FILE: " ${CMAKE_TOOLCHAIN_FILE})
        message("VCPKG_TARGET_TRIPLET: " ${VCPKG_TARGET_TRIPLET})
        message("CMAKE_BUILD_TYPE: "     ${CMAKE_BUILD_TYPE})
        #set(GLUT_ROOT_PATH "C:/vcpkg/installed/x64-windows")
        #message("GLUT_ROOT_PATH: " ${GLUT_ROOT_PATH})
    else()
        set(GLUT_ROOT_PATH "C:/Program Files/freeglut")
        message("GLUT_ROOT_PATH: " ${GLUT_ROOT_PATH})
    endif()
endif()

#Detects Current CPUs' threads for multi-core ninja compilation.
include(ProcessorCount)
ProcessorCount(NumCPUS)
message("detected number of CPU threads: " ${NumCPUS})
set(MAKEFLAGS "-j${NumCPUS}") #This should be default by now

#// Prepare EXE - by listing all source files (explicitly is safer), and the Res.rc (icon resource)
add_executable(${PACKAGE_NAME} WIN32
    src/main.cpp
    src/ui/camera.cpp
    src/ui/opengl.cpp
#    src/ui/menu.cpp
    src/engine/center.cpp
    src/engine/corner.cpp
    src/engine/edge.cpp
    src/engine/face.cpp
    src/engine/megaminx.cpp
    src/engine/solve.cpp
    src/engine/shadow.cpp
    src/engine/load.cpp
    src/main.cpp
    src/Res.rc
)

#CMake Settings:
message("C++ 17 Standard required, Enabling")
# Turn on C++ 17 flag.
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
#// Use C++17 on all linkages (is this needed if we have ^^ ?)
target_compile_features(${PACKAGE_NAME} INTERFACE cxx_std_17)

# Find Library, _need_ GLUT (aka freeglut)
find_package(GLUT REQUIRED)
message("--> GLUT detected as: libs: ${GLUT_LIBRARIES} include: ${GLUT_INCLUDE_DIR}")
# Link Exe, w/ GLUT
target_link_libraries(${PACKAGE_NAME} PRIVATE GLUT::GLUT)

if(UNIX OR LINUX OR MINGW)
	message("checking confirmation of working GL subsystems...")
#    CMake Warning (dev) at /usr/share/cmake/Modules/FindOpenGL.cmake:315
#    (message):Policy CMP0072 is not set: FindOpenGL prefers GLVND by default when
#    available.  Run "cmake --help-policy CMP0072" for policy details.  Use the
#    cmake_policy command to set the policy and suppress this warning.
#    
#    FindOpenGL found both a legacy GL library:
#      OPENGL_gl_LIBRARY: /usr/lib64/libGL.so
#    
#    and GLVND libraries for OpenGL and GLX:
#      OPENGL_opengl_LIBRARY: /usr/lib64/libOpenGL.so
#      OPENGL_glx_LIBRARY: /usr/lib64/libGLX.so
#    
#    OpenGL_GL_PREFERENCE has not been set to "GLVND" or "LEGACY", so for
#    compatibility with CMake 3.10 and below the legacy GL library will be used.
#Either works.
	set(OpenGL_GL_PREFERENCE "GLVND")
    message("OpenGL Preference set to GLVND instead of legacy...")
    # Find Library, _need_ a OpenGL (or Xmesa)
    find_package(OpenGL REQUIRED)
    if(NOT OPENGL_FOUND) #(or OPENGL_XMESA_FOUND)
        message(ERROR "--> OPENGL not found! ${OPENGL_LIBRARY_DIR}")
    endif(NOT OPENGL_FOUND)
    # _need_ a GLU of some sort (glu32 or libGLU from opengl most likely)
    if(NOT OPENGL_GLU_FOUND)
        message(ERROR "--> OPENGL GLU not found!")
    endif(NOT OPENGL_GLU_FOUND)
	# Link Exe w/ GL / GLU
    target_link_libraries(${PACKAGE_NAME} PRIVATE OpenGL::GL OpenGL::GLU)
endif()
