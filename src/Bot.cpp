/* This file is part of JCyberEvolution.

JCyberEvolution is free software: you can redistribute it and/or modify it 
under the terms of the GNU General Public License as published by the Free Software Foundation, 
either version 3 of the License, or (at your option) any later version.

JCyberEvolution is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; 
without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with JCyberEvolution. 
If not, see <https://www.gnu.org/licenses/>. */

#include "Bot.h"
#include "Cell.h"
#include "Field.h"
#include "utility.h"
#include "Decision.h"

#include <SFML/Graphics.hpp>
using sf::Vector2f;
using sf::Vector2i;
using sf::Color;
using sf::RenderTarget;
using sf::RenderStates;

#include <random>
using std::mt19937_64;

#include <algorithm>
using std::min;

#include <memory>
using std::shared_ptr;
using std::make_shared;
using std::uniform_int_distribution;

#include <cassert>

Bot::Bot() noexcept : Bot({0, 0}, 0, 0.0, nullptr) {}

Bot::Bot(Vector2i position, int rotation, double energy, shared_ptr<Species> species) noexcept : 
        m_instructionPointer{0}, m_age{0}, m_energy{energy},  
        m_position{position}, m_directionShape{{0.1f, 0.3f}} {
    setSpecies(species);

    m_directionShape.setOrigin(0.05f, 0.05f);
    m_directionShape.setPosition(position.x + 0.5f, position.y + 0.5f);
    setRotation(rotation);
}

int Bot::decodeRotation(uint16_t code, mt19937_64& randomEngine) const noexcept {
    if (code & (1 << 4)) {
        return (getRotation() + code % 8) % 8;
    } else {
        if (code & (1 << 3)) {
            return code % 8;
        } else {
            return uniform_int_distribution(0, 7)(randomEngine);
        }
    }
}

int Bot::decodeAddress(uint16_t code, mt19937_64& randomEngine) const noexcept {
    if (code & (1 << 9)) {
        return (m_instructionPointer + code % 256) % 256;
    } else {
        if (code & (1 << 8)) {
            return code % 256;
        } else {
            return uniform_int_distribution(0, 255)(randomEngine);
        }
    }
}

void Bot::executeTest(bool condition, mt19937_64& randomEngine) noexcept {
    if (condition) {
        m_instructionPointer 
            = decodeAddress((*m_species)[(m_instructionPointer + 1) % 256], randomEngine);
    } else {
        m_instructionPointer 
            = decodeAddress((*m_species)[(m_instructionPointer + 2) % 256], randomEngine);
    }
}

bool Bot::decodeCoords(uint16_t code, int& x, int& y, 
                       const Field& field, mt19937_64& randomEngine) const noexcept {
    int direction = decodeRotation((*m_species)[(m_instructionPointer + 3) % 256], randomEngine);
    Vector2i coords = m_position + getOffsetForRotation(direction);
    x = coords.x, y = coords.y;

    return field.makeIndicesSafe(x, y);
}

Decision Bot::makeDecision(Field& field) noexcept {
    if (++ m_age > field.getLifetime()) return {Decision::Action::DIE, -1};

    Decision decision{Decision::Action::SKIP, -1};

    mt19937_64& randomEngine = field.getRandomEngine();
    Cell& cell = field.at(m_position.x, m_position.y);

    bool run = true;
    while (run && m_energy > 0) {
        switch (static_cast<Bot::Instruction>(((*m_species)[m_instructionPointer]) % 16)) {
        case Instruction::MOVE:
            decision = {Decision::Action::MOVE, 
                decodeRotation((*m_species)[(m_instructionPointer + 1) % 256], 
                               field.getRandomEngine())};
            run = false;
            m_instructionPointer += 2;
            break;
        case Instruction::ROTATE: {
            int newRotation = m_rotation + 1;
            setRotation(decodeRotation((*m_species)[(m_instructionPointer + 1) % 256], 
                                       randomEngine));
            m_instructionPointer += 2;
            break;
        }
        case Instruction::JMP:
            m_instructionPointer 
                = decodeAddress((*m_species)[(m_instructionPointer + 1) % 256], randomEngine);
            break;
        case Instruction::EAT: {
            double eaten = min(field.getEatEfficiency() * cell.getGrass(), 
                               field.getEnergyGain());
            cell.setGrass(cell.getGrass() - eaten / field.getEatEfficiency());
            m_energy += eaten;
            ++ m_instructionPointer;
            break;
        }
        case Instruction::SKIP:
            decision = {Decision::Action::SKIP, -1};
            run = false;
            ++ m_instructionPointer;
            break;
        case Instruction::DIE:
            decision = {Decision::Action::DIE, -1};
            run = false;
            ++ m_instructionPointer;
            break;
        case Instruction::MULTIPLY:
            decision = {Decision::Action::MULTIPLY, 
                decodeRotation((*m_species)[(m_instructionPointer + 1) % 256], randomEngine)};
            run = false;
            m_energy -= field.getMultiplyCost();
            m_instructionPointer += 2;
            break;
        case Instruction::ATTACK:
            decision = {Decision::Action::ATTACK, 
                decodeRotation((*m_species)[(m_instructionPointer + 1) % 256], randomEngine)};
            run = false;
            m_instructionPointer += 2;
            break;
        case Instruction::TEST_EMPTY: {
            int x, y;
            if (decodeCoords((*m_species)[(m_instructionPointer + 3) % 256], 
                             x, y, field, randomEngine)) {
                executeTest(!field.at(x, y).hasBot(), randomEngine);
            } else {
                executeTest(false, randomEngine);
            }
            break;
        }
        case Instruction::TEST_ENEMY: {
            int x, y;
            if (decodeCoords((*m_species)[(m_instructionPointer + 3) % 256], 
                             x, y, field, randomEngine)) {
                const Cell& cell = field.at(x, y);
                executeTest(cell.hasBot() 
                    && computeDifference(*m_species, *cell.getBot().getSpecies()) != 0, 
                    randomEngine);
            } else {
                executeTest(false, randomEngine);
            }
            break;
        }
        case Instruction::TEST_ALLY: {
            int x, y;
            if (decodeCoords((*m_species)[(m_instructionPointer + 3) % 256], 
                             x, y, field, randomEngine)) {
                const Cell& cell = field.at(x, y);
                executeTest(field.at(x, y).hasBot()
                    && computeDifference(*m_species, *cell.getBot().getSpecies()) == 0,
                    randomEngine);
            } else {
                executeTest(false, randomEngine);
            }
            break;
        }
        case Instruction::TEST_ENERGY:
            executeTest(m_energy > (*m_species)[(m_instructionPointer + 3) % 256], 
                        randomEngine);
            break;
        default: 
            ++ m_instructionPointer;
            break;
        }

        -- m_energy;

        m_instructionPointer %= 256;
        if (m_instructionPointer < 0) m_instructionPointer += 256;
    }

    if (m_energy <= 0) return {Decision::Action::DIE, -1};

    return decision;
}
