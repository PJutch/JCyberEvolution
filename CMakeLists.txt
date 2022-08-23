cmake_minimum_required(VERSION 3.10)

project(JCyberEvolution VERSION 1.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include_directories(src)
include_directories(extlibs/imgui)
include_directories(extlibs/SFML/include)

add_executable(JCyberEvolution src/main.cpp src/Field.cpp src/Cell.cpp src/FieldView.cpp)
set_property(TARGET JCyberEvolution PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreadedDLL")

add_library(DearImGui STATIC ../extlibs/imgui/imgui.cpp ../extlibs/imgui/imgui_draw.cpp 
                             ../extlibs/imgui/imgui_widgets.cpp ../extlibs/imgui/imgui_tables.cpp 
                             ../extlibs/imgui/imgui_demo.cpp ../extlibs/imgui/imgui-SFML.cpp)
target_link_libraries(JCyberEvolution PRIVATE DearImGui)

add_library(OpenGL STATIC IMPORTED)
set_property(TARGET OpenGL PROPERTY IMPORTED_LOCATION opengl32.lib)
target_link_libraries(JCyberEvolution PRIVATE OpenGL)

add_library(SFML_Main STATIC IMPORTED)
set_property(TARGET SFML_Main PROPERTY IMPORTED_LOCATION ../extlibs/SFML/lib/sfml-main.lib)
target_link_libraries(JCyberEvolution PRIVATE SFML_Main)

add_library(SFML_System SHARED IMPORTED)
set_property(TARGET SFML_System PROPERTY IMPORTED_LOCATION ../extlibs/SFML/bin/sfml-system-2.dll)
set_property(TARGET SFML_System PROPERTY IMPORTED_IMPLIB ../extlibs/SFML/lib/sfml-system.lib)
target_link_libraries(JCyberEvolution PRIVATE SFML_System)

add_library(SFML_Window SHARED IMPORTED)
set_property(TARGET SFML_Window PROPERTY IMPORTED_LOCATION ../extlibs/SFML/bin/sfml-window-2.dll)
set_property(TARGET SFML_Window PROPERTY IMPORTED_IMPLIB ../extlibs/SFML/lib/sfml-window.lib)
target_link_libraries(JCyberEvolution PRIVATE SFML_Window)

add_library(SFML_Graphics SHARED IMPORTED)
set_property(TARGET SFML_Graphics PROPERTY IMPORTED_LOCATION ../extlibs/SFML/bin/sfml-graphics-2.dll)
set_property(TARGET SFML_Graphics PROPERTY IMPORTED_IMPLIB ../extlibs/SFML/lib/sfml-graphics.lib)
target_link_libraries(JCyberEvolution PRIVATE SFML_Graphics)

add_library(SFML_Audio SHARED IMPORTED)
set_property(TARGET SFML_Audio PROPERTY IMPORTED_LOCATION ../extlibs/SFML/bin/sfml-audio-2.dll)
set_property(TARGET SFML_Audio PROPERTY IMPORTED_IMPLIB ../extlibs/SFML/lib/sfml-audio.lib)
target_link_libraries(JCyberEvolution PRIVATE SFML_Audio)
