add_library (roaring
    roaring.c
)

add_library (fastdensity
    fastdensity.cpp
)

add_executable (main main.cpp)
target_link_libraries (main fastdensity)
if(MSVC)
	target_link_libraries(main glfw3)
else()
	target_link_libraries(main glfw)
endif()
target_link_libraries(main ${OPENGL_gl_LIBRARY})
target_link_libraries(main ${OPENGL_glu_LIBRARY})

add_executable (amalgamation_demo amalgamation_demo.cpp)
target_link_libraries(amalgamation_demo fastdensity)
target_link_libraries(amalgamation_demo roaring)

add_executable(remimaps remimaps.cpp)
target_link_libraries (remimaps fastdensity)
target_link_libraries (remimaps roaring)
target_link_libraries (remimaps ${OPENGL_gl_LIBRARY})
target_link_libraries(remimaps ${OPENGL_glew_LIBRARY})
target_link_libraries(remimaps ${OPENGL_glu_LIBRARY})
if(MSVC)
	target_link_libraries(remimaps glfw3)
else()
	target_link_libraries(remimaps glfw)
endif()

#set(CMAKE_BUILD_TYPE Debug)