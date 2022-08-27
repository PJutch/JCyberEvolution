/* This file is part of JCyberEvolution.

JCyberEvolution is free software: you can redistribute it and/or modify it 
under the terms of the GNU General Public License as published by the Free Software Foundation, 
either version 3 of the License, or (at your option) any later version.

JCyberEvolution is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; 
without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with JCyberEvolution. 
If not, see <https://www.gnu.org/licenses/>. */

#ifndef UTILITY_H_
#define UTILITY_H_

#include <SFML/Graphics.hpp>

#include <cmath>
using std::abs;

sf::Color getOutlineColorFor(sf::Color color) noexcept;

sf::Vector2i getOffsetForRotation(int rotation) noexcept;

inline bool areOpposite(int rotation1, int rotation2) noexcept {
    return abs(rotation1 - rotation2) == 4;
}

inline int getOppositeRotation(int rotation) noexcept {
    return (rotation + 4) % 8;
}

template<typename T>
float containerGetter(void* data, int index) noexcept {
    return static_cast<float>((*static_cast<T*>(data))[index]);
}

#endif