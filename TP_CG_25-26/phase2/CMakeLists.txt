cmake_minimum_required(VERSION 3.15)
project(solar-system LANGUAGES CXX)

# ----------------------------
# Project-wide settings
# ----------------------------
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Good default warnings (tune as you like)
if(MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# Output binaries to /bin (inside build dir)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# ----------------------------
# Common include directory
# ----------------------------
# Your headers are in include/
set(PROJECT_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)

# ----------------------------
# Dependencies: OpenGL + GLUT
# ----------------------------
find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)

# ----------------------------
# Helper: create a common "core" library
# Put shared code between engine and generator here
# ----------------------------
add_library(solar_core
    # Add shared cpp files here (example):
    # src/world_parser.cpp
    # src/math.cpp
    src/tinyxml2.cpp
    src/World.cpp
    src/Group.cpp
    src/Model.cpp
    src/Point.cpp
    src/Vector.cpp
    src/SceneParser.cpp
    src/Transformation.cpp
)

target_include_directories(solar_core
    PUBLIC
    ${PROJECT_INCLUDE_DIR}
)

target_link_libraries(solar_core
    PUBLIC
    OpenGL::GL
    OpenGL::GLU
    GLUT::GLUT
)

# If you need platform-specific tweaks:
if(NOT MSVC)
    # Fix: previous file had "Wno-deprecated" missing dash
    target_compile_options(solar_core PRIVATE -Wno-deprecated-declarations)
endif()

# ----------------------------
# Executable: engine
# ----------------------------
add_executable(engine
    src/engine.cpp
    # src/engine/...
)

target_link_libraries(engine
    PRIVATE
    solar_core
)

# ----------------------------
# Executable: generator
# ----------------------------
add_executable(generator
    src/generator.cpp
    # src/generator/...
)

target_link_libraries(generator
    PRIVATE
    solar_core
)

# ----------------------------
# Doxygen documentation target
# ----------------------------
find_package(Doxygen QUIET)

if(DOXYGEN_FOUND)
    set(DOXYFILE_IN ${CMAKE_CURRENT_SOURCE_DIR}/docs/Doxyfile)
    set(DOXYFILE_OUT ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)

    if(EXISTS ${DOXYFILE_IN})
        # If your Doxyfile already has correct paths, you can skip configure_file.
        # This allows you to use @PROJECT_SOURCE_DIR@ etc if you want.
        configure_file(${DOXYFILE_IN} ${DOXYFILE_OUT} @ONLY)

        add_custom_target(doc
            COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYFILE_OUT}
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
            COMMENT "Generating Doxygen documentation"
            VERBATIM
        )
    else()
        message(WARNING "Doxygen found, but docs/Doxyfile not found.")
    endif()
else()
    message(STATUS "Doxygen not found. 'doc' target will not be available.")
endif()
