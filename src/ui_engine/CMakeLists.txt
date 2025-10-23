cmake_minimum_required (VERSION 3.5.0)

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

set(OpenGL_GL_PREFERENCE GLVND)
find_package(OpenGL REQUIRED OpenGL GLX) 
find_package(glfw3 3.4 REQUIRED)

add_executable(
    ui_poc
    poc.c
    dstudio_ui_engine.c
    extensions.c
    ../memory_management/dstudio_memory_management.c
)

target_include_directories(ui_poc PRIVATE ${PNG_INCLUDE_DIR} ${OPENGL_INCLUDE_DIR})
target_link_libraries(ui_poc ${OPENGL_LIBRARIES} ${CMAKE_DL_LIBS} glfw png)

if(CMAKE_BUILD_TYPE MATCHES Release)
    add_custom_command(
        TARGET ui_poc POST_BUILD
        COMMAND strip --strip-unneeded -s -R .comment -R .gnu.version ui_poc
        COMMENT "strip --strip-unneeded -s -R .comment -R .gnu.version ui_poc"
    )
endif()
