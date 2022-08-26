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
using sf::Color;
using sf::RenderTarget;
using sf::RenderStates;

#include <cassert>

Bot::Bot(Vector2f position, int rotation, std::shared_ptr<Species> species) noexcept : 
        m_instructionPointer{0}, m_species{species}, m_age{0}, m_rotation{rotation},
        m_shape{{0.8f, 0.8f}}, m_directionShape{{0.1f, 0.3f}}, m_shouldDrawDirection{true} {
    Color outlineColor = getOutlineColorFor(species->getColor());

    m_shape.setOrigin(-0.1f, -0.1f);
    m_shape.setPosition(position);
    m_shape.setFillColor(species->getColor());
    m_shape.setOutlineColor(outlineColor);
    m_shape.setOutlineThickness(0);

    m_directionShape.setOrigin(0.05f, 0.25f);
    m_directionShape.setPosition(position + Vector2f(0.5f, 0.5f));
    m_directionShape.setFillColor(outlineColor);
    m_directionShape.setRotation(rotation * 45.f + 180.f);
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
        case 2: // rotate right
            if (++ m_rotation == 8) {
                m_rotation = 0;
            }
            m_directionShape.setRotation(m_rotation * 45.f + 180.f);
            ++ m_instructionPointer;
            break;
        case 3: // rotate left
            if (-- m_rotation == -1) {
                m_rotation = 7;
            }
            m_directionShape.setRotation(m_rotation * 45.f + 180.f);
            ++ m_instructionPointer;
            break;
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
