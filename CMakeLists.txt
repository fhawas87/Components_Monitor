cmake_minimum_required(VERSION 3.5)

project(app LANGUAGES C CXX)

find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)
find_package(GLEW REQUIRED)

set (SRC
  
  src/main.cpp
  src/RenderWindow/MainRender/render.cpp
)

#set(EXTERNAL ${CMAKE_CURRENT_SOURCE_DIR}/external)
#set(IMGUI_DIR ${EXTERNAL}/imgui)

set(IMGUI_DIR ${CMAKE_CURRENT_SOURCE_DIR}/external/imgui)

add_library(imgui STATIC
  
  ${IMGUI_DIR}/imgui.cpp
  ${IMGUI_DIR}/imgui_draw.cpp
  ${IMGUI_DIR}/imgui_tables.cpp
  ${IMGUI_DIR}/imgui_widgets.cpp
  ${IMGUI_DIR}/backends/imgui_impl_glfw.cpp
  ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp
)

target_compile_definitions(imgui PUBLIC IMGUI_IMPL_OPENGL_LOADER_GLEW)

add_executable(${PROJECT_NAME} ${SRC})

target_include_directories(imgui PUBLIC ${IMGUI_DIR} ${IMGUI_DIR}/backends)
target_include_directories(${PROJECT_NAME} PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/src
  ${CMAKE_CURRENT_SOURCE_DIR}/src/RenderWindow
  ${CMAKE_CURRENT_SOURCE_DIR}/src/RenderWindow/MainRender
)
target_link_libraries(${PROJECT_NAME} PRIVATE
  imgui
  glfw 
  GLEW::GLEW
  OpenGL::GL
)
