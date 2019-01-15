CMAKE_MINIMUM_REQUIRED(VERSION 3.1)
project(fastdensity)

if (NOT CMAKE_BUILD_TYPE)
   message(STATUS "No build type selected, default to Release")
   set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build." FORCE)
endif()

option(BUILD_STATIC "Build a static library" OFF)


include_directories(include)

# Require C++11 support, prefer ISO C++ over GNU variants,
# as relying solely on ISO C++ is more portable.
SET(CMAKE_CXX_STANDARD 11)
SET(CMAKE_CXX_STANDARD_REQUIRED ON)
SET(CMAKE_CXX_EXTENSIONS OFF)

SET(CMAKE_C_STANDARD 99)
SET(CMAKE_C_STANDARD_REQUIRED ON)
SET(CMAKE_C_EXTENSIONS OFF)

add_subdirectory (src) 
# enable_testing ()
# add_subdirectory(tests)

MESSAGE( STATUS "CMAKE_SYSTEM_PROCESSOR: " ${CMAKE_SYSTEM_PROCESSOR})
MESSAGE( STATUS "CMAKE_BUILD_TYPE: " ${CMAKE_BUILD_TYPE} )
MESSAGE( STATUS "CMAKE_CXX_COMPILER: " ${CMAKE_CXX_COMPILER} ) # important to know which compiler is used
MESSAGE (STATUS "CMAKE_CXX_FLAGS: " ${CMAKE_CXX_FLAGS} ) # important to know the flags
MESSAGE( STATUS "CMAKE_CXX_FLAGS_DEBUG: " ${CMAKE_CXX_FLAGS_DEBUG} )
MESSAGE( STATUS "CMAKE_CXX_FLAGS_RELEASE: " ${CMAKE_CXX_FLAGS_RELEASE} )
