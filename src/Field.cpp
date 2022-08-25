/* This file is part of JCyberEvolution.

JCyberEvolution is free software: you can redistribute it and/or modify it 
under the terms of the GNU General Public License as published by the Free Software Foundation, 
either version 3 of the License, or (at your option) any later version.

JCyberEvolution is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; 
without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with JCyberEvolution. 
If not, see <https://www.gnu.org/licenses/>. */

#include "Field.h"
#include "Cell.h"
#include "Bot.h"

#include <SFML/Graphics.hpp>
using sf::RenderTarget;
using sf::RenderStates;
using sf::Color;
using sf::Vector2f;
using sf::Uint32;
using sf::Uint8;

#include <random>
using std::uniform_int_distribution;
using std::uniform_real_distribution;

#include <memory>
using std::make_unique;

#include <limits>
using std::numeric_limits;

Field::Field(int width, int height, uint64_t seed) : 
        m_width{width}, m_height{height}, m_cells{width * height}, m_shouldDrawBorder{false},
        m_borderShape{{static_cast<float>(width), static_cast<float>(height)}}, m_randomEngine{seed} {
    m_borderShape.setFillColor(Color::Transparent);
    m_borderShape.setOutlineColor(Color::Black);
    m_borderShape.setOutlineThickness(1.f);

    for (int i = 0; i < m_width; ++ i) {
        for (int j = 0; j < m_height; ++ j) {
            emplace(i, j, Color::Green);
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
    if (m_shouldDrawBorder) target.draw(m_borderShape, states);
}

void Field::randomFill(float density) noexcept {
    clear();

    for (Cell& cell : m_cells) {
        if (uniform_real_distribution<float>(0.f, 1.f)(m_randomEngine) < density) {
            Color color{uniform_int_distribution<Uint32>()(m_randomEngine)};
            color.a = numeric_limits<Uint8>::max();
            int rotation = uniform_int_distribution(0, 7)(m_randomEngine);
            cell.createBot(color, rotation * 45.f);
        }
    }
}

void Field::clear() noexcept {
    for (Cell& cell : m_cells) {
        cell.deleteBot();
    }
}
