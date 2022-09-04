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

#include <utility>
using std::swap;

#include <limits>
using std::numeric_limits;

#include <cmath>
using std::abs;

#include <cassert>

Field::Field(int width, int height, uint64_t seed) : 
        m_width{width}, m_height{height}, m_topology{Topology::TORUS}, m_cells{width * height}, 
        m_epoch{0},  m_lifetime{256}, m_mutationChance{0.001},
        m_energyGain{10.0}, m_multiplyCost{20.0}, m_startEnergy{10.0}, m_killGainRatio{0.5},
        m_view{nullptr}, m_borderShape{{static_cast<float>(width), static_cast<float>(height)}}, 
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
    for (int i = 0; i < 256; ++ i) (*species)[i] = 0;
    (*species)[0] = 4;
    (*species)[1] = 2;
    (*species)[2] = 0b10001;
    (*species)[3] = 11;
    (*species)[4] = 0b0100000000;
    (*species)[5] = 0b1100000000 + 4;
    (*species)[6] = 0b10000;
    (*species)[7] = 9;
    (*species)[8] = 0b1100000000 + 4;
    (*species)[9] = 0b1100000000 + 8;
    (*species)[10] = 0b10000;
    (*species)[11] = 7;
    (*species)[12] = 0b10000;
    (*species)[13] = 3;
    (*species)[14] = 0b0100000000;
    (*species)[15] = 8;
    (*species)[16] = 0b10000;
    (*species)[17] = 3;
    (*species)[18] = 0b0100000000;
    at(2, 100).createBot(0, m_startEnergy, species);
}

bool Field::makeIndicesSafe(int& x, int& y, int* rotation) const noexcept {
    if (rotation) *rotation %= 8;

    switch (m_topology) {
    case Topology::TORUS:
        x %= m_width;
        if (x < 0) x += m_width;

        y %= m_height;
        if (y < 0) y += m_height;
        return true;
    case Topology::PLANE:
        return IntRect(0, 0, m_width, m_height).contains(x, y);
    case Topology::CYLINDER_Y:
        y %= m_height;
        if (y < 0) y += m_height;
        return 0 <= x && x < m_width;
    case Topology::CYLINDER_X:
        x %= m_width;
        if (x < 0) x += m_width;
        return 0 <= y && y < m_height;
    case Topology::SPHERE_LEFT:
        assert(m_width == m_height);

        x %= 2 * m_width;
        if (x < 0) x += 2 * m_width;

        y %= 2 * m_height;
        if (y < 0) y += 2 * m_height;

        if (x < m_width) {
            if (y >= m_height) {
                swap(x, y);
                x = 2 * m_width - x - 1;

                if (rotation) {
                    *rotation += 6;
                    *rotation %= 8;
                }
            }
        } else {
            if (y < m_height) {
                swap(x, y);
                y = 2 * m_height - y - 1;

                if (rotation) {    
                    *rotation += 2;
                    *rotation %= 8;
                }
            } else {
                x = 2 * m_width - x - 1;
                y = 2 * m_height - y - 1;

                if (rotation) {    
                    *rotation += 4;
                    *rotation %= 8;
                }
            }
        }
        return true;
    case Topology::SPHERE_RIGHT:
        assert(m_width == m_height);

        x %= 2 * m_width;
        if (x < 0) x += 2 * m_width;

        y %= 2 * m_height;
        if (y < 0) y += 2 * m_height;

        if (x < m_width) {
            if (y >= m_height) {
                swap(x, y);
                x = x - m_height;
                y = m_height - y - 1;

                if (rotation) {   
                    *rotation += 2;
                    *rotation %= 8;
                }
            }
        } else {
            if (y < m_height) {
                swap(x, y);
                x = m_width - x - 1;
                y = y - m_width;

                if (rotation) {   
                    *rotation += 6;
                    *rotation %= 8;
                }
            } else {
                x = 2 * m_width - x - 1;
                y = 2 * m_height - y - 1;

                if (rotation) {
                    *rotation += 4;
                    *rotation %= 8;
                }
            }
        }
        return true;
    case Topology::CONE_LEFT_TOP:
        assert(m_width == m_height);

        if (!IntRect{-m_width, -m_height, 2 * m_width, 2 * m_height}.contains(x, y))
            return false;

        if (x < 0) {
            if (y < 0) {
                x = -x - 1, y = -y - 1;

                if (rotation) {
                    *rotation += 4;
                    *rotation %= 8;
                }
            } else {
                swap(x, y);
                y = -y - 1;

                if (rotation) {    
                    *rotation += 2;
                    *rotation %= 8;
                }
            }
        } else if (y < 0) {
            swap(x, y);
            x = -x - 1;

            if (rotation) {    
                *rotation += 6;
                *rotation %= 8;
            }
        }
        return true;
    case Topology::CONE_RIGHT_TOP:
        assert(m_width == m_height);

        if (!IntRect{0, -m_height, 2 * m_width, 2 * m_height}.contains(x, y))
            return false;

        if (x >= m_width) {
            if (y < 0) {
                x = 2 * m_width - x - 1, y = -y - 1;

                if (rotation) {
                    *rotation += 4;
                    *rotation %= 8;
                }
            } else {
                swap(x, y);
                x = m_width - x - 1, y = y - m_height;

                if (rotation) {    
                    *rotation += 6;
                    *rotation %= 8;
                }
            }
        } else if (y < 0) {
            swap(x, y);
            x = x + m_width, y = m_width - y - 1;

            if (rotation) {    
                *rotation += 2;
                *rotation %= 8;
            }
        }
        return true;
    case Topology::CONE_LEFT_BOTTOM:
        assert(m_width == m_height);

        if (!IntRect{-m_width, 0, 2 * m_width, 2 * m_height}.contains(x, y))
            return false;

        if (x < 0) {
            if (y >= m_height) {
                x = -x - 1, y = 2 * m_width - y - 1;

                if (rotation) {
                    *rotation += 4;
                    *rotation %= 8;
                }
            } else {
                swap(x, y);
                x = m_height - x - 1, y = y + m_height;

                if (rotation) {    
                    *rotation += 6;
                    *rotation %= 8;
                }
            }
        } else if (y >= m_height) {
            swap(x, y);
            x = x - m_width, y = m_height - y - 1;

            if (rotation) {    
                *rotation += 2;
                *rotation %= 8;
            }
        }
        return true;
    case Topology::CONE_RIGHT_BOTTOM:
        assert(m_width == m_height);

        if (!IntRect{0, 0, 2 * m_width, 2 * m_height}.contains(x, y))
            return false;

        if (x < m_width) {
            if (y >= m_height) {
                swap(x, y);
                x = 2 * m_width - x - 1;

                if (rotation) {
                    *rotation += 6;
                    *rotation %= 8;
                }
            }
        } else {
            if (y < m_height) {
                swap(x, y);
                y = 2 * m_height - y - 1;

                if (rotation) {    
                    *rotation += 2;
                    *rotation %= 8;
                }
            } else {
                x = 2 * m_width - x - 1;
                y = 2 * m_height - y - 1;

                if (rotation) {    
                    *rotation += 4;
                    *rotation %= 8;
                }
            }
        }
        return true;
    }
    return false;
}

