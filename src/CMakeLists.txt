# TODO:
#   - Make Jack optionnal

cmake_minimum_required (VERSION 3.0.2)
project(DStudio)
set(PACKAGE_VERSION "0.0.0")

set(CUSTOM_FLAGS    "-Werror -Wall -Wextra -fPIC")

if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    add_definitions(-DDSTUDIO_RELY_ON_X11)
    set(DSTUDIO_RELY_ON_X11 1)
    message(STATUS "DStudio target GNU/Linux plateform.")
endif()

if ( (NOT CMAKE_BUILD_TYPE) OR (CMAKE_BUILD_TYPE MATCHES Release))
    message(STATUS "Release mode. ${CMAKE_C_FLAGS_RELEASE} ${CUSTOM_FLAGS}")
    set(CMAKE_BUILD_TYPE "Release")
    set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} ${CUSTOM_FLAGS}")

endif()

if (CMAKE_BUILD_TYPE MATCHES Debug)
    add_definitions(-DDSTUDIO_DEBUG)
    if(DEFINED FSANITIZE AND FSANITIZE MATCHES Disabled)
        message(STATUS "Debug mode. ${CMAKE_C_FLAGS_DEBUG} -fPIC -fno-omit-frame-pointer -fsanitize=undefined -fsanitize-address-use-after-scope")
        set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -fPIC -fno-omit-frame-pointer -fsanitize=undefined -fsanitize-address-use-after-scope")
    else()
        message(STATUS "Debug mode. ${CMAKE_C_FLAGS_DEBUG} -fPIC -fsanitize=address -fno-omit-frame-pointer -fsanitize=undefined -fsanitize-address-use-after-scope")
        message("-- /!\\ Please note -fsanitize=address might cause OpenGL to not work as expected.")
        message("-- /!\\ If this is the case, using -DFSANITIZE=Disabled might help.")
        set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -fPIC -fsanitize=address -fno-omit-frame-pointer -fsanitize=undefined -fsanitize-address-use-after-scope")
    endif()
endif()

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake_modules")

find_package(FLAC REQUIRED)

add_definitions(-DDSTUDIO_USE_JACK_AUDIO_CONNECTION_KIT)
find_package(Jack REQUIRED)

set(OpenGL_GL_PREFERENCE GLVND)
find_package(OpenGL REQUIRED OpenGL GLX) 
find_package(PNG REQUIRED)
find_package(Threads REQUIRED)

if(DEFINED DSTUDIO_RELY_ON_X11)
    find_package(X11 REQUIRED)
endif()

add_executable(
	dsandgrains
    dsandgrains/add.c
    dsandgrains/dsandgrains.c
	dsandgrains/main.c
    dsandgrains/samples.c
    dsandgrains/sample_screen.c
    dsandgrains/ui.c
    audio_api.c
    bar_plot.c
    buttons.c
    common.c
    extensions.c
    fileutils.c
    flac.c
    info_bar.c
    instances.c
    interactive_list.c
    knob.c
    open_file.c
    sliders.c
    text.c
    text_pointer.c
    transition_animation.c
    ui.c
    voices.c
    window_management.c
)

target_include_directories(dsandgrains PRIVATE ${PNG_INCLUDE_DIR} ${OPENGL_INCLUDE_DIR} ${FLAC_INCLUDE_DIR})
target_link_libraries(dsandgrains ${PNG_LIBRARY} ${OPENGL_LIBRARIES} ${CMAKE_DL_LIBS} ${CMAKE_THREAD_LIBS_INIT} ${FLAC_LIBRARY} ${JACK_LIBRARY} m)

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
