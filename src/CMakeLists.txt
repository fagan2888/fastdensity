
add_library (fastdensity
    roaring.c
    fastdensity.cpp
)

add_executable (main main.cpp)
target_link_libraries (main fastdensity)
target_link_libraries(main ${OPENGL_gl_LIBRARY} glfw)
target_link_libraries(main ${OPENGL_glu_LIBRARY})

add_executable (amalgamation_demo amalgamation_demo.cpp)
target_link_libraries (amalgamation_demo fastdensity)

add_executable(remimaps remimaps.cpp)
target_link_libraries (remimaps fastdensity)
target_link_libraries (remimaps ${OPENGL_gl_LIBRARY} glfw)
target_link_libraries(remimaps ${OPENGL_glew_LIBRARY})
target_link_libraries(remimaps ${OPENGL_glu_LIBRARY})

#set(CMAKE_BUILD_TYPE Debug)
