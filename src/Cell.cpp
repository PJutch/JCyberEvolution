#include "Cell.h"

#include <SFML/Graphics.hpp>
using sf::Vector2i;
using sf::Color;

Cell::Cell(sf::Vector2i position, sf::Color color) noexcept : m_shape{{SIZE, SIZE}} {
    m_shape.setPosition(static_cast<float>(position.x * SIZE), 
                        static_cast<float>(position.y * SIZE));
    setColor(color);
}

void Cell::setColor(sf::Color color) noexcept {
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
