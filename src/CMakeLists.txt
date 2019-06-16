cmake_minimum_required (VERSION 3.0.2)
project(DStudio)
set(PACKAGE_VERSION "0.0.0")

if ( (NOT CMAKE_BUILD_TYPE) OR (CMAKE_BUILD_TYPE MATCHES Release))
    message(STATUS "Release mode. ${CMAKE_CXX_FLAGS_RELEASE} -Wall -Winline")
    set(CMAKE_BUILD_TYPE "Release")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Wall -Winline")

endif()

if (CMAKE_BUILD_TYPE MATCHES Debug)
    message(STATUS "Debug mode. ${CMAKE_CXX_FLAGS_DEBUG} -Wall -Winline")
    add_definitions(-DDSTUDIO_DEBUG)
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall -Winline")
endif()

set(DSTUDIO_USE_GLFW3 "1")

if(DEFINED DSTUDIO_USE_GLFW3)
    find_package(glfw3 REQUIRED)
endif()

find_package(OpenGL REQUIRED)
find_package(PNG REQUIRED)
find_package (Threads REQUIRED)
add_executable(
	dsandgrains
	dsandgrains/main.c
    dsandgrains/ui.c
    extensions.c
    knobs.c
    sliders.c
    ui.c
)

if(DEFINED DSTUDIO_USE_GLFW3)
    add_definitions(-DDSTUDIO_USE_GLFW3)
    target_link_libraries(dsandgrains glfw)
endif()

target_include_directories(dsandgrains PRIVATE ${PNG_INCLUDE_DIR} ${OPENGL_INCLUDE_DIR})
target_link_libraries(dsandgrains ${PNG_LIBRARY} ${OPENGL_gl_LIBRARY} ${CMAKE_DL_LIBS} ${CMAKE_THREAD_LIBS_INIT} m)

add_custom_command(
    TARGET dsandgrains POST_BUILD
    COMMAND strip --strip-unneeded -s -R .comment -R .gnu.version dsandgrains
    COMMENT "strip --strip-unneeded -s -R .comment -R .gnu.version dsandgrains"
)
