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
using sf::IntRect;
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
        m_width{width}, m_height{height}, m_topology{Topology::TORUS}, m_cells{width * height}, 
        m_epoch{0}, m_lifetime{256}, m_mutationChance{0.001}, m_view{nullptr}, m_shouldDrawBorder{false},
        m_borderShape{{static_cast<float>(width), static_cast<float>(height)}}, 
        m_randomEngine{seed} {
    m_borderShape.setFillColor(Color::Transparent);
    m_borderShape.setOutlineColor(Color::Black);
    m_borderShape.setOutlineThickness(1.f);

    for (int y = 0; y < m_height; ++ y) {
        for (int x = 0; x < m_width; ++ x) {
            emplace(x, y, Color::Green);
        }
    }

    auto species = make_shared<Species>(Color::Red);
    for (int i = 0; i < 4; ++ i) (*species)[i] = 3;
    (*species)[4] = 8;
    for (int i = 5; i < 10; ++ i) (*species)[i] = 1;
    (*species)[11] = 7;
    for (int i = 12; i < 256; ++ i) (*species)[i] = 0;
    at(2, 2).createBot(0, species);
}

bool Field::makeIndicesSafe(int& x, int& y) const noexcept {
    switch (m_topology) {
    case Topology::TORUS:
        y %= m_height;
        if (y < 0) y += m_height;

        x %= m_width;
        if (x < 0) x += m_width;
        return true;
    case Topology::PLANE:
        return IntRect(0, 0, m_width, m_height).contains(x, y);
    }
    return false;
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

    for (int y = 0; y < m_height; ++ y)
        for (int x = 0; x < m_width; ++ x) {
            int startRotation = uniform_int_distribution(0, 7)(m_randomEngine);
            for (int rotation = startRotation; rotation < startRotation + 8; ++ rotation) {
                auto [dx, dy] = getOffsetForRotation(rotation % 8);
                int xCurrent = x + dx, yCurrent = y + dy;

                if (!makeIndicesSafe(xCurrent, yCurrent)) continue;
                int index = yCurrent * m_width + xCurrent;

                Decision decision = decisions[index];
                Cell& cell = m_cells[index];
                if (!cell.hasBot()) continue;

                Bot& bot = cell.getBot();
                // Skip if not rotated to current cell
                if (!areOpposite(bot.getRotation(), rotation % 8)) continue;

                switch (decision.instruction) {
                case 1:
                    if (!at(x, y).hasBot()) {
                        at(x, y).setBot(make_unique<Bot>(bot));
                        if (m_view) m_view->handleBotMoved({xCurrent, yCurrent}, {x, y});
                        cell.setShouldDie(true);
                    }
                    break;
                case 2:
                    if (!at(x, y).hasBot()) {
                        shared_ptr<Species> parent = bot.getSpecies();
                        shared_ptr<Species> offspring = parent->createMutant(
                            m_randomEngine, m_epoch, m_mutationChance);

                        at(x, y).createBot(bot.getRotation(), offspring);
                    }
                    break;
                case 4:
                    if (at(x, y).isAlive()) {
                        at(x, y).setShouldDie(true);
                    }
                    break;
                default: break;
                }
            }

            if (decisions[y * m_width + x].instruction == 3 && at(x, y).isAlive()) {
                at(x, y).setShouldDie(true);
            }
    }

    for (int y = 0; y < m_height; ++ y)
        for (int x = 0; x < m_width; ++ x) {
            if (at(x, y).checkShouldDie()) {
                if (m_view) m_view->handleBotDied({x, y});
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

    for (int y = 0; y < m_height; ++ y)
        for (int x = 0; x < m_width; ++ x) {
            target.draw(at(x, y), states);
    }
    if (m_topology == Field::Topology::PLANE) target.draw(m_borderShape, states);
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
