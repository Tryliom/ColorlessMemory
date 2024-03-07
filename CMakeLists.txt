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

add_executable(client MainClient.cpp)
add_executable(server MainServer.cpp)

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

# Client
file(GLOB_RECURSE CLIENT_FILES client/src/*.cpp client/include/*.h)
add_library(ClientPart STATIC ${CLIENT_FILES})
target_include_directories(ClientPart PUBLIC client/include/)
target_include_directories(ClientPart PUBLIC client/src/)
target_link_libraries(ClientPart PUBLIC sfml-graphics sfml-window sfml-system sfml-network)

if(MSVC)
    target_link_libraries(ClientPart PRIVATE sfml-main FLAC OpenAL OpenGL Vorbis)
else()
    target_link_libraries(ClientPart PRIVATE X11 FLAC UDev OpenAL ${OPENGL_LIBRARIES})
endif()

# Server
file(GLOB_RECURSE SERVER_FILES server/src/*.cpp server/include/*.h)
add_library(ServerPart STATIC ${SERVER_FILES})
target_include_directories(ServerPart PUBLIC server/include/)
target_include_directories(ServerPart PUBLIC server/src/)

# Common
file(GLOB_RECURSE COMMON_FILES common/include/*.h common/src/*.cpp)
add_library(Common STATIC ${COMMON_FILES})
target_include_directories(Common PUBLIC common/include/)
target_include_directories(Common PUBLIC common/src/)

# Network
file(GLOB_RECURSE NETWORK_FILES network/include/*.h network/src/*.cpp)
add_library(Network STATIC ${NETWORK_FILES})
target_include_directories(Network PUBLIC network/include/)
target_include_directories(Network PUBLIC network/src/)
target_link_libraries(Network PUBLIC sfml-system sfml-network)

# Linking
target_link_libraries(Network PUBLIC Common)
target_link_libraries(ClientPart PUBLIC Network)
target_link_libraries(ServerPart PUBLIC Network)

target_link_libraries(client PRIVATE ClientPart)
target_link_libraries(server PRIVATE ServerPart)

add_dependencies(client data_target)