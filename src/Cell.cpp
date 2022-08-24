#include "Cell.h"

#include <SFML/Graphics.hpp>
using sf::Vector2f;
using sf::Color;

#include <memory>

Cell::Cell(Vector2f position, Color color) noexcept : 
        m_shape{{1, 1}}, m_bot{nullptr} {
    m_shape.setPosition(position);
    setColor(color);
}

void Cell::setColor(Color color) noexcept {
    m_shape.setFillColor(color);

    Color outlineColor;
    if (static_cast<int>(color.r) + static_cast<int>(color.g)
         + static_cast<int>(color.b) > 382) {
        outlineColor = Color::Black;
    } else {
        outlineColor = Color::White;
    }
    outlineColor.a = color.a;
    m_shape.setOutlineColor(outlineColor);
}
