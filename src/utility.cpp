#include "utility.h"

#include <SFML/Graphics.hpp>
using sf::Color;

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