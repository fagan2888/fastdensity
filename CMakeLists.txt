CMAKE_MINIMUM_REQUIRED(VERSION 3.12)
project(fastdensity)

if (NOT CMAKE_BUILD_TYPE)
   message(STATUS "No build type selected, default to Release")
   set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build." FORCE)
endif()

find_package(glfw3 3.2 QUIET)
if (GLFW3_FOUND)
#TODO
else()
    set(GLFW_INCLUDE_PATH C:/ CACHE PATH "Path to GLFW include directory")
    set(GLFW_LIBRARY_PATH C:/ CACHE PATH "Path to GLFW library     directory")
    include_directories(${GLFW_INCLUDE_PATH})
    link_directories(${GLFW_LIBRARY_PATH})
    link_libraries("glfw3")
endif()

find_package(OpenGL REQUIRED)
if (OPENGL_FOUND)
    message("opengl found")
    message("include dir: ${OPENGL_INCLUDE_DIR}")
    message("link libraries: ${OPENGL_gl_LIBRARY}")
else (OPENGL_FOUND)
    message("opengl not found")
endif()

find_package(GLEW QUIET)
if (GLEW_FOUND)
    include_directories(${GLEW_INCLUDE_DIRS})
    link_libraries(${GLEW_LIBRARIES})
else()
    set(GLEW_INCLUDE_PATH C:/ CACHE PATH "Path to GLEW include directory")
    set(GLEW_LIBRARY_PATH C:/ CACHE PATH "Path to GLEW library directory")
    include_directories(${GLEW_INCLUDE_PATH})
    link_directories(${GLEW_LIBRARIES_PATH})
    link_libraries("glew32")
endif()


find_package( OpenCV QUIET )
if (OpenCV_FOUND)
    include_directories(${OpenCV_INCLUDE_DIRS})
    link_libraries(${OpenCV_LIBS})
else()
    set(OPENCV_INCLUDE_PATH C:/ CACHE PATH "Path to OpenCV include directory")
    set(OPENCV_LIBRARY_PATH C:/ CACHE PATH "Path to OpenCV library directory")
    include_directories(${OPENCV_INCLUDE_PATH})
    link_directories(${OPENCV_LIBRARIES_PATH})
	if(MSVC)
		link_libraries("opencv_world343")
	else()
	   #TODO for linux / MINGW / MacOS/...
	endif()
endif()

if(OPENMP_FOUND)
	message("openMP found")
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
else()
	message("openMP not found. Maybe you're IDE will provide you a way to link it")
endif()

option(BUILD_STATIC "Build a static library" OFF)
include_directories(include ${OPENGL_INCLUDE_DIR})

# Require C++11 support, prefer ISO C++ over GNU variants,
# as relying solely on ISO C++ is more portable.
SET(CMAKE_CXX_STANDARD 11)
#SET(CMAKE_CXX_STANDARD_REQUIRED ON)
#SET(CMAKE_CXX_EXTENSIONS OFF)

SET(CMAKE_C_STANDARD 99)
#SET(CMAKE_C_STANDARD_REQUIRED ON)
#SET(CMAKE_C_EXTENSIONS OFF)

add_subdirectory (src)
# enable_testing ()
# add_subdirectory(tests)

MESSAGE( STATUS "CMAKE_SYSTEM_PROCESSOR: " ${CMAKE_SYSTEM_PROCESSOR})
MESSAGE( STATUS "CMAKE_BUILD_TYPE: " ${CMAKE_BUILD_TYPE} )
MESSAGE( STATUS "CMAKE_CXX_COMPILER: " ${CMAKE_CXX_COMPILER} ) # important to know which compiler is used
MESSAGE (STATUS "CMAKE_CXX_FLAGS: " ${CMAKE_CXX_FLAGS} ) # important to know the flags
MESSAGE( STATUS "CMAKE_CXX_FLAGS_DEBUG: " ${CMAKE_CXX_FLAGS_DEBUG} )
MESSAGE( STATUS "CMAKE_CXX_FLAGS_RELEASE: " ${CMAKE_CXX_FLAGS_RELEASE} )

#Tells where are the destination directories
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY   ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY   ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY   ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
