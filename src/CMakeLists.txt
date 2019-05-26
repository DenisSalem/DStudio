cmake_minimum_required (VERSION 3.0.2)
project(DStudio)
enable_testing()

find_package(PNG REQUIRED)


set(PACKAGE_VERSION "0.0.0")
set(DSTUDIO_USE_GLFW3 "1")

find_package(OpenGL REQUIRED)

add_executable(
	mockup
	main.c
    extensions.c
    knob.c
    ui.c
)

add_definitions(-DDSTUDIO_DEBUG)

if(DEFINED DSTUDIO_USE_GLFW3)
    add_definitions(-DDSTUDIO_USE_GLFW3)

    target_link_libraries(mockup glfw)
endif()

target_include_directories(mockup PRIVATE ${PNG_INCLUDE_DIR} ${OPENGL_INCLUDE_DIR})
target_link_libraries(mockup ${PNG_LIBRARY} ${OPENGL_gl_LIBRARY} ${CMAKE_DL_LIBS} )
