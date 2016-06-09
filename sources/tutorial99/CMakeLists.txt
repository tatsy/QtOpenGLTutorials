set(BUILD_TARGET "qopenglsample")

set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

configure_file("${CMAKE_CURRENT_LIST_DIR}/common.h.in"
               "${CMAKE_CURRENT_LIST_DIR}/common.h" @ONLY)

set(SOURCES ${SOURCES}
    common.h
    appmain.cpp glwidget.h glwidget.cpp)
set(SHADERS ${SHADERS}
    render.vs render.fs)

add_executable(${BUILD_TARGET} ${SOURCES} ${SHADERS})
qt5_use_modules(${BUILD_TARGET} Widgets OpenGL)

if (MSVC)
  source_group("Source Files" FILES ${SOURCES})
  source_group("Shaders" FILES ${SHADERS})
endif()

target_link_libraries(${BUILD_TARGET} ${QT_LIBRARIES} ${OPENGL_LIBRARIES})
