#ifndef FIELD_H_
#define FIELD_H_

#include "Cell.h"

#include <SFML/Graphics.hpp>

#include <vector>

class Field : public sf::Transformable, public sf::Drawable {
public:
    Field(int width, int height);

    int getWidth() const noexcept {
        return m_width;
    }

    int getHeight() const noexcept {
        return m_height;
    }

    sf::Vector2f getSize() const noexcept {
        return sf::Vector2f(m_width, m_height);
    }

    // unsafe, check indices by yourself
    Cell& at(int i, int j) noexcept {
        return m_cells[i * m_width + j];
    }

    // unsafe, check indices by yourself
    const Cell& at(int i, int j) const noexcept {
        return m_cells[i * m_width + j];
    }

    using iterator = std::vector<Cell>::iterator;
    using const_iterator = std::vector<Cell>::const_iterator;

    iterator begin() noexcept {
        return m_cells.begin();
    }

    const_iterator cbegin() const noexcept {
        return m_cells.cbegin();
    }

    const_iterator begin() const noexcept {
        return cbegin();
    }

    iterator end() noexcept {
        return m_cells.end();
    }

    const_iterator cend() const noexcept {
        return m_cells.cend();
    }

    const_iterator end() const noexcept {
        return cend();
    }

    void draw(sf::RenderTarget& target, sf::RenderStates states) const noexcept override;

    void setShouldDrawBorder(bool shouldDrawBorder) noexcept {
        m_shouldDrawBorder = shouldDrawBorder;
    }
private:
    int m_width;
    int m_height;

    std::vector<Cell> m_cells;

    bool m_shouldDrawBorder;
    sf::RectangleShape m_borderShape;
};

#endif