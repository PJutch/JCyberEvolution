/* This file is part of JCyberEvolution.

JCyberEvolution is free software: you can redistribute it and/or modify it 
under the terms of the GNU General Public License as published by the Free Software Foundation, 
either version 3 of the License, or (at your option) any later version.

JCyberEvolution is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; 
without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with JCyberEvolution. 
If not, see <https://www.gnu.org/licenses/>. */

#include "utility.h"

#include <SFML/Graphics.hpp>
using sf::Color;
using sf::Vector2i;

#include <cassert>

Color getOutlineColorFor(Color color) noexcept {
    Color outlineColor;
    if (static_cast<int>(color.r) + static_cast<int>(color.g)
         + static_cast<int>(color.b) > 382) {
        outlineColor = Color::Black;
    } else {
        outlineColor = Color::White;
    }
    outlineColor.a = color.a;
    return outlineColor;
}

Vector2i getOffsetForRotation(int rotation) noexcept {
    switch (rotation) {
    case 0: return { 0,  1};
    case 1: return { 1,  1};
    case 2: return { 1,  0};
    case 3: return { 1, -1};
    case 4: return { 0, -1};
    case 5: return {-1, -1};
    case 6: return {-1,  0};
    case 7: return {-1,  1};
    }
    assert(false && "impossible rotation");
    return {0, 0};
}
