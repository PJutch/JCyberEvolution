/* This file is part of JCyberEvolution.

JCyberEvolution is free software: you can redistribute it and/or modify it 
under the terms of the GNU General Public License as published by the Free Software Foundation, 
either version 3 of the License, or (at your option) any later version.

JCyberEvolution is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; 
without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with JCyberEvolution. 
If not, see <https://www.gnu.org/licenses/>. */

#ifndef CELL_H_
#define CELL_H_

#include "Bot.h"

#include <SFML/Graphics.hpp>

#include <memory>

class Cell : public sf::Drawable {
public:
    Cell() noexcept = default;
    Cell(sf::Vector2f position, sf::Color color) noexcept;

    sf::Color getColor() const noexcept {
        return m_shape.getFillColor();
    }

    void setBot(std::unique_ptr<Bot>&& bot) noexcept {
        m_bot = std::move(bot);
    }

    void setShouldDrawOutline(bool shouldDrawOutline) noexcept {
        m_shape.setOutlineThickness(shouldDrawOutline ? -0.05f : 0.f);
    }

    void setShouldDrawBotOutline(bool shouldDrawOutline) noexcept {
        if (m_bot) m_bot->setShouldDrawOutline(shouldDrawOutline);
    }

    void setShouldDrawBotDirection(bool shouldBotDirection) noexcept {
        if (m_bot) m_bot->setShouldDrawDirection(shouldBotDirection);
    }

    // work only if bot is visible
    void setShouldDrawBackground(bool shouldDrawBackground) noexcept {
        m_shouldDrawBackground = shouldDrawBackground;
    }

    void setShouldDrawBot(bool shouldDrawBot) noexcept {
        m_shouldDrawBot = shouldDrawBot;
    }

    void draw(sf::RenderTarget& target, sf::RenderStates states) const noexcept override;
private:
    sf::RectangleShape m_shape;
    std::unique_ptr<Bot> m_bot;

    bool m_shouldDrawBackground;
    bool m_shouldDrawBot;

    void setColor(sf::Color color) noexcept;
};

#endif