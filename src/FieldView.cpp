/* This file is part of JCyberEvolution.

JCyberEvolution is free software: you can redistribute it and/or modify it 
under the terms of the GNU General Public License as published by the Free Software Foundation, 
either version 3 of the License, or (at your option) any later version.

JCyberEvolution is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; 
without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with JCyberEvolution. 
If not, see <https://www.gnu.org/licenses/>. */

#include "FieldView.h"

#include <SFML/Graphics.hpp>
using sf::FloatRect;
using sf::Color;
using sf::RenderTarget;
using sf::RenderStates;
using sf::Vector2f;
using sf::Transform;

#include <SFML/System.hpp>
using sf::Keyboard;
using sf::Event;
using sf::Time;

#include <algorithm>
using std::clamp;

#include <cmath>
using std::pow;
using std::fmod;

FieldView::FieldView(FloatRect rect, FloatRect viewport, Field& field) : 
        m_field{field}, m_view{rect}, m_zoom{1.0f}, m_shouldRepeat{true}, m_shouldDrawBots{true}, 
        m_fillDensity{0.5f}, m_baseZoomingChange{1.1f}, m_baseMovingSpeed{10.f}, m_speedModificator{10.f} {
    m_view.setViewport(viewport);
}

bool FieldView::handleEvent(const Event& event) noexcept {
    m_view.zoom(1 / m_zoom);

    float zoomChange = -event.mouseWheelScroll.delta;
    if (Keyboard::isKeyPressed(Keyboard::LShift)) zoomChange *= m_speedModificator;
    if (Keyboard::isKeyPressed(Keyboard::LControl)) zoomChange /= m_speedModificator;

    m_zoom *= pow(m_baseZoomingChange, zoomChange);
    m_zoom = clamp(m_zoom, 0.01f, 1.f);

    m_view.zoom(m_zoom);

    return true;
}

void FieldView::update(bool keyboardAvailable, Time elapsedTime) noexcept {
    if (keyboardAvailable) {
        float moved = m_baseMovingSpeed * elapsedTime.asSeconds();
        if (Keyboard::isKeyPressed(Keyboard::LShift)) moved *= m_speedModificator;
        if (Keyboard::isKeyPressed(Keyboard::LControl)) moved /= m_speedModificator;

        if (Keyboard::isKeyPressed(Keyboard::W))
            m_view.setCenter(m_view.getCenter().x, m_view.getCenter().y - moved); 
        if (Keyboard::isKeyPressed(Keyboard::S))
            m_view.setCenter(m_view.getCenter().x, m_view.getCenter().y + moved); 
        if (Keyboard::isKeyPressed(Keyboard::A))
            m_view.setCenter(m_view.getCenter().x - moved, m_view.getCenter().y); 
        if (Keyboard::isKeyPressed(Keyboard::D))
            m_view.setCenter(m_view.getCenter().x + moved, m_view.getCenter().y); 
    }
}

void FieldView::draw(RenderTarget& target, RenderStates states) const noexcept {
    sf::View prevView = target.getView();

    sf::RectangleShape borderShape;
    borderShape.setSize({prevView.getSize().x * m_view.getViewport().width,  
                        prevView.getSize().y * m_view.getViewport().height});
    borderShape.setPosition(prevView.getSize().x * m_view.getViewport().left,  
                            prevView.getSize().y * m_view.getViewport().top);
    borderShape.setFillColor(Color::Transparent);
    borderShape.setOutlineColor(Color::Black);
    borderShape.setOutlineThickness(5.f);
    target.draw(borderShape, states);

    target.setView(m_view);

    if (m_shouldRepeat) {
        Vector2f viewStart = m_view.getCenter() - m_view.getSize() / 2.f;
        Vector2f viewEnd = m_view.getCenter() + m_view.getSize() / 2.f;

        Vector2f relativeFieldPos = m_field.getPosition() - viewStart;
        Vector2f renderOffset{fmodf(relativeFieldPos.x, m_field.getWidth()), 
                              fmodf(relativeFieldPos.y, m_field.getHeight())};
    

        Vector2f renderStart = viewStart + renderOffset - m_field.getSize();

        for (float y = renderStart.y; y < viewEnd.y; y += m_field.getHeight()) {
            for (float x = renderStart.x; x < viewEnd.x; x += m_field.getWidth()) {
                RenderStates currentStates = states;
                currentStates.transform.translate(x, y);
                currentStates.transform.translate(-m_field.getPosition());

                target.draw(m_field, currentStates);
            }
        }
    } else {
        target.draw(m_field, states);
    }

    target.setView(prevView);
}

void FieldView::showGui() noexcept {
    with_Window("View") {
        if (ImGui::Checkbox("Show bots", &m_shouldDrawBots)) {
            for (Cell& cell : m_field) cell.setShouldDrawBot(m_shouldDrawBots);
        }
        if (ImGui::Checkbox("Repeat", &m_shouldRepeat)) {
            setShouldRepeat(m_shouldRepeat);
        }
         if (ImGui::Button("To center")) {
            m_view.setCenter(m_field.getPosition() + m_field.getSize() / 2.f);
        }
    }
    with_Window("Field") {
        ImGui::SliderFloat("Fill density", &m_fillDensity, 0.f, 1.f);
        if (ImGui::Button("Random fill")) {
            m_field.randomFill(m_fillDensity);
        }

        if (ImGui::Button("Clear")) {
            m_field.clear();
        }
    }
}
