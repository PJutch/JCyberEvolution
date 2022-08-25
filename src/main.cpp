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
#include "FieldView.h"

#include <imgui.h>
#include <imgui-SFML.h>
#include <imgui_sugar.hpp>

#include <SFML/Graphics.hpp>
using sf::Color;
using sf::RenderWindow;
using sf::Texture;
using sf::View;
using sf::FloatRect;

#include <SFML/System.hpp>
using sf::Clock;
using sf::Time;

#include <SFML/Window.hpp>
using sf::Event;
using sf::Keyboard;
using sf::VideoMode;
using sf::Clipboard;
namespace Style = sf::Style;

#include <random>
using std::random_device;

#include <iostream>
using std::cout;
using std::endl;

#include <string>
using std::string;

#include <algorithm>
using std::clamp;

#include <stdlib.h>

int main(int argc, char** argv) {
    auto videoMode = VideoMode::getDesktopMode();
    float width = static_cast<float>(videoMode.width);
    float height = static_cast<float>(videoMode.height);

    RenderWindow window{videoMode, "JCyberEvolution", Style::Fullscreen};
    window.setVerticalSyncEnabled(true);

    ImGui::SFML::Init(window);
    ImGuiIO& io = ImGui::GetIO();

    random_device randomDevice;

    Field field{128, 128, randomDevice()};
    field.setPosition(0, 0);

    FieldView fieldView{{0.f, 0.f, 128.f, 128.f}, {0.f, 0.f, height / width, 1.f}, field};
    fieldView.setShouldRepeat(false);

    float baseMovingSpeed = 100.0f;

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

                    switch (event.key.code) {
                        case Keyboard::Escape: window.close(); break;
                    }
                    break;
                case Event::MouseWheelScrolled:
                    if (io.WantCaptureMouse) break;
                    fieldView.handleEvent(event);
                    break;
            }
        }

        ImGui::SFML::Update(window, elapsedTime);

        fieldView.update(!io.WantCaptureKeyboard, elapsedTime);
    
        window.clear(Color::White);

        window.draw(fieldView);

        fieldView.showGui();
        
        ImGui::ShowDemoWindow();

        ImGui::SFML::Render(window);

        window.display();
    }

    ImGui::SFML::Shutdown();
    return EXIT_SUCCESS;
}
