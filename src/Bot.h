#ifndef BOT_H_
#define BOT_H_

#include <SFML/Graphics.hpp>

class Bot : public sf::Drawable {
public:
    Bot(sf::Vector2f position, sf::Color color) noexcept;

    void draw(sf::RenderTarget& target, sf::RenderStates states) const noexcept override;
private:
    sf::RectangleShape m_shape;
};

#endif