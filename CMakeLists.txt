cmake_minimum_required(VERSION 3.22)
project(HideAndSeek2D)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(SFML COMPONENTS system network CONFIG REQUIRED)

add_executable(src client.cpp)
add_executable(server server.cpp)

file(GLOB_RECURSE DATA_FILES
        "data/*.json"
        "data/*.png"
        "data/*.jpg"
        "data/*.bmp"
        "data/*.hdr"
        "data/*.obj"
        "data/*.mtl"
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
target_link_libraries(Common PUBLIC sfml-system sfml-network)
set_target_properties(Common PROPERTIES UNITY_BUILD ON)

target_link_libraries(src PRIVATE Common)
target_link_libraries(server PRIVATE Common)

add_dependencies(src data_target)