#include "Field.h"
#include "Cell.h"
#include "Bot.h"

#include <SFML/Graphics.hpp>
using sf::RenderTarget;
using sf::RenderStates;
using sf::Color;
using sf::Vector2f;

#include <memory>
using std::make_unique;

Field::Field(int width, int height) : 
        m_width{width}, m_height{height}, m_cells{width * height}, m_shouldDrawBorder{true},
        m_borderShape{{static_cast<float>(width), static_cast<float>(height)}} {
    m_borderShape.setFillColor(Color::Transparent);
    m_borderShape.setOutlineColor(Color::Black);
    m_borderShape.setOutlineThickness(1.f);

    for (int i = 0; i < m_width; ++ i) {
        for (int j = 0; j < m_height; ++ j) {
            at(i, j) = Cell{Vector2f(i, j), Color::Green};
            at(i, j).setBot(make_unique<Bot>(Vector2f(i, j), Color::Red));
        }
    }
}

void Field::draw(RenderTarget& target, RenderStates states) const noexcept {
    states.transform *= getTransform();

    for (int i = 0; i < m_width; ++ i) {
        for (int j = 0; j < m_height; ++ j) {
            target.draw(at(i, j), states);
        }
    }
    if (m_shouldDrawBorder) target.draw(m_borderShape, states);
}
