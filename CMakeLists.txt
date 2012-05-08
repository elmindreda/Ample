cmake_minimum_required(VERSION 2.8)

project(ample C CXX)
set(AMPLE_VERSION 0.3)

find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)

find_library(verse_LIBRARY verse PATHS ../verse)
if (NOT verse_LIBRARY)
  message(FATAL_ERROR "Verse library not found")
endif()

find_path(verse_HEADER verse.h PATHS ../verse)
if (NOT verse_HEADER)
  message(FATAL_ERROR "Verse header not found")
endif()

include_directories(${ample_SOURCE_DIR}/../verse)

configure_file(${ample_SOURCE_DIR}/docs/Doxyfile.in
               ${ample_BINARY_DIR}/docs/Doxyfile @ONLY)

configure_file(${ample_SOURCE_DIR}/include/Config.h.in
               ${ample_BINARY_DIR}/include/Config.h @ONLY)

include_directories(include ${verse_INCLUDE})

add_subdirectory(src)
add_subdirectory(examples)

install(DIRECTORY include
        DESTINATION include
        FILES_MATCHING PATTERN "*.h")

