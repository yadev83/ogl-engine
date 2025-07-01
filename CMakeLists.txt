# initial config
cmake_minimum_required(VERSION 3.10)
project(OGL_ENGINE VERSION 1.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

# set variables
set(SRC_DIR ${PROJECT_SOURCE_DIR}/src)
set(CONFIG_DIR ${PROJECT_SOURCE_DIR}/config)
include_directories(${SRC_DIR})

# add a custom SOURCE_ROUT definition for relative file path
add_compile_definitions(SOURCE_ROOT="${CMAKE_SOURCE_DIR}/")

# source files
file(GLOB_RECURSE PROJECT_SRC_FILES CONFIGURE_DEPENDS "${PROJECT_SOURCE_DIR}/src/*.cpp")

# add tools subdir
add_subdirectory(tools)

# load libs
find_package(glfw3 CONFIG REQUIRED)
find_package(glad CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(Freetype CONFIG REQUIRED)
find_package(OpenAL CONFIG REQUIRED)
find_package(Stb REQUIRED)

# create executable and set dest dir based on build type
if(CMAKE_BUILD_TYPE STREQUAL "Release")
    add_executable(${PROJECT_NAME} WIN32 src/main.cpp ${PROJECT_SRC_FILES})
    set(BUILD_DEST_DIR ${CMAKE_BINARY_DIR}/Release)
else()
    add_executable(${PROJECT_NAME} src/main.cpp ${PROJECT_SRC_FILES})
    set(BUILD_DEST_DIR ${CMAKE_BINARY_DIR}/Debug)
endif()

# Link libraries
target_link_libraries(${PROJECT_NAME} PRIVATE glfw glad::glad glm::glm Freetype::Freetype OpenAL::OpenAL ${Stb_INCLUDE_DIR})

# Bundle assets
set(DATA_INPUT_DIR ${PROJECT_SOURCE_DIR}/assets)
set(DATA_OUTPUT_DIR ${BUILD_DEST_DIR}/data)

file(GLOB ASSET_TOP_DIRS RELATIVE ${DATA_INPUT_DIR} ${DATA_INPUT_DIR}/*)

foreach(ASSET ${ASSET_TOP_DIRS})
    if(IS_DIRECTORY ${DATA_INPUT_DIR}/${ASSET})
        set(INPUT_DIR ${DATA_INPUT_DIR}/${ASSET})
        set(OUTPUT_PAK ${DATA_OUTPUT_DIR}/${ASSET}.pak)

        add_custom_command(
            OUTPUT ${OUTPUT_PAK}
            COMMAND tools/pak_bundler.exe ${INPUT_DIR} ${OUTPUT_PAK}
            DEPENDS pak_bundler
            COMMENT "Bundling ${ASSET}"
        )
        
        add_custom_target(${ASSET}_pak DEPENDS ${OUTPUT_PAK})
        add_dependencies(${PROJECT_NAME} ${ASSET}_pak)
    endif()
endforeach()

# copy config to build folder
file(COPY ${CONFIG_DIR} DESTINATION ${BUILD_DEST_DIR})