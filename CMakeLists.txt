#OpenGL.
#cmake_minimum_required(

include_directories(
C:/Software/msys64/mingw64/include
)

add_definitions(
)
add_executable(main.cpp
engine/center.cpp
engine/corner.cpp
engine/edge.cpp
engine/face.cpp
engine/megaminx.cpp
engine/utils.cpp
)

link_directories(
C:/Software/msys64/mingw64/lib		
)

target_link_libraries(
    freeglut
    glu32
    opengl32
)