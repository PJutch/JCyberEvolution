#include "Field.h"
#include "Cell.h"

#include <SFML/Graphics.hpp>
using sf::RenderTarget;
using sf::RenderStates;
using sf::Color;

Field::Field(int width, int height) : 
        m_width{width}, m_height{height}, m_cells{width * height} {
    for (int i = 0; i < m_width; ++ i) {
        for (int j = 0; j < m_height; ++ j) {
            at(i, j) = Cell{{i, j}, Color::Red};
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
}

void Field::zoom(float zoom) noexcept {
    for (Cell& cell : m_cells) {
        cell.zoom(zoom);
    }
}
