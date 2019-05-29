cmake_minimum_required (VERSION 3.0.2)
project(DStudio)
set(PACKAGE_VERSION "0.0.0")

if ( (NOT CMAKE_BUILD_TYPE) OR (CMAKE_BUILD_TYPE MATCHES Release))
    message(STATUS "Release mode. ${CMAKE_CXX_FLAGS_RELEASE} -Wall")
    set(CMAKE_BUILD_TYPE "Release")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Wall")

endif()

if (CMAKE_BUILD_TYPE MATCHES Debug)
    message(STATUS "Debug mode. ${CMAKE_CXX_FLAGS_DEBUG} -Wall")
    add_definitions(-DDSTUDIO_DEBUG)
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall")
endif()

set(DSTUDIO_USE_GLFW3 "1")

find_package(glfw3 REQUIRED)
find_package(OpenGL REQUIRED)
find_package(PNG REQUIRED)

add_executable(
	mockup
	main.c
    extensions.c
    knobs.c
    ui.c
    dsandgrains/ui.c
)

if(DEFINED DSTUDIO_USE_GLFW3)
    add_definitions(-DDSTUDIO_USE_GLFW3)
    target_link_libraries(mockup glfw)
endif()

target_include_directories(mockup PRIVATE ${PNG_INCLUDE_DIR} ${OPENGL_INCLUDE_DIR})
target_link_libraries(mockup ${PNG_LIBRARY} ${OPENGL_gl_LIBRARY} ${CMAKE_DL_LIBS} )
