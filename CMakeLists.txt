cmake_minimum_required(VERSION 3.0.0)
project(QtOpenGLTutorials)

if (MSVC)
  set(QT5_ROOT "QT5-NOT_FOUND" CACHE PATH "")
  if (EXISTS "${QT5_ROOT}/lib/Qt5Widgets.lib")
    set(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} "${QT5_ROOT}")
  endif()
endif()

find_package(Qt5Widgets REQUIRED)
find_package(Qt5OpenGL REQUIRED)
find_package(OpenGL REQUIRED)

set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/build)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/bin)

add_subdirectory(sources)

file(MAKE_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
set(QT_LIBRARY_LIST Qt5Widgets Qt5Widgetsd Qt5Core Qt5Cored Qt5Gui Qt5Guid
                    Qt5OpenGL Qt5OpenGLd)
foreach(LIB ${QT_LIBRARY_LIST})
  file(COPY ${CMAKE_PREFIX_PATH}/bin/${LIB}.dll
       DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
endforeach()
