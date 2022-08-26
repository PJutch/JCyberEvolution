#ifndef SPECIES_H_
#define SPECIES_H_

#include <SFML/Graphics.hpp>

#include <random>
#include <array>
#include <memory>

class Species : public std::enable_shared_from_this<Species> {
public:
    Species(sf::Color color) noexcept;

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
private:
    sf::Color m_color;
    std::array<uint16_t, 256> m_genome;
};

#endif