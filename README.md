# JCyberEvolution
 
## About
This is an [artifical life](https://en.wikipedia.org/wiki/Artificial_life) simulator based on the evolving executable DNA.

Currently work is in progress.

## License
JCyberEvolution is licensed under GNU General Public License 3

## Dependencies
All dependencies are provided with this project in extlibs directory (as source or binaries)
There are:
- [SFML](https://www.sfml-dev.org)
- [Dear Imgui](https://github.com/ocornut/imgui)
- [Dear Imgui SFML backend](https://github.com/eliasdaler/imgui-sfml)
- [Dear ImGui syntactic sugar](https://github.com/mnesarco/imgui_sugar)
- [Dear ImGui file dialog](https://github.com/aiekick/ImGuiFileDialog)

## Build
You can build this project with CMake under Windows with MSVC.

Under Linux or with MinGW, you need to download or build SFML and link against it.
(It should work, but hasn't been tested)
May be it will be fixed in the future.

After build, copy the contents of extlibs/SFML/bin to the directory with executable.
