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

    void setShouldDrawBorder(bool shouldDrawBorder) noexcept {
        m_shape.setOutlineThickness(shouldDrawBorder ? -0.05f : 0.f);
    }

    void draw(sf::RenderTarget& target, sf::RenderStates states) const noexcept override {
        target.draw(m_shape, states);
        if (m_bot) target.draw(*m_bot, states);
    }
private:
    sf::RectangleShape m_shape;
    std::unique_ptr<Bot> m_bot;

    void setColor(sf::Color color) noexcept;
};

#endif