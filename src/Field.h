/* This file is part of JCyberEvolution.

JCyberEvolution is free software: you can redistribute it and/or modify it 
under the terms of the GNU General Public License as published by the Free Software Foundation, 
either version 3 of the License, or (at your option) any later version.

JCyberEvolution is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; 
without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with JCyberEvolution. 
If not, see <https://www.gnu.org/licenses/>. */

#ifndef FIELD_H_
#define FIELD_H_

#include "Cell.h"

#include <SFML/Graphics.hpp>

#include <vector>
#include <random>

class FieldView;

class Field {
public:
    struct Settings {
        int lifetime = 256;
        float mutationChance = 0.001f;
        float energyGain = 10.0f;
        float multiplyCost = 20.0f;
        float startEnergy = 10.0f;
        float instructionCost = 0.1f;
        float killGainRatio = 0.5f;
        float eatEfficiency = 0.5f;
        float grassGrowth = 0.05f;
        float grassSpread = 0.1f;
        bool eatLong = true;
        float usedEnergyOrganicRatio = 0.5f;
        float eatenOrganicRatio = 0.5f;
        float killOrganicRatio = 0.5f;
        float diedOrganicRatio = 0.25f;
        float organicGrassRatio = 5.f;
        float organicSpread = 0.1f;
        float organicSpoil = 0.05f;
        float grassDeath = 0.05f;
        float deadGrassOrganicRatio = 0.5f;
        bool preserveEnergy = false;
    };

    enum class Topology {
        TORUS = 0,
        CYLINDER_X,
        CYLINDER_Y,
        PLANE,
        SPHERE_LEFT,
        SPHERE_RIGHT,
        CONE_LEFT_TOP,
        CONE_RIGHT_TOP,
        CONE_LEFT_BOTTOM,
        CONE_RIGHT_BOTTOM
    };

    struct Statistics {
        int population;
        float totalEnergy;
    };

    Field(int width, int height, uint64_t seed);

    int getWidth() const noexcept {
        return m_width;
    }

    int getHeight() const noexcept {
        return m_height;
    }

    sf::Vector2f getSize() const noexcept {
        return sf::Vector2f(m_width, m_height);
    }

    sf::FloatRect getRect() const noexcept {
        return sf::FloatRect(0.f, 0.f, m_width, m_height);
    }

    bool makeIndicesSafe(int& x, int& y, int* rotation = nullptr) const noexcept;

    std::mt19937_64& getRandomEngine() noexcept {
        return m_randomEngine;
    }

    int getEpoch() const noexcept {
        return m_epoch;
    }

    Statistics computeStatistics() const;

    Settings& getSettings() noexcept {
        return m_settings;
    }

    const Settings& getSettings() const noexcept {
        return m_settings;
    }

    Topology getTopology() const noexcept {
        return m_topology;
    }

    void setTopology(Topology topology) noexcept {
        m_topology = topology;
    }

    // i is y and j is x
    // unsafe, check indices by yourself
    Cell& at(int x, int y) noexcept {
        return m_cells[y * m_width + x];
    }

    // i is y and j is x
    // unsafe, check indices by yourself
    const Cell& at(int x, int y) const noexcept {
        return m_cells[y * m_width + x];
    }

    // automatically pass position to constructor
    template<typename... Args>
    void emplace(int x, int y, Args&&... args) noexcept {
        at(x, y) = Cell{sf::Vector2f(x, y), std::forward<Args>(args)...};
    }

    using iterator = std::vector<Cell>::iterator;
    using const_iterator = std::vector<Cell>::const_iterator;

    iterator begin() noexcept {
        return m_cells.begin();
    }

    const_iterator cbegin() const noexcept {
        return m_cells.cbegin();
    }

    const_iterator begin() const noexcept {
        return cbegin();
    }

    iterator end() noexcept {
        return m_cells.end();
    }

    const_iterator cend() const noexcept {
        return m_cells.cend();
    }

    const_iterator end() const noexcept {
        return cend();
    }

    void randomFill(float density) noexcept;
    void clear() noexcept;

    void update();

    void setView(FieldView* view) noexcept {
        m_view = view;
    }
private:
    int m_width;
    int m_height;
    Topology m_topology;

    std::vector<Cell> m_cells;
    int m_epoch;

    Settings m_settings;

    FieldView* m_view;

    sf::RectangleShape m_borderShape;

    std::mt19937_64 m_randomEngine;

    int computePopulation() const;
    double computeTotalEnergy() const;

    int getArea() const {
        return m_width * m_height;
    }

    std::vector<Decision> makeDecisions();

    void applyDecisions(std::vector<Decision>&& decisions);

    void applyDecisions(const std::vector<Decision>& decisions_) {
        auto decisions = decisions_;
        applyDecisions(std::move(decisions));
    }

    void updateGrass();
    void diffuseGrass();

    void fixEnergy(double shouldBe);

    void notifyDied();
};

#endif