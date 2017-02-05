function(BUILD_EXAMPLE EXPNAME)
  set(TARGET_EXAMPLE_DIR "${CMAKE_CURRENT_LIST_DIR}/${EXPNAME}")

  configure_file("${CMAKE_CURRENT_LIST_DIR}/${EXPNAME}/common.h.in"
                 "${CMAKE_CURRENT_LIST_DIR}/${EXPNAME}/common.h" @ONLY)

  set(CMAKE_AUTOMOC ON)
  set(CMAKE_INCLUDE_CURRENT_DIR ON)

  file(GLOB SOURCE_FILES "${EXPNAME}/*.cpp" "${EXPNAME}/*.h")
  file(GLOB SHADER_FILES "${EXPNAME}/shaders/*.vert"
                         "${EXPNAME}/shaders/*.tesc"
                         "${EXPNAME}/shaders/*.tese"
                         "${EXPNAME}/shaders/*.geom"
                         "${EXPNAME}/shaders/*.frag"
                         "${EXPNAME}/shaders/*.comp")

  add_executable(${EXPNAME} ${SOURCE_FILES} ${SHADER_FILES})
  qt5_use_modules(${EXPNAME} Widgets OpenGL)

  target_link_libraries(${EXPNAME} ${OPENGL_LIBRARIES} ${QT_LIBRARIES})
  source_group("Source Files" FILES ${SOURCE_FILES})
  source_group("Shader Files" FILES ${SHADER_FILES})
endfunction(BUILD_EXAMPLE)

file(GLOB SUBDIR_LIST RELATIVE ${CMAKE_CURRENT_LIST_DIR} "*")
foreach(SUBDIR ${SUBDIR_LIST})
  if (IS_DIRECTORY "${CMAKE_CURRENT_LIST_DIR}/${SUBDIR}")
    BUILD_EXAMPLE(${SUBDIR})
  endif()
endforeach()
