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
    
        window.clear(Color::Black);

        ImGui::SFML::Render(window);

        window.display();
    }

    ImGui::SFML::Shutdown();
    return EXIT_SUCCESS;
}