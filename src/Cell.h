/* This file is part of JCyberEvolution.

JCyberEvolution is free software: you can redistribute it and/or modify it 
under the terms of the GNU General Public License as published by the Free Software Foundation, 
either version 3 of the License, or (at your option) any later version.

JCyberEvolution is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; 
without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with JCyberEvolution. 
If not, see <https://www.gnu.org/licenses/>. */

#ifndef CELL_H_
#define CELL_H_

#include "Bot.h"

#include <SFML/Graphics.hpp>

#include <memory>
#include <utility>

class Cell {
public:
    Cell() noexcept = default;
    Cell(sf::Vector2f position) noexcept;

    bool hasBot() const noexcept {
        return static_cast<bool>(m_bot);
    }

    // unsafe
    Bot& getBot() noexcept {
        return *m_bot;
    }

    // unsafe
    const Bot& getBot() const noexcept {
        return *m_bot;
    }

    void setBot(std::unique_ptr<Bot>&& bot) noexcept {
        m_bot = std::move(bot);
        m_bot->setPosition(m_position);
    }

    void deleteBot() noexcept {
        m_bot.reset();
    }  

    template<typename... Args>
    void createBot(Args&&... args) noexcept {
        m_bot = std::make_unique<Bot>(m_position, std::forward<Args>(args)...);
    }

    bool shouldDie() {
        return m_shouldDie;
    }

    void setShouldDie(bool shouldDie) noexcept {
        m_shouldDie = shouldDie;
    }

    bool checkShouldDie() noexcept {
        if (m_shouldDie) {
            deleteBot();
            m_shouldDie = false;
            return true;
        }
        return false;
    }

    double getGrass() const noexcept {
        return m_grass;
    }

    void setGrass(double grass) noexcept {
        m_grass = grass;
    }

    double getOrganic() const noexcept {
        return m_organic;
    }

    void setOrganic(double organic) noexcept {
        m_organic = organic;
    }

    bool isAlive() const noexcept {
        return hasBot() && !m_shouldDie;
    }
private:
    std::unique_ptr<Bot> m_bot;
    bool m_shouldDie;

    double m_grass;
    double m_organic;

    sf::Vector2i m_position;
};

#endif