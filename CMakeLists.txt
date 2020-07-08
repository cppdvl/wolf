cmake_minimum_required(VERSION 3.12)
project(Wolf)
message(STATUS " ._-= Project Name : ${PROJECT_NAME} =-_.")

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

message(STATUS ${GUIIMPL_SOURCES})
foreach(GUISOURCES ${GUIIMPL_SOURCES})
    message(STATUS "Gui Resource: ${GUISOURCES}")
endforeach()

file(GLOB_RECURSE   PROJECT_HEADERS Wolf/Headers/*.hpp)
file(GLOB           PROJECT_SOURCES Wolf/Sources/main.cpp)
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

add_subdirectory(Wolf/Vendor/wolfengine)
message (STATUS "WolfEngine Resources Path: ${ENGINE_RESOURCES_FULLPATH}")


add_definitions(-DPROJECT_SOURCE_DIR=\"${PROJECT_SOURCE_DIR}\"
                -DSTB_IMAGE_IMPLEMENTATION
                )
list(APPEND FULL_SOURCES_LIST ${PROJECT_SOURCES} ${PROJECT_HEADERS}
                               ${PROJECT_SHADERS} ${PROJECT_CONFIGS}
                               )

message(STATUS "Full sources list: ")
foreach(FULL_SOURCES_LIST_ELEM IN LISTS FULL_SOURCES_LIST)
    message(STATUS "Resource: ${FULL_SOURCES_LIST_ELEM}")
endforeach()

add_executable(${PROJECT_NAME})
target_sources(${PROJECT_NAME} PRIVATE ${FULL_SOURCES_LIST})

target_include_directories(${PROJECT_NAME} PRIVATE 
                    /usr/local/include
                    Wolf/Headers/
                    Wolf/Headers/Utils/  
                    Wolf/Vendor/wolfengine/Vendor/glm/
                    Wolf/Vendor/wolfengine/Vendor/glad/include
                    Wolf/Vendor/wolfengine/Vendor/gui                    
                    Wolf/Vendor/stb/
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
    TARGET ${PROJECT_NAME} PRE_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/Wolf/Resources $<TARGET_FILE_DIR:${PROJECT_NAME}>
    )
add_custom_command(
    TARGET ${PROJECT_NAME} PRE_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/Wolf/Vendor/wolfengine/Resources $<TARGET_FILE_DIR:${PROJECT_NAME}>/wolfengine/Resources
    )

