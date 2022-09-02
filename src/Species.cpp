/* This file is part of JCyberEvolution.

JCyberEvolution is free software: you can redistribute it and/or modify it 
under the terms of the GNU General Public License as published by the Free Software Foundation, 
either version 3 of the License, or (at your option) any later version.

JCyberEvolution is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; 
without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with JCyberEvolution. 
If not, see <https://www.gnu.org/licenses/>. */

#include "Species.h"

#include <SFML/Graphics.hpp>
using sf::Color;
using sf::Uint8;
using sf::Uint32;

#include <random>
using std::uniform_int_distribution;
using std::uniform_real_distribution;
using std::mt19937_64;

#include <memory>
using std::shared_ptr;
using std::make_shared;
using std::enable_shared_from_this;

#include <iostream>
using std::ostream;
using std::istream;

#include <limits>
using std::numeric_limits;

#include <cmath>
using std::sin;

using std::ssize;

Species::Species() noexcept : Species{Color::Black} {}

Species::Species(sf::Color color) noexcept : enable_shared_from_this<Species>{}, 
    m_color{color}, m_genome{} {}

shared_ptr<Species> Species::createRandom(mt19937_64& randomEngine) noexcept {
    Color color{uniform_int_distribution<Uint32>()(randomEngine)};
    color.a = numeric_limits<Uint8>::max();

    auto result = make_shared<Species>(color);

    uniform_int_distribution<uint16_t> genomeDistribution;
    for (int i = 0; i < ssize(result->m_genome); ++ i) {
        result->m_genome[i] = genomeDistribution(randomEngine);
    }
    return result;
}

shared_ptr<Species> Species::createMutant(mt19937_64& randomEngine, int epoch, double mutationChance) noexcept {
    std::shared_ptr<Species> result;

    uniform_int_distribution<uint16_t> genomeDistribution;
    uniform_real_distribution canonicalDistribution{0.0, 1.0};
    for (int i = 0; i < ssize(m_genome); ++ i) {
        if (canonicalDistribution(randomEngine) < mutationChance) {
            if (!result) {
                result = make_shared<Species>(m_color);
                result->m_genome = m_genome;
            }

            result->m_genome[i] = genomeDistribution(randomEngine);

            if (canonicalDistribution(randomEngine) 
                < sin(static_cast<double>(epoch / 100.f)) / 2.0 + 0.5) {
                if (result->m_color.r != numeric_limits<Uint8>::max()) result->m_color.r += 1;
            } else {
                if (result->m_color.r != 0) result->m_color.r -= 1;
            }

            if (canonicalDistribution(randomEngine) 
                < static_cast<double>(result->m_genome[i] % 16) / 16.0) {
                if (result->m_color.g != numeric_limits<Uint8>::max()) result->m_color.g += 1;
            } else {
                if (result->m_color.g != 0) result->m_color.g -= 1;
            }

            if (canonicalDistribution(randomEngine) 
                < static_cast<double>(i) / 255.0) {
                if (result->m_color.b != numeric_limits<Uint8>::max()) result->m_color.b += 1;
            } else {
                if (result->m_color.b != 0) result->m_color.b -= 1;
            }
        }
    }

    if (!result) return shared_from_this();
    return result;
}

int computeDifference(const Species& lhs, const Species& rhs) noexcept {
    int difference = 0;
    for (int i = 0; i < ssize(lhs.m_genome); ++ i) {
        if (lhs[i] != rhs[i]) ++ difference;
    }
    return difference;
}

ostream& operator<< (ostream& os, const Species& species) noexcept {
    os << 1 << ' ' << species.m_color.toInteger();
    for (uint16_t value : species.m_genome) {
        os << ' ' << value;
    }
    return os;
}

istream& operator>> (istream& is, Species& species) noexcept {
    int version;
    is >> version;

    Uint32 color;
    is >> color;
    species.m_color = Color{color};

    for (uint16_t& value : species.m_genome) {
        is >> value;
    }
    return is;
}
