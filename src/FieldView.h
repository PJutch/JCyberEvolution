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

class FieldView : public sf::Drawable {
public:
    FieldView(sf::FloatRect rect, sf::Vector2f screenSize, Field& field);

    bool handleMouseWheelScrollEvent(const sf::Event::MouseWheelScrollEvent& event) noexcept;

    bool handleResizeEvent(const sf::Event::SizeEvent& event) noexcept;

    bool handleKeyPressedEvent(const sf::Event::KeyEvent& event) noexcept {
        if (event.code == sf::Keyboard::Space) {
            m_paused = !m_paused;
            return true;
        }
        return false;
    }

    void update(bool keyboardAvailable, sf::Time elapsedTime) noexcept;

    void showGui() noexcept;

    void draw(sf::RenderTarget& target, sf::RenderStates states) const noexcept override;

    int getSimulationSpeed() const noexcept {
        return m_paused ? 0 : m_simulationSpeed;
    }

    void setShouldRepeat(bool shouldRepeat) noexcept {
        m_shouldRepeat = shouldRepeat;
        m_field.setShouldDrawBorder(!m_shouldRepeat);
    }
private:
    Field& m_field;
    sf::View m_view;

    float m_zoom;

    bool m_shouldRepeat;
    bool m_shouldDrawBots;

    float m_fillDensity;
    int m_simulationSpeed;
    bool m_paused;

    float m_baseZoomingChange;
    float m_baseMovingSpeed;
    float m_speedModificator;

    void resize(float width, float height) noexcept;
};

#endif