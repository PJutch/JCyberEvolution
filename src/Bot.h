/* This file is part of JCyberEvolution.

JCyberEvolution is free software: you can redistribute it and/or modify it 
under the terms of the GNU General Public License as published by the Free Software Foundation, 
either version 3 of the License, or (at your option) any later version.

JCyberEvolution is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; 
without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with JCyberEvolution. 
If not, see <https://www.gnu.org/licenses/>. */

#ifndef BOT_H_
#define BOT_H_

#include <SFML/Graphics.hpp>

class Bot : public sf::Drawable {
public:
    Bot(sf::Vector2f position, sf::Color color, float rotation) noexcept;

    sf::Vector2f getSize() const noexcept {
        return m_shape.getSize();
    }

    void draw(sf::RenderTarget& target, sf::RenderStates states) const noexcept override {
        target.draw(m_shape, states);
        if (m_shouldDrawDirection) target.draw(m_directionShape, states);
    }

    void setShouldDrawOutline(bool shouldDrawOutline) noexcept {
        m_shape.setOutlineThickness(shouldDrawOutline ? -0.07f : 0.f);
    }

    void setShouldDrawDirection(bool shouldDrawOutline) noexcept {
        m_shouldDrawDirection = shouldDrawOutline;
    }
private:
    sf::RectangleShape m_shape;
    sf::RectangleShape m_directionShape;

    bool m_shouldDrawDirection;
};

#endif