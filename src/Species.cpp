#include "Species.h"

#include <SFML/Graphics.hpp>
using sf::Color;

using sf::Uint8;
using sf::Uint32;

#include <random>
using std::uniform_int_distribution;
using std::mt19937_64;

#include <memory>
using std::shared_ptr;
using std::make_shared;

#include <limits>
using std::numeric_limits;

using std::ssize;

Species::Species(sf::Color color) noexcept : m_color{color}, m_genome{} {}

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
