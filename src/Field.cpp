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
#include "Topology.h"

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

#include <algorithm>
using std::max;
using std::clamp;

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
        m_width{width}, m_height{height}, m_topology{nullptr}, m_cells{width * height}, 
        m_epoch{0},  m_settings{},
        m_view{nullptr}, m_borderShape{{static_cast<float>(width), static_cast<float>(height)}}, 
        m_randomEngine{seed} {
    m_borderShape.setFillColor(Color::Transparent);
    m_borderShape.setOutlineColor(Color::Black);
    m_borderShape.setOutlineThickness(1.f);

    for (int y = 0; y < m_height; ++ y) {
        for (int x = 0; x < m_width; ++ x) {
            emplace(x, y);
            at(x, y).setGrass(255.0);
        }
    }
}

double Field::computeTotalEnergy() const {
    double totalEnergy = 0.0;
    for (const Cell& cell : m_cells) {
        totalEnergy += cell.getGrass() + m_settings.organicGrassRatio * cell.getOrganic();

        if (cell.hasBot())
            totalEnergy += cell.getBot().getEnergy() 
                * m_settings.diedOrganicRatio * m_settings.organicGrassRatio;
    }
    return totalEnergy;
}

int Field::computePopulation() const {
    int population = 0;
    for (const Cell& cell : m_cells)
        if (cell.hasBot())
            ++ population;
    return population;
}

std::vector<Decision> Field::makeDecisions() {
    std::vector<Decision> decisions;
    decisions.reserve(m_width * m_height);
    for (Cell& cell : m_cells)
        if (cell.hasBot())
            decisions.push_back(cell.getBot().makeDecision(*this));
        else
            decisions.emplace_back(Decision::Action::SKIP, -1);
    return decisions;
}

void Field::applyDecisions(std::vector<Decision>&& decisions) {
    for (int y = 0; y < m_height; ++ y)
        for (int x = 0; x < m_width; ++ x) {
            int startRotation = uniform_int_distribution(0, 7)(m_randomEngine);
            for (int rotation = startRotation; rotation < startRotation + 8; ++ rotation) {
                auto [dx, dy] = getOffsetForRotation(rotation % 8);
                int xCurrent = x + dx, yCurrent = y + dy;
                int currentRotation = rotation;

                if (!getTopology().makeIndicesSafe(xCurrent, yCurrent, currentRotation)) continue;
                int rotationDelta = rotation % 8 - currentRotation;
                int index = yCurrent * m_width + xCurrent;

                Decision decision = decisions[index];
                Cell& cell = m_cells[index];
                if (!cell.isAlive() 
                    || !areOpposite(decision.direction, currentRotation)) continue;

                Bot& bot = cell.getBot();
                switch (decision.action) {
                case Decision::Action::MOVE:
                    if (!at(x, y).hasBot()) {
                        at(x, y).setBot(make_unique<Bot>(bot));
                        int botRotation = at(x, y).getBot().getRotation();
                        at(x, y).getBot().setRotation((botRotation + rotationDelta) % 8);

                        if (m_view) m_view->handleBotMoved({xCurrent, yCurrent}, {x, y});
                        cell.setShouldDie(true);
                    }
                    break;
                case Decision::Action::MULTIPLY:
                    if (!at(x, y).hasBot()) {
                        shared_ptr<Species> parent = bot.getSpecies();
                        shared_ptr<Species> offspring = parent->createMutant(
                            m_randomEngine, m_epoch, m_settings.mutationChance);

                        at(x, y).createBot((decision.direction + rotationDelta) % 8, 
                            m_settings.startEnergy, offspring);
                    } else {
                        decisions[index].organic += m_settings.usedEnergyOrganicRatio 
                                                  * m_settings.startEnergy;
                    }
                    break;
                case Decision::Action::ATTACK:
                    if (at(x, y).isAlive()) {
                        bot.setEnergy(bot.getEnergy()
                            + m_settings.killGainRatio * max(at(x, y).getBot().getEnergy(), 0.0));
                        decision.organic += m_settings.killOrganicRatio 
                            * (1 - m_settings.killGainRatio) * max(at(x, y).getBot().getEnergy(), 0.0);
                        at(x, y).setShouldDie(true);
                        bot.handleKill();
                    }
                    break;
                }
            }

            if (decisions[y * m_width + x].action == Decision::Action::DIE 
                    && at(x, y).isAlive()) {
                at(x, y).setShouldDie(true);
                decisions[y * m_width + x].organic += m_settings.diedOrganicRatio 
                                                    * max(at(x, y).getBot().getEnergy(), 0.0);
            }

            at(x, y).setOrganic(at(x, y).getOrganic() + decisions[y * m_width + x].organic);
    }
}

