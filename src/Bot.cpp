#include "Bot.h"

#include <SFML/Graphics.hpp>
using sf::Vector2f;
using sf::Color;
using sf::RenderTarget;
using sf::RenderStates;

Bot::Bot(Vector2f position, Color color) noexcept : m_shape{{0.7f, 0.7f}} {
    m_shape.setOrigin(-0.15f, -0.15f);
    m_shape.setPosition(position);
    m_shape.setFillColor(color);
}

void Bot::draw(RenderTarget& target, RenderStates states) const noexcept {
    target.draw(m_shape, states);
}