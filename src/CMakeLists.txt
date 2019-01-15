
add_library (fastdensity
    roaring.c
    fastdensity.cpp
)

add_executable (main main.cpp)
target_link_libraries (main fastdensity)
add_executable (amalgamation_demo amalgamation_demo.cpp)
target_link_libraries (amalgamation_demo fastdensity)

#set(CMAKE_BUILD_TYPE Debug)
