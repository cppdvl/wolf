cmake_minimum_required(VERSION 3.0)
project(Glitter)
message(STATUS " ._-= Project Name : ${PROJECT_NAME} =-_.")
option(GLFW_BUILD_DOCS OFF)
option(GLFW_BUILD_EXAMPLES OFF)
option(GLFW_BUILD_TESTS OFF)
add_subdirectory(Glitter/Vendor/glfw)

option(ASSIMP_BUILD_ASSIMP_TOOLS OFF)
option(ASSIMP_BUILD_SAMPLES OFF)
option(ASSIMP_BUILD_TESTS OFF)
#add_subdirectory(Glitter/Vendor/assimp)

option(BUILD_BULLET2_DEMOS OFF)
option(BUILD_CPU_DEMOS OFF)
option(BUILD_EXTRAS OFF)
option(BUILD_OPENGL3_DEMOS OFF)
option(BUILD_UNIT_TESTS OFF)
#add_subdirectory(Glitter/Vendor/bullet)

if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic -std=c++17")
    if(NOT WIN32)
        set(GLAD_LIBRARIES dl)
    endif()
endif()
set(CMAKE_VERBOSE_MAKEFILE TRUE)

configure_file(Glitter/Headers/Utils/filesystem.hpp.in ${CMAKE_SOURCE_DIR}/Glitter/Headers/Utils/filesystem.hpp)

file(GLOB           VENDORS_SOURCES Glitter/Vendor/glad/src/glad.c)
file(GLOB           GUIIMPL_SOURCES Glitter/Vendor/imgui/examples/imgui_impl_glfw.*
                                    Glitter/Vendor/imgui/examples/imgui_impl_opengl3.*
                                    Glitter/Vendor/imgui/*.h
                                    Glitter/Vendor/imgui/imgui.cpp
                                    Glitter/Vendor/imgui/imgui_draw.cpp
                                    Glitter/Vendor/imgui/imgui_widgets.cpp)

message(STATUS ${GUIIMPL_SOURCES})
foreach(GUISOIRCES ${GUIIMPL_SOURCES})
    message(STATUS "Gui Resource: ${GUISOIRCES}")
endforeach()

file(GLOB_RECURSE   PROJECT_HEADERS Glitter/Headers/*.hpp)
file(GLOB           PROJECT_SOURCES Glitter/Sources/*.cpp)
file(GLOB           PROJECT_SHADERS Glitter/Shaders/*.comp
                                    Glitter/Shaders/*.frag
                                    Glitter/Shaders/*.geom
                                    Glitter/Shaders/*.vert)
file(GLOB           PROJECT_CONFIGS CMakeLists.txt
                                    Readme.md
                                    .gitattributes
                                    .gitignore
                                    .gitmodules)

source_group("Headers" FILES ${PROJECT_HEADERS})
source_group("Shaders" FILES ${PROJECT_SHADERS})
source_group("Sources" FILES ${PROJECT_SOURCES})
source_group("Vendors" FILES ${VENDORS_SOURCES})

add_subdirectory(Glitter/Vendor/wolfengine/src)


add_definitions(-DGLFW_INCLUDE_NONE
                -DPROJECT_SOURCE_DIR=\"${PROJECT_SOURCE_DIR}\"
                -DSTB_IMAGE_IMPLEMENTATION
                )
list(APPEND FULL_SOURCES_LIST ${PROJECT_SOURCES} ${PROJECT_HEADERS}
                               ${PROJECT_SHADERS} ${PROJECT_CONFIGS}
                               ${VENDORS_SOURCES} ${GUIIMPL_SOURCES})
message(STATUS "Full sources list: ")
foreach(FULL_SOURCES_LIST_ELEM IN LISTS FULL_SOURCES_LIST)
    message(STATUS "Resource: ${FULL_SOURCES_LIST_ELEM}")
endforeach()

add_executable(${PROJECT_NAME})
target_sources(${PROJECT_NAME} PRIVATE ${FULL_SOURCES_LIST})

target_include_directories(${PROJECT_NAME} PRIVATE 
                    Glitter/Headers/
                    Glitter/Headers/Utils/  
#                   Glitter/Vendor/assimp/include/
#                   Glitter/Vendor/bullet/src/
                    Glitter/Vendor/glad/include/
                    Glitter/Vendor/glfw/include/
                    Glitter/Vendor/glm/
                    Glitter/Vendor/stb/
                    Glitter/Vendor/imgui/
                    Glitter/Vendor/imgui/examples
                    Glitter/Vendor/wolfengine/include)

target_link_libraries(${PROJECT_NAME} PRIVATE glfw
                      ${GLFW_LIBRARIES} ${GLAD_LIBRARIES}
#                     BulletDynamics BulletCollision LinearMath 
                        WolfImporter)
set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${PROJECT_NAME})

add_custom_command(
    TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/Glitter/Shaders $<TARGET_FILE_DIR:${PROJECT_NAME}>
    DEPENDS ${PROJECT_SHADERS})
add_custom_command(
    TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/Glitter/Vendor/wolfengine/*.mtl $<TARGET_FILE_DIR:${PROJECT_NAME}>
    DEPENDS ${PROJECT_SHADERS})
add_custom_command(
    TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/Glitter/Vendor/wolfengine/*.obj $<TARGET_FILE_DIR:${PROJECT_NAME}>
    DEPENDS ${PROJECT_SHADERS})
add_custom_command(
    TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/Glitter/Vendor/wolfengine/*.png $<TARGET_FILE_DIR:${PROJECT_NAME}>
    DEPENDS ${PROJECT_SHADERS})
