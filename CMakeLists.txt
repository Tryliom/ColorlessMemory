cmake_minimum_required(VERSION 3.22)
project(ColorlessMemory)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(HOST_NAME "localhost" CACHE STRING "Set the host name")
set(PORT "43845" CACHE STRING "Set the port")

add_definitions("-DHOST_NAME=\"${HOST_NAME}\"")
add_definitions("-DPORT=${PORT}")

find_package(OpenGL REQUIRED)
find_package(SFML COMPONENTS system network CONFIG REQUIRED)

add_executable(client main_client.cpp)
add_executable(server main_server.cpp)

file(GLOB_RECURSE DATA_FILES
        "data/*.png"
        "data/*.jpg"
        "data/*.ttf"
)
foreach (DATA ${DATA_FILES})
    get_filename_component(FILE_NAME ${DATA} NAME)
    get_filename_component(PATH_NAME ${DATA} DIRECTORY)
    get_filename_component(EXTENSION ${DATA} EXT)
    file(RELATIVE_PATH PATH_NAME "${CMAKE_CURRENT_SOURCE_DIR}" ${PATH_NAME})
    #MESSAGE("Data PATH: ${PATH_NAME} NAME: ${FILE_NAME}")
    set(DATA_OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/${PATH_NAME}/${FILE_NAME}")
    #MESSAGE("Data OUT PATH: ${DATA_OUTPUT}")
    add_custom_command(
            OUTPUT ${DATA_OUTPUT}
            COMMAND ${CMAKE_COMMAND} -E copy
            ${DATA}
            ${DATA_OUTPUT}
            DEPENDS ${DATA})
    list(APPEND Data_OUTPUT_FILES ${DATA_OUTPUT})
endforeach (DATA)


add_custom_target(
        data_target
        DEPENDS ${Data_OUTPUT_FILES}
)

file(GLOB_RECURSE COMMON_FILES src/*.cpp include/*.h)
add_library(Common STATIC ${COMMON_FILES})
target_include_directories(Common PUBLIC include/)
target_include_directories(Common PUBLIC src/)
target_link_libraries(Common PUBLIC sfml-graphics sfml-audio sfml-window sfml-system sfml-network)

if(MSVC)
    target_link_libraries(Common PRIVATE sfml-main FLAC OpenAL OpenGL Vorbis)
else()
    target_link_libraries(Common PRIVATE X11 FLAC UDev OpenAL ${OPENGL_LIBRARIES})
endif()

target_link_libraries(client PRIVATE Common)
target_link_libraries(server PRIVATE Common)

add_dependencies(client data_target)