/* This file is part of JCyberEvolution.

JCyberEvolution is free software: you can redistribute it and/or modify it 
under the terms of the GNU General Public License as published by the Free Software Foundation, 
either version 3 of the License, or (at your option) any later version.

JCyberEvolution is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; 
without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with JCyberEvolution. 
If not, see <https://www.gnu.org/licenses/>. */

#ifndef FIELD_VIEW_H_
#define FIELD_VIEW_H_

#include "Field.h"

#include <imgui.h>
#include <imgui-SFML.h>
#include <imgui_sugar.hpp>

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>

#include <deque>
#include <string>
#include <utility>

class FieldView : public sf::Drawable {
public:
    enum class Tool : int {
        SELECT_BOT = 0,
        DELETE_BOT,
        PLACE_BOT,
    };

    FieldView(sf::Vector2f screenSize, Field& field);

    // return true if handled
    bool handleMouseWheelScrollEvent(const sf::Event::MouseWheelScrollEvent& event) noexcept;

    bool handleResizeEvent(const sf::Event::SizeEvent& event) noexcept {
        resize(event.width, event.height);
        return true;
    }

    bool handleKeyPressedEvent(const sf::Event::KeyEvent& event) noexcept {
        if (event.code == sf::Keyboard::Space) {
            m_paused = !m_paused;
            return true;
        }
        return false;
    }

    bool handleMouseButtonPressedEvent(const sf::Event::MouseButtonEvent& event, 
                                       const sf::RenderTarget& target) noexcept;

    void updateField() noexcept;

    void update(bool keyboardAvailable, sf::Time elapsedTime) noexcept;

    void showGui() noexcept;

    void draw(sf::RenderTarget& target, sf::RenderStates states) const noexcept override;

    int getSimulationSpeed() const noexcept {
        return m_paused ? 0 : m_simulationSpeed;
    }

    bool handleBotMoved(sf::Vector2i from, sf::Vector2i to) noexcept {
        if (from == m_selectedBot) {
            selectBot(to);
            return true;
        }
        return false;
    }

    bool handleBotDied(sf::Vector2i coords) noexcept {
        if (coords == m_selectedBot) {
            selectBot({-1, -1});
            return true;
        }
        return false;
    }
private:
    Field& m_field;
    sf::View m_view;

    float m_zoom;

    bool m_shouldDrawBots;

    float m_fillDensity;
    int m_simulationSpeed;
    bool m_paused;

    Tool m_tool;
    sf::Vector2i m_selectedBot;
    sf::RectangleShape m_selectionShape;

    std::deque<std::pair<std::string, std::string>> m_recentFiles;
    int m_selectedFile;
    std::unique_ptr<Bot> m_loadedBot;

    std::deque<int> m_populationHistory;

    float m_baseZoomingChange;
    float m_baseMovingSpeed;
    float m_speedModificator;

    void resize(float width, float height) noexcept;

    void selectBot(sf::Vector2i coords) noexcept {
        m_selectedBot = coords;
        if (coords != sf::Vector2i{-1, -1}) {
            m_selectionShape.setSize({1.5f, 1.5f});
            m_selectionShape.setPosition(coords.x, coords.y);
        }
    }

    void selectFile(int index) noexcept {
        m_selectedFile = index;
        m_loadedBot.reset();
    }
};

#endif