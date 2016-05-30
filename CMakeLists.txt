cmake_minimum_required(VERSION 3.0.0)
project(qopenglsample)

if (MSVC)
  set(QT5_ROOT "QT5-NOT_FOUND" CACHE PATH "")
  if (EXISTS "${QT5_ROOT}/lib/Qt5Widgets.lib")
    set(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} "${QT5_ROOT}")
  endif()
endif()

find_package(Qt5Widgets REQUIRED)
find_package(Qt5OpenGL REQUIRED)
find_package(OpenGL REQUIRED)

add_subdirectory(sources)
