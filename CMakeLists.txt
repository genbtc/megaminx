#by genBTC 2017

cmake_minimum_required(VERSION 2.7)
include(ProcessorCount)
ProcessorCount(NumCPUS)
message("detected number of processors: " ${NumCPUS})
project(MegaMinx2)
set(LIBRARIES_FROM_REFERENCES "")
add_executable(MegaMinx2 WIN32 common_physics/camera.cpp common_physics/opengl.cpp common_physics/raytri.cpp engine/center.cpp engine/corner.cpp engine/edge.cpp engine/face.cpp engine/megaminx.cpp engine/utils.cpp main.cpp Res.rc)
# GLUT
find_package(GLUT REQUIRED)
if(NOT GLUT_FOUND)
    message(ERROR "GLUT not found!")
endif(NOT GLUT_FOUND)
# OpenGL
find_package(OpenGL REQUIRED)
if(NOT OPENGL_FOUND)
    message(ERROR "OPENGL not found!")
endif(NOT OPENGL_FOUND)
target_link_libraries(MegaMinx2 -lfreeglut -lglu32 -lopengl32 "${LIBRARIES_FROM_REFERENCES}")
SET(CMAKE_SYSTEM_NAME Windows)
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -std=c++1z -DDEBUG=1")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -std=c++1z -DDEBUG=1")
