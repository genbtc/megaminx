# by genBTC 2017/2018/2019 requires CMake>=3.8 for C++ 17
cmake_minimum_required(VERSION 3.8)
# project setup on CMake on Windows 64 using MSYS2/MingW64/G++ compilers. Change stuff as needed.
# Also compiles natively on Windows with MSVC as long as you MSVC build freeglut and set the GLUT_ROOT_PATH on Line 33.
# Works on VCPKG using CMake in Visual Studio 2017 natively, with: "vcpkg install freeglut:x64-windows" ; "vcpkg integrate install"
set(PACKAGE_NAME "Megaminx")
set(PACKAGE_VERSION "1.3.4")
project(${PACKAGE_NAME})
set(PACKAGE_STRING "${PACKAGE_NAME} ${PACKAGE_VERSION}")
set(PACKAGE_TARNAME ${PACKAGE_NAME})
#CMake Settings:
# Turn on C++ 17 flag.
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
#MingW Specific verbose Settings:
if(MINGW)
    #TODO Flag for 64 bit or not
	set(CMAKE_PREFIX_PATH "C:/msys64/mingw64")
	set(CMAKE_MAKE_PROGRAM "ninja")
	set(CMAKE_C_COMPILER   "gcc")
	set(CMAKE_CXX_COMPILER "g++")
	message("MingW Platform detected, using G++/GCC/Ninja from: " ${CMAKE_PREFIX_PATH})
	set(MING_INCLUDES "
    ${CMAKE_PREFIX_PATH}/include/c++/9.2.0;
    ${CMAKE_PREFIX_PATH}/include/c++/9.2.0/x86_64-w64-mingw32;
    ${CMAKE_PREFIX_PATH}/include/c++/9.2.0/backward;
    ${CMAKE_PREFIX_PATH}/lib/gcc/x86_64-w64-mingw32/9.2.0/include;
    ${CMAKE_PREFIX_PATH}/include;    
    ${CMAKE_PREFIX_PATH}/lib/gcc/x86_64-w64-mingw32/9.2.0/include-fixed;
    ${CMAKE_PREFIX_PATH}/x86_64-w64-mingw32/include;
    ")
	message("Manually specified MingW GCC System Include Dirs: " ${MING_INCLUDES})
	include_directories(${MING_INCLUDES})
elseif(NOT MINGW AND MSVC)	
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
set(MAKEFLAGS "-j${NumCPUS}")
# need GLUT (aka freeglut)
find_package(GLUT REQUIRED)
message("--> GLUT detected as such: incl: ${GLUT_INCLUDE_DIR}  libs: ${GLUT_LIBRARIES} ")
if(MINGW)
    # need OpenGL (or Xmesa)
    find_package(OpenGL REQUIRED)
    if(NOT OPENGL_FOUND) #(or OPENGL_XMESA_FOUND)
        message(ERROR "--> OPENGL not found! ${OPENGL_LIBRARY_DIR}")
    endif(NOT OPENGL_FOUND)
    # need GLU of some sort (glu32 from openGL most likely)
    if(NOT OPENGL_GLU_FOUND)
        message(ERROR "--> OPENGL GLU not found!")
    endif(NOT OPENGL_GLU_FOUND)
endif()
#// Prepare EXE - by listing all source files (explicitly is safer), and the Res.rc (icon resource)
add_executable(${PACKAGE_NAME} WIN32 
    src/ui/camera.cpp
    src/ui/opengl.cpp
    src/engine/center.cpp
    src/engine/corner.cpp
    src/engine/edge.cpp
    src/engine/face.cpp
    src/engine/megaminx.cpp
    src/main.cpp
    src/Res.rc
)
#//Use Perf-Profiler option in MSVC - Blocks /Incremental option.
#if (MSVC)
#    set_property(TARGET ${PACKAGE_NAME} APPEND_STRING PROPERTY LINK_FLAGS "/Profile")
#endif()
#// Use C++17 on all linkages (is this needed if we have Line 12+13?)
target_compile_features(${PACKAGE_NAME} INTERFACE cxx_std_17)
#// Make Exe, go!
target_link_libraries(${PACKAGE_NAME} PRIVATE GLUT::GLUT)
if(MINGW)
    #target_include_directories(${PACKAGE_NAME} PRIVATE src)    
    #target_link_libraries(${PACKAGE_NAME} ${GLUT_LIBRARIES})
    target_link_libraries(${PACKAGE_NAME} PRIVATE OpenGL::GL OpenGL::GLU)
endif()
#/// (old) Linked Library Setup: simply (-lfreeglut;-lopengl32;-lglu32;)
#message("Or just do Link libraries Like this: ${GLUT_LIBRARIES};${OPENGL_gl_LIBRARY};${OPENGL_glu_LIBRARY}")