#include "Bot.h"
#include "utility.h"

#include <SFML/Graphics.hpp>
using sf::Vector2f;
using sf::Color;
using sf::RenderTarget;
using sf::RenderStates;

Bot::Bot(Vector2f position, Color color) noexcept : 
        m_shape{{0.7f, 0.7f}}, m_directionShape{{0.1f, 0.3f}}, m_shouldDrawDirection{false} {
    Color outlineColor = getOutlineColorFor(color);

    m_shape.setOrigin(-0.15f, -0.15f);
    m_shape.setPosition(position);
    m_shape.setFillColor(color);
    m_shape.setOutlineColor(outlineColor);
    m_shape.setOutlineThickness(0);

    m_directionShape.setOrigin(0.05f, 0.25f);
    m_directionShape.setPosition(position + Vector2f(0.5f, 0.5f));
    m_directionShape.setFillColor(outlineColor);
}
