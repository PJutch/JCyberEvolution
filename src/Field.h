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

    int computePopulation() const noexcept;

    int getLifetime() const noexcept {
        return m_lifetime;
    }

    void setLifetime(int lifetime) noexcept {
        m_lifetime = lifetime;
    }

    double getMutationChance() const noexcept {
        return m_mutationChance;
    }

    void setMutationChance(double mutationChance) noexcept {
        m_mutationChance = mutationChance;
    }

    double getEnergyGain() const noexcept {
        return m_energyGain;
    }

    void setEnergyGain(double energyGain) noexcept {
        m_energyGain = energyGain;
    }

    double getMultiplyCost() const noexcept {
        return m_multiplyCost;
    }

    void setMultiplyCost(double multiplyCost) noexcept {
        m_multiplyCost = multiplyCost;
    }

    double getStartEnergy() const noexcept {
        return m_startEnergy;
    }

    void setStartEnergy(double startEnergy) noexcept {
        m_startEnergy = startEnergy;
    }

    double getKillGainRatio() const noexcept {
        return m_killGainRatio;
    }

    void setKillGainRatio(double killGainRatio) noexcept {
        m_killGainRatio = killGainRatio;
    }

    double getEatEfficiency() const noexcept {
        return m_eatEfficiency;
    }

    void setEatEfficiency(double eatEfficiency) noexcept {
        m_eatEfficiency = eatEfficiency;
    }

    double getGrassGrowth() const noexcept {
        return m_grassGrowth;
    }

    void setGrassGrowth(double grassGrowth) noexcept {
        m_grassGrowth = grassGrowth;
    }

    double getGrassSpread() const noexcept {
        return m_grassSpread;
    }

    void setGrassSpread(double grassSpread) noexcept {
        m_grassSpread = grassSpread;
    }

    bool isEatLong() const noexcept {
        return m_eatLong;
    }

    void setEatLong(bool eatLong) noexcept {
        m_eatLong = eatLong;
    }

    double getUsedEnergyOrganicRatio() const noexcept {
        return m_usedEnergyOrganicRatio;
    }

    void setUsedEnergyOrganicRatio(double usedEnergyOrganicRatio) noexcept {
        m_usedEnergyOrganicRatio = usedEnergyOrganicRatio;
    }

    double getEatenOrganicRatio() const noexcept {
        return m_eatenOrganicRatio;
    }

    void setEatenOrganicRatio(double eatenOrganicRatio) noexcept {
        m_eatenOrganicRatio = eatenOrganicRatio;
    }

    double getKillOrganicRatio() const noexcept {
        return m_killOrganicRatio;
    }

    void setKillOrganicRatio(double killOrganicRatio) noexcept {
        m_killOrganicRatio = killOrganicRatio;
    }

    double getOrganicGrassRatio() const noexcept {
        return m_organicGrassRatio;
    }

    void setOrganicGrassRatio(double organicGrassRatio) noexcept {
        m_organicGrassRatio = organicGrassRatio;
    }

    double getDiedOrganicRatio() const noexcept {
        return m_diedOrganicRatio;
    }

    void setDiedOrganicRatio(double diedOrganicRatio) noexcept {
        m_diedOrganicRatio = diedOrganicRatio;
    }

    double getOrganicSpread() const noexcept {
        return m_organicSpread;
    }

    void setOrganicSpread(double organicSpread) noexcept {
        m_organicSpread = organicSpread;
    }

    double getOrganicSpoil() const noexcept {
        return m_organicSpoil;
    }

    void setOrganicSpoil(double organicSpoil) noexcept {
        m_organicSpoil = organicSpoil;
    }

    double getGrassDeath() const noexcept {
        return m_grassDeath;
    }

    void setGrassDeath(double grassDeath) noexcept {
        m_grassDeath = grassDeath;
    }

    double getDeadGrassOrganicRatio() const noexcept {
        return m_deadGrassOrganicRatio;
    }

    void setDeadGrassOrganicRatio(double deadGrassOrganicRatio) noexcept {
        m_deadGrassOrganicRatio = deadGrassOrganicRatio;
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

    void update() noexcept;

    void setView(FieldView* view) noexcept {
        m_view = view;
    }
private:
    int m_width;
    int m_height;
    Topology m_topology;

    std::vector<Cell> m_cells;
    int m_epoch;

    int m_lifetime;
    double m_mutationChance;
    double m_energyGain;
    double m_multiplyCost;
    double m_startEnergy;
    double m_killGainRatio;
    double m_eatEfficiency;
    double m_grassGrowth;
    double m_grassSpread;
    bool m_eatLong;
    double m_usedEnergyOrganicRatio;
    double m_eatenOrganicRatio;
    double m_killOrganicRatio;
    double m_diedOrganicRatio;
    double m_organicGrassRatio;
    double m_organicSpread;
    double m_organicSpoil;
    double m_grassDeath;
    double m_deadGrassOrganicRatio;

    FieldView* m_view;

    sf::RectangleShape m_borderShape;

    std::mt19937_64 m_randomEngine;
};

#endif