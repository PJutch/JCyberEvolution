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