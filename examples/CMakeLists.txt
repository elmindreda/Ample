
include_directories(${OPENGL_INCLUDE_DIR}
                    ${GLUT_INCLUDE_DIR})

add_executable(hello hello.cpp)
add_executable(lint lint.cpp)
add_executable(render render.cpp)

target_link_libraries(hello ample ${verse_LIBRARY})
target_link_libraries(lint ample ${verse_LIBRARY})
target_link_libraries(render ample ${verse_LIBRARY} ${GLUT_LIBRARY} ${OPENGL_glu_LIBRARY} ${OPENGL_gl_LIBRARY})

