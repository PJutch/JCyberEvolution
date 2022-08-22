/* This file is part of JCyberEvolution.

JCyberEvolution is free software: you can redistribute it and/or modify it 
under the terms of the GNU General Public License as published by the Free Software Foundation, 
either version 3 of the License, or (at your option) any later version.

JCyberEvolution is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; 
without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with JCyberEvolution. 
If not, see <https://www.gnu.org/licenses/>. */

#include "Field.h"

#include <imgui.h>
#include <imgui-SFML.h>
#include <imgui_sugar.hpp>

#include <SFML/Graphics.hpp>
using sf::Color;
using sf::RenderWindow;
using sf::Texture;

#include <SFML/System.hpp>
using sf::Clock;
using sf::Time;

#include <SFML/Window.hpp>
using sf::Event;
using sf::Keyboard;
using sf::VideoMode;
using sf::Clipboard;
namespace Style = sf::Style;

#include <iostream>
using std::cout;
using std::endl;

#include <string>
using std::string;

#include <stdlib.h>

int main(int argc, char** argv) {
    auto videoMode = VideoMode::getDesktopMode();
    float width = videoMode.width;
    float height = videoMode.height;

    RenderWindow window{videoMode, "JCyberEvolution", Style::Fullscreen};
    window.setFramerateLimit(60);

    ImGui::SFML::Init(window);
    ImGuiIO& io = ImGui::GetIO();

    Field field{128, 128};

    Clock clock;
    while (window.isOpen()) {
        Time elapsedTime = clock.restart();

        Event event;
        while (window.pollEvent(event)) {
            ImGui::SFML::ProcessEvent(window, event);

            switch (event.type) {
                case Event::Closed: window.close(); break;
                case Event::KeyPressed:
                    if (io.WantCaptureKeyboard) break;
                    if (event.key.code == Keyboard::Escape) window.close();
                    break;
            }
        }
        
        ImGui::SFML::Update(window, elapsedTime);
    
        window.clear(Color::White);
        window.draw(field);

        ImGui::SFML::Render(window);

        window.display();
    }

    ImGui::SFML::Shutdown();
    return EXIT_SUCCESS;
}