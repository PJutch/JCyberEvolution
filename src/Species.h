#ifndef SPECIES_H_
#define SPECIES_H_

#include <SFML/Graphics.hpp>

#include <random>
#include <array>
#include <memory>

class Species {
public:
    Species(sf::Color color) noexcept;

    static std::shared_ptr<Species> createRandom(std::mt19937_64& randomEngine) noexcept;

    sf::Color getColor() const noexcept {
        return m_color;
    }
private:
    sf::Color m_color;
    std::array<uint16_t, 256> m_genome;
};

#endif