/* This file is part of JCyberEvolution.

JCyberEvolution is free software: you can redistribute it and/or modify it 
under the terms of the GNU General Public License as published by the Free Software Foundation, 
either version 3 of the License, or (at your option) any later version.

JCyberEvolution is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; 
without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with JCyberEvolution. 
If not, see <https://www.gnu.org/licenses/>. */

#ifndef FIELD_H_
#define FIELD_H_

#include "Cell.h"

#include <SFML/Graphics.hpp>

#include <vector>
#include <random>

class Field : public sf::Transformable, public sf::Drawable {
public:
    Field(int width, int height, uint64_t seed);

    int getWidth() const noexcept {
        return m_width;
    }

    int getHeight() const noexcept {
        return m_height;
    }

    sf::Vector2f getSize() const noexcept {
        return sf::Vector2f(m_width, m_height);
    }

    std::mt19937_64& getRandomEngine() noexcept {
        return m_randomEngine;
    }

    // unsafe, check indices by yourself
    Cell& at(int i, int j) noexcept {
        return m_cells[i * m_width + j];
    }

    // unsafe, check indices by yourself
    const Cell& at(int i, int j) const noexcept {
        return m_cells[i * m_width + j];
    }

    // automatically pass position to constructor
    template<typename... Args>
    void emplace(int i, int j, Args&&... args) noexcept {
        at(i, j) = Cell{sf::Vector2f(i, j), std::forward<Args>(args)...};
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

    void randomFill(float density) noexcept;
    void clear() noexcept;

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

    std::mt19937_64 m_randomEngine;
};

#endif