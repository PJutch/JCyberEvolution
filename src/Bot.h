#ifndef BOT_H_
#define BOT_H_

#include <SFML/Graphics.hpp>

class Bot : public sf::Drawable {
public:
    Bot(sf::Vector2f position, sf::Color color) noexcept;

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