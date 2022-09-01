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

#include <memory>
using std::shared_ptr;
using std::make_shared;

#include <cassert>

Bot::Bot() noexcept : Bot({0, 0}, 0, nullptr) {}

Bot::Bot(Vector2i position, int rotation, shared_ptr<Species> species) noexcept : 
        m_instructionPointer{0}, m_age{0}, m_directionShape{{0.1f, 0.3f}} {
    setSpecies(species);

    m_directionShape.setOrigin(0.05f, 0.05f);
    m_directionShape.setPosition(position.x + 0.5f, position.y + 0.5f);
    setRotation(rotation);
}

Decision Bot::makeDecision(int lifetime) noexcept {
    if (++ m_age > lifetime) return {3};

    Decision decision{0}; // skip

    bool run = true;
    int executedCommands = 0;
    while (run && executedCommands < 10) {
        switch (((*m_species)[m_instructionPointer]) % 16) {
        case 1: // move
            decision = {1};
            run = false;
            ++ m_instructionPointer;
            break;
        case 2: { // rotate right
            int newRotation = m_rotation + 1;
            setRotation(newRotation == 8 ? 0 : newRotation);
            ++ m_instructionPointer;
            break;
        }
        case 3: { // rotate left
            int newRotation = m_rotation - 1;
            setRotation(newRotation == -1 ? 7 : newRotation);
            ++ m_instructionPointer;
            break;
        }
        case 4: // jump to start
            m_instructionPointer = 0;
        case 5: // skip
            decision = {0};
            run = false;
            ++ m_instructionPointer;
            break;
        case 6: // die
            decision = {3};
            run = false;
            ++ m_instructionPointer;
            break;
        case 7: // multiply
            decision = {2};
            run = false;
            ++ m_instructionPointer;
            break;
        case 8: // attack
            decision = {4};
            run = false;
            ++ m_instructionPointer;
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
