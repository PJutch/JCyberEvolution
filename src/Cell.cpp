#include "Cell.h"

#include <SFML/Graphics.hpp>
using sf::Vector2i;
using sf::Color;

Cell::Cell(sf::Vector2i position, sf::Color color) noexcept : m_position{position}, m_shape{{SIZE, SIZE}} {
    m_shape.setPosition(position.x * SIZE, position.y * SIZE);
    m_shape.setFillColor(color);
}
