#ifndef BOT_H_
#define BOT_H_

#include <SFML/Graphics.hpp>

class Bot : public sf::Drawable {
public:
    Bot(sf::Vector2f position, sf::Color color) noexcept;

    sf::Vector2f getSize() const noexcept {
        return m_shape.getSize();
    }

    void draw(sf::RenderTarget& target, sf::RenderStates states) const noexcept override;

    void setShouldDrawOutline(bool shouldDrawOutline) noexcept {
        m_shape.setOutlineThickness(shouldDrawOutline ? -0.035f : 0.f);
    }
private:
    sf::RectangleShape m_shape;
};

#endif