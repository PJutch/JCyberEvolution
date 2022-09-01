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
#include "utility.h"

#include <SFML/Graphics.hpp>
using sf::Vector2f;
using sf::Vector2i;
using sf::Color;
using sf::RenderTarget;
using sf::RenderStates;

#include <random>
using std::mt19937_64;

#include <memory>
using std::shared_ptr;
using std::make_shared;
using std::uniform_int_distribution;

#include <cassert>

Bot::Bot() noexcept : Bot({0, 0}, 0, nullptr) {}

Bot::Bot(Vector2i position, int rotation, shared_ptr<Species> species) noexcept : 
        m_instructionPointer{0}, m_age{0}, m_directionShape{{0.1f, 0.3f}} {
    setSpecies(species);

    m_directionShape.setOrigin(0.05f, 0.05f);
    m_directionShape.setPosition(position.x + 0.5f, position.y + 0.5f);
    setRotation(rotation);
}

int Bot::decodeRotation(uint16_t code, mt19937_64& randomEngine) const noexcept {
    if (code & (1 << 5)) {
        return getRotation() + code % 8;
    } else {
        if (code & (1 << 4)) {
            return code % 8;
        } else {
            return uniform_int_distribution(0, 7)(randomEngine);
        }
    }
}

int Bot::decodeAddress(uint16_t code, mt19937_64& randomEngine) const noexcept {
    if (code & (1 << 10)) {
        return m_instructionPointer + code % 256;
    } else {
        if (code & (1 << 9)) {
            return code % 256;
        } else {
            return uniform_int_distribution(0, 255)(randomEngine);
        }
    }
}

Decision Bot::makeDecision(int lifetime, mt19937_64& randomEngine) noexcept {
    if (++ m_age > lifetime) return {3};

    Decision decision{0}; // skip

    bool run = true;
    int executedCommands = 0;
    while (run && executedCommands < 10) {
        switch (((*m_species)[m_instructionPointer]) % 16) {
        case 1: // move
            decision = {1, 
                decodeRotation((*m_species)[(m_instructionPointer + 1) % 256], randomEngine)};
            run = false;
            m_instructionPointer += 2;
            break;
        case 2: { // rotate
            int newRotation = m_rotation + 1;
            setRotation(decodeRotation((*m_species)[(m_instructionPointer + 1) % 16], 
                                       randomEngine));
            m_instructionPointer += 2;
            break;
        }
        case 3: // jump
            m_instructionPointer 
                = decodeAddress((*m_species)[(m_instructionPointer + 1) % 16], randomEngine);
            break;
        case 5: // skip
            decision = {0, -1};
            run = false;
            ++ m_instructionPointer;
            break;
        case 6: // die
            decision = {3, -1};
            run = false;
            ++ m_instructionPointer;
            break;
        case 7: // multiply
            decision = {2, 
                decodeRotation((*m_species)[(m_instructionPointer + 1) % 16], randomEngine)};
            run = false;
            m_instructionPointer += 2;
            break;
        case 8: // attack
            decision = {4, 
                decodeRotation((*m_species)[(m_instructionPointer + 1) % 16], randomEngine)};
            run = false;
            m_instructionPointer += 2;
            break;
        default: 
            ++ m_instructionPointer;
            break;
        }

        ++ executedCommands;

        m_instructionPointer %= 256;
        if (m_instructionPointer < 0) m_instructionPointer += 256;
    }

    return decision;
}