void Field::update() noexcept {
    vector<Decision> decisions;
    decisions.reserve(m_width * m_height);
    for (Cell& cell : m_cells) {
        if (cell.hasBot()) {
            decisions.push_back(cell.getBot().makeDecision(
                m_lifetime, m_energyGain, m_multiplyCost, *this, m_randomEngine));
        } else {
            decisions.emplace_back(Decision::Command::SKIP, -1);
        }
    }

    for (int y = 0; y < m_height; ++ y)
        for (int x = 0; x < m_width; ++ x) {
            int startRotation = uniform_int_distribution(0, 7)(m_randomEngine);
            for (int rotation = startRotation; rotation < startRotation + 8; ++ rotation) {
                auto [dx, dy] = getOffsetForRotation(rotation % 8);
                int xCurrent = x + dx, yCurrent = y + dy;
                int currentRotation = rotation;

                if (!makeIndicesSafe(xCurrent, yCurrent, &currentRotation)) continue;
                int rotationDelta = rotation % 8 - currentRotation;
                int index = yCurrent * m_width + xCurrent;

                Decision decision = decisions[index];
                Cell& cell = m_cells[index];
                if (!cell.isAlive() || !areOpposite(decision.direction, currentRotation)) continue;

                Bot& bot = cell.getBot();
                switch (decision.command) {
                case Decision::Command::MOVE:
                    if (!at(x, y).hasBot()) {
                        at(x, y).setBot(make_unique<Bot>(bot));
                        int botRotation = at(x, y).getBot().getRotation();
                        at(x, y).getBot().setRotation((botRotation + rotationDelta) % 8);

                        if (m_view) m_view->handleBotMoved({xCurrent, yCurrent}, {x, y});
                        cell.setShouldDie(true);
                    }
                    break;
                case Decision::Command::MULTIPLY:
                    if (!at(x, y).hasBot()) {
                        shared_ptr<Species> parent = bot.getSpecies();
                        shared_ptr<Species> offspring = parent->createMutant(
                            m_randomEngine, m_epoch, m_mutationChance);

                        at(x, y).createBot((decision.direction + rotationDelta) % 8, 
                            m_startEnergy, offspring);
                    }
                    break;
                case Decision::Command::ATTACK:
                    if (at(x, y).isAlive()) {
                        bot.setEnergy(bot.getEnergy()
                             + m_killGainRatio * at(x, y).getBot().getEnergy());
                        at(x, y).setShouldDie(true);
                    }
                    break;
                }
            }

            if (decisions[y * m_width + x].command == Decision::Command::DIE && at(x, y).isAlive()) {
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
