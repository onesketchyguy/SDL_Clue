cmake_minimum_required(VERSION 3.5.0)

project(Clue)
enable_language(CXX)

# Set the C standard
set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED True)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Set the project name
project(Clue)

# Find OpenGL
set(OpenGL_GL_PREFERENCE LEGACY) # Use the old version of OGL
find_package(OpenGL REQUIRED)

# Find SDL2
find_package(SDL2 REQUIRED)

# Add the executable
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "../")
file(GLOB_RECURSE SOURCES "../src/*.cpp")
add_executable(${PROJECT_NAME} ${SOURCES})
# Link the libraries
include_directories(${SDL2_INCLUDE_DIRS} ../libs)
target_link_libraries(${PROJECT_NAME} ${OPENGL_LIBRARIES} ${SDL2_LIBRARIES}  SDL2main SDL2_ttf SDL2_image SDL2 GL X11)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -lpthread -lstdc++fs -fopenmp")
set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)