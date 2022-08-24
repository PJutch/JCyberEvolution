/* This file is part of JCyberEvolution.

JCyberEvolution is free software: you can redistribute it and/or modify it 
under the terms of the GNU General Public License as published by the Free Software Foundation, 
either version 3 of the License, or (at your option) any later version.

JCyberEvolution is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; 
without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with JCyberEvolution. 
If not, see <https://www.gnu.org/licenses/>. */

#include "Cell.h"
#include "utility.h"

#include <SFML/Graphics.hpp>
using sf::Vector2f;
using sf::Color;
using sf::RenderTarget;
using sf::RenderStates;

#include <memory>

Cell::Cell(Vector2f position, Color color) noexcept : 
        m_shape{{1, 1}}, m_bot{nullptr}, m_shouldDrawBackground{false}, m_shouldDrawBot{true} {
    m_shape.setPosition(position);
    m_shape.setFillColor(color);
    m_shape.setOutlineColor(getOutlineColorFor(color));
}

void Cell::draw(RenderTarget& target, RenderStates states) const noexcept {
    if (m_bot && m_shouldDrawBot) {
        if (m_shouldDrawBackground) {
            target.draw(m_shape, states);
            target.draw(*m_bot, states);
        } else {
            Vector2f center = m_shape.getPosition() + m_shape.getSize() / 2.f;
            states.transform.scale(m_shape.getSize().x / m_bot->getSize().x, 
                                   m_shape.getSize().y / m_bot->getSize().y, center.x, center.y);
            target.draw(*m_bot, states);
        }
    } else {
        target.draw(m_shape, states);
    }
}
