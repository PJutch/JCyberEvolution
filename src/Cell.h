#ifndef CELL_H_
#define CELL_H_

#include <SFML/Graphics.hpp>

class Cell : public sf::Drawable {
public:
    Cell() = default;
    Cell(sf::Vector2i position, sf::Color color) noexcept;

    sf::Color getColor() const noexcept {
        return m_shape.getFillColor();
    }

    void zoom(float zoom) noexcept {
        m_shape.setOutlineThickness(zoom < 0.4f ? -0.05f : 0.f);
    }

    void draw(sf::RenderTarget& target, sf::RenderStates states) const noexcept override {
        target.draw(m_shape, states);
    }
private:
    sf::RectangleShape m_shape;

    void setColor(sf::Color color) noexcept;
};

#endif