cmake_minimum_required(VERSION 3.12)
project(Wolf)
message(STATUS " ._-= Project Name : ${PROJECT_NAME} =-_.")
option(GLFW_BUILD_DOCS OFF)
add_subdirectory(Wolf/Vendor/glfw)

if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
	set(CMAKE_CXX_STANDARD 17)
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic -std=c++17")
    if(NOT WIN32)
        set(GLAD_LIBRARIES dl)
    endif()
endif()
set(CMAKE_VERBOSE_MAKEFILE TRUE)

configure_file(Wolf/Headers/Utils/filesystem.hpp.in ${CMAKE_SOURCE_DIR}/Wolf/Headers/Utils/filesystem.hpp)

file(GLOB           VENDORS_SOURCES Wolf/Vendor/glad/src/glad.c)
file(GLOB           GUIIMPL_SOURCES Wolf/Vendor/imgui/examples/imgui_impl_glfw.*
                                    Wolf/Vendor/imgui/examples/imgui_impl_opengl3.*
                                    Wolf/Vendor/imgui/*.h
                                    Wolf/Vendor/imgui/imgui.cpp
                                    Wolf/Vendor/imgui/imgui_draw.cpp
                                    Wolf/Vendor/imgui/imgui_widgets.cpp)

message(STATUS ${GUIIMPL_SOURCES})
foreach(GUISOURCES ${GUIIMPL_SOURCES})
    message(STATUS "Gui Resource: ${GUISOURCES}")
endforeach()

file(GLOB_RECURSE   PROJECT_HEADERS Wolf/Headers/*.hpp)
file(GLOB           PROJECT_SOURCES Wolf/Sources/*.cpp)
file(GLOB           PROJECT_SHADERS Wolf/Shaders/*.comp
                                    Wolf/Shaders/*.frag
                                    Wolf/Shaders/*.geom
                                    Wolf/Shaders/*.vert)
file(GLOB           PROJECT_CONFIGS CMakeLists.txt
                                    Readme.md
                                    .gitattributes
                                    .gitignore
                                    .gitmodules)

source_group("Headers" FILES ${PROJECT_HEADERS})
source_group("Shaders" FILES ${PROJECT_SHADERS})
source_group("Sources" FILES ${PROJECT_SOURCES})
source_group("Vendors" FILES ${VENDORS_SOURCES})

add_subdirectory(Wolf/Vendor/wolfengine/src)


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
                    Wolf/Headers/
                    Wolf/Headers/Utils/  
                    Wolf/Vendor/glad/include/
                    Wolf/Vendor/glfw/include/
                    Wolf/Vendor/glm/
                    Wolf/Vendor/stb/
                    Wolf/Vendor/imgui/
                    Wolf/Vendor/imgui/examples
                    Wolf/Vendor/wolfengine/include)

target_link_libraries(${PROJECT_NAME} PRIVATE glfw
                      ${GLFW_LIBRARIES} ${GLAD_LIBRARIES}
                        WolfImporter)
set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${PROJECT_NAME})

add_custom_command(
    TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/Wolf/Shaders $<TARGET_FILE_DIR:${PROJECT_NAME}>
    DEPENDS ${PROJECT_SHADERS})
add_custom_command(
    TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/Wolf/Resources/Objects/wolf/*.mtl $<TARGET_FILE_DIR:${PROJECT_NAME}>
    DEPENDS ${PROJECT_SHADERS})
add_custom_command(
    TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/Wolf/Resources/Objects/wolf/*.obj $<TARGET_FILE_DIR:${PROJECT_NAME}>
    DEPENDS ${PROJECT_SHADERS})
add_custom_command(
    TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/Wolf/Resources/Textures/*.png $<TARGET_FILE_DIR:${PROJECT_NAME}>
    DEPENDS ${PROJECT_SHADERS})
