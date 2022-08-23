#include "FieldView.h"

#include <SFML/Graphics.hpp>
using sf::FloatRect;

#include <SFML/System.hpp>
using sf::Keyboard;
using sf::Event;
using sf::Time;

#include <algorithm>
using std::clamp;

#include <cmath>
using std::pow;

FieldView::FieldView(FloatRect rect, FloatRect viewport, Field& field) : 
        m_field{field}, m_view{rect}, m_zoom{1.0f}, 
        m_baseZoomingChange{1.1f}, m_baseMovingSpeed{10.f}, m_speedModificator{10.f} {
    m_view.setViewport(viewport);
}

bool FieldView::handleEvent(const Event& event) noexcept {
    m_view.zoom(1 / m_zoom);

    float zoomChange = -event.mouseWheelScroll.delta;
    if (Keyboard::isKeyPressed(Keyboard::LShift)) zoomChange *= m_speedModificator;
    if (Keyboard::isKeyPressed(Keyboard::LControl)) zoomChange /= m_speedModificator;

    m_zoom *= pow(m_baseZoomingChange, zoomChange);
    m_zoom = clamp(m_zoom, 0.01f, 100.0f);

    m_view.zoom(m_zoom);
    m_field.zoom(m_zoom);

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