void Field::updateGrass() {
    for (int y = 0; y < m_height; ++ y)
        for (int x = 0; x < m_width; ++ x) {
            int index = y * m_width + x;
            at(x, y).setOrganic((1 - m_settings.organicSpoil) * at(x, y).getOrganic());

            at(x, y).setOrganic(at(x, y).getOrganic()
                + m_settings.grassDeath * m_settings.deadGrassOrganicRatio * at(x, y).getGrass());
            at(x, y).setGrass((1 - m_settings.grassDeath) * at(x, y).getGrass());

            at(x, y).setGrass(at(x, y).getGrass()
                + m_settings.grassGrowth * m_settings.organicGrassRatio * at(x, y).getOrganic());
            at(x, y).setOrganic((1 - m_settings.grassGrowth) * at(x, y).getOrganic());
    }
}

void Field::diffuseGrass() {
    vector<double> newGrass;
    newGrass.reserve(m_width * m_height);
    for (int y = 0; y < m_height; ++ y)
        for (int x = 0; x < m_width; ++ x)
            newGrass.push_back(at(x, y).getGrass());

    vector<double> newOrganic;
    newOrganic.reserve(m_width * m_height);
    for (int y = 0; y < m_height; ++ y)
        for (int x = 0; x < m_width; ++ x)
            newOrganic.push_back(at(x, y).getOrganic());

    for (int y = 0; y < m_height; ++ y)
        for (int x = 0; x < m_width; ++ x)
            for (int rotation = 0; rotation < 8; ++ rotation) {
                auto [dx, dy] = getOffsetForRotation(rotation);
                int xCurrent = x + dx, yCurrent = y + dy;
                int currentRotation = rotation;

                if (!getTopology().makeIndicesSafe(xCurrent, yCurrent, currentRotation)) 
                    continue;
                
                int rotationDelta = rotation - currentRotation;
                int index = yCurrent * m_width + xCurrent;

                newGrass[y * m_width + x] += m_settings.grassSpread * at(xCurrent, yCurrent).getGrass();
                newGrass[index] -= m_settings.grassSpread * at(xCurrent, yCurrent).getGrass();

                newOrganic[y * m_width + x] += m_settings.organicSpread 
                                                * at(xCurrent, yCurrent).getOrganic();
                newOrganic[index] -= m_settings.organicSpread * at(xCurrent, yCurrent).getOrganic();
    }

    for (int y = 0; y < m_height; ++ y)
        for (int x = 0; x < m_width; ++ x)
            at(x, y).setGrass(clamp(newGrass[y * m_width + x], 0.0, 255.0));
    
    for (int y = 0; y < m_height; ++ y)
        for (int x = 0; x < m_width; ++ x)
            at(x, y).setOrganic(clamp(newOrganic[y * m_width + x], 0.0, 255.0));
}

void Field::fixEnergy(double shouldBe) {
    double deltaEnergy = computeTotalEnergy() - shouldBe;
    double deltaOrganic = deltaEnergy / m_settings.organicGrassRatio;
    for (Cell& cell : m_cells)
        cell.setOrganic(clamp(cell.getOrganic() - deltaOrganic / getArea(), 0.0, 255.0));
}

void Field::notifyDied() {
    for (int y = 0; y < m_height; ++ y)
        for (int x = 0; x < m_width; ++ x)
            if (at(x, y).checkShouldDie())
                if (m_view) 
                    m_view->handleBotDied({x, y});
}

void Field::update() {
    double totalEnergy = 0.f;
    if (m_settings.preserveEnergy)
        totalEnergy = computeTotalEnergy();

    applyDecisions(makeDecisions());

    updateGrass();
    diffuseGrass();

    if (m_settings.preserveEnergy) 
        fixEnergy(totalEnergy);

    notifyDied();

    ++ m_epoch;
}

Field::Statistics Field::computeStatistics() const {
    return Statistics(computePopulation(), computeTotalEnergy());
}

void Field::randomFill(float density) noexcept {
    clear();

    for (int x = 0; x < m_width; ++ x) 
        for (int y = 0; y < m_height; ++ y)
            if (uniform_real_distribution<float>(0.f, 1.f)(m_randomEngine) < density)
                at(x, y).setBot(make_unique<Bot>(Bot::createRandom({x, y}, m_randomEngine)));
}

void Field::clear() noexcept {
    m_epoch = 0;

    for (Cell& cell : m_cells) {
        cell.deleteBot();
        cell.setGrass(255.0);
        cell.setOrganic(0.0);
    }
}
