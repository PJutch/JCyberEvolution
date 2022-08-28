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
#include "FieldView.h"
#include "Cell.h"
#include "Bot.h"
#include "Species.h"
#include "Decision.h"
#include "utility.h"

#include <SFML/Graphics.hpp>
using sf::RenderTarget;
using sf::RenderStates;
using sf::Color;
using sf::Vector2f;
using sf::Vector2i;
using sf::Uint32;
using sf::Uint8;

#include <random>
using std::uniform_int_distribution;
using std::uniform_real_distribution;

#include <vector>
using std::vector;

#include <memory>
using std::make_unique;
using std::make_shared;
using std::shared_ptr;

#include <limits>
using std::numeric_limits;

#include <cmath>
using std::abs;

Field::Field(int width, int height, uint64_t seed) : 
        m_width{width}, m_height{height}, m_cells{width * height}, 
        m_epoch{0}, m_lifetime{256}, m_view{nullptr}, m_shouldDrawBorder{false},
        m_borderShape{{static_cast<float>(width), static_cast<float>(height)}}, 
        m_randomEngine{seed} {
    m_borderShape.setFillColor(Color::Transparent);
    m_borderShape.setOutlineColor(Color::Black);
    m_borderShape.setOutlineThickness(1.f);

    for (int i = 0; i < m_height; ++ i) {
        for (int j = 0; j < m_width; ++ j) {
            emplace(i, j, Color::Green);
        }
    }

    auto species = make_shared<Species>(Color::Red);
    for (int i = 0; i < 4; ++ i) (*species)[i] = 3;
    (*species)[4] = 8;
    for (int i = 5; i < 10; ++ i) (*species)[i] = 1;
    (*species)[11] = 7;
    for (int i = 12; i < 256; ++ i) (*species)[i] = 0;
    at(16, 16).createBot(0, species);
}

Vector2i Field::getSafeIndices(int i, int j) const noexcept {
    i %= m_height;
    if (i < 0) i += m_height;

    j %= m_width;
    if (j < 0) j += m_width;

    return {i, j};
}

void Field::update() noexcept {
    vector<Decision> decisions;
    decisions.reserve(m_width * m_height);
    for (Cell& cell : m_cells) {
        if (cell.hasBot()) {
            decisions.push_back(cell.getBot().makeDecision(m_lifetime));
        } else {
            decisions.emplace_back(0);
        }
    }

    for (int i = 0; i < m_height; ++ i)
        for (int j = 0; j < m_width; ++ j) {
            int startRotation = uniform_int_distribution(0, 7)(m_randomEngine);
            for (int rotation = startRotation; rotation < startRotation + 8; ++ rotation) {
                auto [dj, di] = getOffsetForRotation(rotation % 8);
                auto [iCurrent, jCurrent] = getSafeIndices(i + di, j + dj);
                int index = iCurrent * m_width + jCurrent;

                Decision decision = decisions[index];
                Cell& cell = m_cells[index];
                if (!cell.hasBot()) continue;

                Bot& bot = cell.getBot();
                // Skip if not rotated to current cell
                if (!areOpposite(bot.getRotation(), rotation % 8)) continue;

                switch (decision.instruction) {
                case 1:
                    if (!at(i, j).hasBot()) {
                        at(i, j).setBot(make_unique<Bot>(bot));
                        if (m_view) m_view->handleBotMoved({jCurrent, iCurrent}, {j, i});
                        cell.setShouldDie(true);
                    }
                    break;
                case 2:
                    if (!at(i, j).hasBot()) {
                        shared_ptr<Species> parent = bot.getSpecies();
                        shared_ptr<Species> offspring = parent->createMutant(m_randomEngine, m_epoch);

                        at(i, j).createBot(bot.getRotation(), offspring);
                    }
                    break;
                case 4:
                    if (at(i, j).isAlive()) {
                        at(i, j).setShouldDie(true);
                    }
                    break;
                default: break;
                }
            }

            if (decisions[i * m_width + j].instruction == 3 && at(i, j).isAlive()) {
                at(i, j).setShouldDie(true);
            }
    }

    for (int i = 0; i < m_height; ++ i)
        for (int j = 0; j < m_width; ++ j) {
            if (at(i, j).checkShouldDie()) {
                if (m_view) m_view->handleBotDied({j, i});
            }
    }

    ++ m_epoch;
}

int Field::computePopulation() const noexcept {
    int population = 0;
    for (const Cell& cell : m_cells) {
        if (cell.hasBot()) ++ population;
    }
    return population;
}

void Field::draw(RenderTarget& target, RenderStates states) const noexcept {
    states.transform *= getTransform();

    for (int i = 0; i < m_height; ++ i) {
        for (int j = 0; j < m_width; ++ j) {
            target.draw(at(i, j), states);
        }
    }
    if (m_shouldDrawBorder) target.draw(m_borderShape, states);
}

void Field::randomFill(float density) noexcept {
    clear();

    for (Cell& cell : m_cells) {
        if (uniform_real_distribution<float>(0.f, 1.f)(m_randomEngine) < density) {
            cell.setBot(make_unique<Bot>(Bot::createRandom(m_randomEngine)));
        }
    }
}

void Field::clear() noexcept {
    m_epoch = 0;

    for (Cell& cell : m_cells) {
        cell.deleteBot();
    }
}
