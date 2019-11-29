cmake_minimum_required (VERSION 3.0.2)
project(DStudio)
set(PACKAGE_VERSION "0.0.0")

set(CUSTOM_FLAGS    "-Werror \
                     -Wall \
                     -Wextra"
)

if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    add_definitions(-DDSTUDIO_RELY_ON_X11)
    set(DSTUDIO_RELY_ON_X11 1)
    message(STATUS "DStudio target GNU/Linux plateform.")
endif()

if ( (NOT CMAKE_BUILD_TYPE) OR (CMAKE_BUILD_TYPE MATCHES Release))
    message(STATUS "Release mode. ${CMAKE_C_FLAGS_RELEASE} ${CUSTOM_FLAGS}")
    set(CMAKE_BUILD_TYPE "Release")
    set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE}s ${CUSTOM_FLAGS}")

endif()

if (CMAKE_BUILD_TYPE MATCHES Debug)
    message(STATUS "Debug mode. ${CMAKE_C_FLAGS_DEBUG} ${CUSTOM_FLAGS}")
    add_definitions(-DDSTUDIO_DEBUG)
    set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} ${CUSTOM_FLAGS}")
endif()

find_package(OpenGL REQUIRED)
find_package(PNG REQUIRED)
find_package(Threads REQUIRED)

if(DEFINED DSTUDIO_RELY_ON_X11)
    find_package(X11 REQUIRED)
endif()

add_executable(
	dsandgrains
	dsandgrains/main.c
    dsandgrains/ui.c
    add.c
    buttons.c
    common.c
    extensions.c
    fileutils.c
    instances.c
    interactive_list.c
    system_usage.c
    text.c
    text_pointer.c
    ui.c
    voices.c
    window_management.c
)

target_include_directories(dsandgrains PRIVATE ${PNG_INCLUDE_DIR} ${OPENGL_INCLUDE_DIR})
target_link_libraries(dsandgrains ${PNG_LIBRARY} ${OPENGL_gl_LIBRARY} ${CMAKE_DL_LIBS} ${CMAKE_THREAD_LIBS_INIT} m)

if(DEFINED DSTUDIO_RELY_ON_X11)
    target_include_directories(dsandgrains PRIVATE ${X11_INCLUDE_DIR})
    target_link_libraries(dsandgrains ${X11_LIBRARIES})
endif()

if(CMAKE_BUILD_TYPE MATCHES Release)
    add_custom_command(
        TARGET dsandgrains POST_BUILD
        COMMAND strip --strip-unneeded -s -R .comment -R .gnu.version dsandgrains
        COMMENT "strip --strip-unneeded -s -R .comment -R .gnu.version dsandgrains"
    )
endif()

