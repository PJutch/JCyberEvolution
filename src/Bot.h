/* This file is part of JCyberEvolution.

JCyberEvolution is free software: you can redistribute it and/or modify it 
under the terms of the GNU General Public License as published by the Free Software Foundation, 
either version 3 of the License, or (at your option) any later version.

JCyberEvolution is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; 
without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with JCyberEvolution. 
If not, see <https://www.gnu.org/licenses/>. */

#ifndef BOT_H_
#define BOT_H_

#include "Species.h"
#include "Decision.h"
#include "utility.h"

#include <SFML/Graphics.hpp>

#include <random>
#include <memory>

class Bot {
public:
    Bot() noexcept;
    Bot(sf::Vector2i position, int rotation, std::shared_ptr<Species> species) noexcept;

    static Bot createRandom(std::mt19937_64& randomEngine) noexcept {
        int rotation = std::uniform_int_distribution(0, 7)(randomEngine);
        return Bot{{0, 0}, rotation, Species::createRandom(randomEngine)};
    }

    sf::Color getColor() const noexcept {
        return m_species->getColor();
    }

    int getRotation() const noexcept {
        return m_rotation;
    }

    void setRotation(int rotation) noexcept {
        m_rotation = rotation;
        m_directionShape.setRotation(-m_rotation * 45.f);
    }

    std::shared_ptr<Species> getSpecies() const noexcept {
        return m_species;
    }

    void setPosition(sf::Vector2i position) noexcept {
        m_directionShape.setPosition(position.x + 0.5f, position.y + 0.5f);
    }

    Decision makeDecision(int lifetime, std::mt19937_64& randomEngine) noexcept;

    void drawDirection(sf::RenderTarget& target, sf::RenderStates states) const noexcept {
        target.draw(m_directionShape, states);
    }

    inline friend std::ostream& operator<< (std::ostream& os, const Bot& bot) noexcept {
        os << 1 << ' '  << bot.m_instructionPointer << ' ' << bot.m_age << ' ' << *bot.m_species;
        return os;
    }

    inline friend std::istream& operator>> (std::istream& is, Bot& bot) noexcept {
        int version;
        is >> version; // check in the future
    
        auto species = std::make_shared<Species>();
        is >> bot.m_instructionPointer >> bot.m_age >> *species;
        bot.setSpecies(species);
        return is;
    }
private:
    int m_instructionPointer;
    std::shared_ptr<Species> m_species;
    int m_age;
    
    int m_rotation;

    sf::RectangleShape m_directionShape;

    void setSpecies(std::shared_ptr<Species> species) noexcept {
        m_species = species;
    }

    int decodeRotation(uint16_t code, std::mt19937_64& randomEngine) const noexcept;
};

#endif