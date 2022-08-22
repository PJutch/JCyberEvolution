#ifndef CELL_H_
#define CELL_H_

#include <SFML/Graphics.hpp>

class Cell : public sf::Drawable {
public:
    static const int SIZE = 4;

    Cell() = default;
    Cell(sf::Vector2i position, sf::Color color) noexcept;

    sf::Vector2i getPosition() const noexcept {
        return m_position;
    }

    sf::Color getColor() const noexcept {
        return m_shape.getFillColor();
    }

    void draw(sf::RenderTarget& target, sf::RenderStates states) const noexcept override {
        target.draw(m_shape);
    }
private:
    sf::Vector2i m_position;

    sf::RectangleShape m_shape;
};

#endif