/* This file is part of JCyberEvolution.

JCyberEvolution is free software: you can redistribute it and/or modify it 
under the terms of the GNU General Public License as published by the Free Software Foundation, 
either version 3 of the License, or (at your option) any later version.

JCyberEvolution is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; 
without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with JCyberEvolution. 
If not, see <https://www.gnu.org/licenses/>. */

#ifndef SPECIES_H_
#define SPECIES_H_

#include <SFML/Graphics.hpp>

#include <random>
#include <array>
#include <memory>
#include <iostream>

class Species : public std::enable_shared_from_this<Species> {
public:
    Species() noexcept;
    explicit Species(sf::Color color) noexcept;

    static std::shared_ptr<Species> createRandom(std::mt19937_64& randomEngine) noexcept;

    // return this if no mutation
    std::shared_ptr<Species> createMutant(std::mt19937_64& randomEngine, int epoch) noexcept;

    // unsafe, check index by yourself
    uint16_t& operator[] (int i) noexcept {
        return m_genome[i];
    }

    // unsafe, check index by yourself
    uint16_t operator[] (int i) const noexcept {
        return m_genome[i];
    }

    sf::Color getColor() const noexcept {
        return m_color;
    }

    friend std::ostream& operator<< (std::ostream& os, const Species& species) noexcept;
    friend std::istream& operator>> (std::istream& is, Species& species) noexcept;
private:
    sf::Color m_color;
    std::array<uint16_t, 256> m_genome;
};

#endif