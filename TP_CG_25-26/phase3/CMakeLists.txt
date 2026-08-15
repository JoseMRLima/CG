cmake_minimum_required(VERSION 3.15)
project(solar-system LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set(PROJECT_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)

find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)
find_package(GLEW REQUIRED)

add_library(solar_core
    lib/xml/tinyxml2.cpp

    lib/core/Model.cpp
    lib/core/Point.cpp
    lib/core/Vector.cpp
    lib/core/utils.cpp

    engine/World.cpp
    engine/Group.cpp
    engine/SceneParser.cpp
    engine/Transformation.cpp
    engine/Animation.cpp

    generator/shapes.cpp
)

target_include_directories(solar_core
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/engine
    ${CMAKE_CURRENT_SOURCE_DIR}/generator
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/core
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/xml
)

target_link_libraries(solar_core
    PUBLIC
    OpenGL::GL
    OpenGL::GLU
    GLUT::GLUT
    GLEW::GLEW
)

if(NOT MSVC)
    target_compile_options(solar_core PRIVATE -Wno-deprecated-declarations)
endif()

add_executable(engine engine/main.cpp)

add_executable(generator generator/main.cpp)

target_link_libraries(engine
    PRIVATE
    solar_core
)

target_link_libraries(generator
    PRIVATE
    solar_core
)

find_package(Doxygen QUIET)

if(DOXYGEN_FOUND)
    set(DOXYFILE_IN ${CMAKE_CURRENT_SOURCE_DIR}/docs/Doxyfile)
    set(DOXYFILE_OUT ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)

    if(EXISTS ${DOXYFILE_IN})
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
