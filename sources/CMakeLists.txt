set(BUILD_TARGET "qopenglsample")

set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

configure_file("${CMAKE_CURRENT_LIST_DIR}/common.in.h"
               "${CMAKE_CURRENT_LIST_DIR}/common.h" @ONLY)

add_executable(${BUILD_TARGET}
               common.h
               appmain.cpp glwidget.h glwidget.cpp
               render.vs render.gs render.fs)
qt5_use_modules(${BUILD_TARGET} Widgets OpenGL)

target_link_libraries(${BUILD_TARGET} ${QT_LIBRARIES} ${OPENGL_LIBRARIES})
