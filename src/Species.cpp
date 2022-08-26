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

#include <limits>
using std::numeric_limits;

#include <cmath>
using std::sin;

using std::ssize;

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

shared_ptr<Species> Species::createMutant(mt19937_64& randomEngine, int epoch) noexcept {
    std::shared_ptr<Species> result;

    uniform_int_distribution<uint16_t> genomeDistribution;
    uniform_real_distribution canonicalDistribution{0.0, 1.0};
    for (int i = 0; i < ssize(m_genome); ++ i) {
        if (canonicalDistribution(randomEngine) < 0.001) {
            if (!result) {
                result = make_shared<Species>(m_color);
                result->m_genome = m_genome;
            }

            result->m_genome[i] = genomeDistribution(randomEngine);

            if (canonicalDistribution(randomEngine) 
                < sin(static_cast<double>(epoch)) / 2.0 + 0.5) {
                if (result->m_color.r != numeric_limits<Uint8>::max()) result->m_color.r += 8;
            } else {
                if (result->m_color.r != 0) result->m_color.r -= 8;
            }

            if (canonicalDistribution(randomEngine) 
                < static_cast<double>(result->m_genome[i]) / 8.0) {
                if (result->m_color.g != numeric_limits<Uint8>::max()) result->m_color.g += 8;
            } else {
                if (result->m_color.g != 0) result->m_color.g -= 8;
            }

            if (canonicalDistribution(randomEngine) 
                < static_cast<double>(i) / 255.0) {
                if (result->m_color.b != numeric_limits<Uint8>::max()) result->m_color.b += 8;
            } else {
                if (result->m_color.b != 0) result->m_color.b -= 8;
            }
        }
    }

    if (!result) return shared_from_this();
    return result;
}
